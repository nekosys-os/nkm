//
// Created by Twometer on 13/07/2020.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "runner.h"
#include "stringbuilder.h"

runner_ctx *runner_create(const char *target, vector *build_script) {
    runner_ctx *ctx = malloc(sizeof(runner_ctx));
    ctx->failed = false;
    ctx->build_script = build_script;
    ctx->target = target;
    return ctx;
}

void runner_exec(runner_ctx *ctx) {
    bool executed = false;
    for (int i = 0; i < ctx->build_script->size; i++) {
        vector_node *node = ctx->build_script->nodes[i];

        if (node->node_type == TYPE_NKS_TARGET) {
            nks_target *candidate = node->node_ptr;
            if (ctx->target == NULL || strcmp(candidate->name, ctx->target) == 0) {
                runner_run_target(ctx, candidate);

                if (ctx->failed)
                    return;

                executed = true;
            }
        }
    }

    if (!executed) {
        runner_fail(ctx, "Target not found, nothing to do.");
        return;
    }
}

void runner_run_target(runner_ctx *ctx, nks_target *target) {
    printf("info: Running target %s\n", target->name);

    for (int i = 0; i < target->commands->size; i++) {
        vector_node *node = target->commands->nodes[i];
        if (node->node_type == TYPE_NKS_RAW_COMMAND) {
            exec_command(ctx, node->node_ptr);
        } else if (node->node_type == TYPE_NKS_TOOL_INVOCATION) {
            nks_tool_invocation *invocation = node->node_ptr;
            nks_tool *tool = find_tool(ctx, invocation->tool_name);
            if (!tool) {
                runner_fail(ctx, "Cannot find tool!");
                return;
            }
            exec_tool(ctx, tool, invocation->params);
        }

        if (ctx->failed)
            return;
    }
}

nks_tool_param *find_param(vector *params, const char *name) {
    for (int i = 0; i < params->size; i++) {
        nks_tool_param *param = params->nodes[i]->node_ptr;
        if (strcmp(param->key, name) == 0)
            return param;
    }
    return NULL;
}

nks_constant *find_constant(runner_ctx *ctx, const char *name) {
    for (int i = 0; i < ctx->build_script->size; i++) {
        vector_node *node = ctx->build_script->nodes[i];
        if (node->node_type == TYPE_NKS_CONSTANT) {
            nks_constant *constant = node->node_ptr;
            if (strcmp(constant->name, name) == 0)
                return constant;
        }
    }
    return NULL;
}

void exec_tool(runner_ctx *ctx, nks_tool *tool, vector *params) {
    const char *command_template = tool->command;
    int command_template_len = strlen(command_template);

    string_builder *cmd_builder = sb_create(command_template_len);
    string_builder *argument_builder = sb_create(8);
    string_builder *default_val_builder = sb_create(8);

    char prev_char = '\0';
    bool in_argument = false;
    bool in_default_val = false;
    for (int i = 0; i < command_template_len; i++) {
        char chr = command_template[i];
        if (chr == '{' && prev_char != '\\') {
            in_argument = true;
        } else if (chr == '}' && prev_char != '\\') {
            in_argument = false;
            in_default_val = false;
            nks_tool_param *param = find_param(params, argument_builder->text);
            if (param == NULL) {
                if (default_val_builder->size > 0) {
                    sb_append_str(cmd_builder, default_val_builder->text);
                } else {
                    nks_constant *constant = find_constant(ctx, argument_builder->text);
                    if (constant == NULL) {
                        runner_fail(ctx, "Parameter without default value not supplied or Undefined reference!");
                        return;
                    } else {
                        sb_append_str(cmd_builder, constant->value);
                    }
                }
            } else {
                for (int j = 0; j < param->values->size; j++) {
                    const char *value = param->values->nodes[j]->node_ptr;
                    sb_append_str(cmd_builder, value);
                    sb_append_chr(cmd_builder, ' ');
                }
            }

            if (default_val_builder->size > 0)
                sb_reset(default_val_builder);
            sb_reset(argument_builder);
        } else if (chr == '=' && prev_char != '\\') {
            in_default_val = true;
        } else if (!in_argument) {
            sb_append_chr(cmd_builder, chr);
        } else {
            sb_append_chr(in_default_val ? default_val_builder : argument_builder, chr);
        }

        prev_char = chr;
    }

    printf("%s\n", cmd_builder->text);

    free(cmd_builder);
}

void runner_fail(runner_ctx *ctx, const char *message) {
    ctx->failed = true;
    printf("error: %s\n", message);
}

void runner_destroy(runner_ctx *ctx) {
    free(ctx);
}

void exec_command(runner_ctx *ctx, const char *command) {
    int statusCode = system(command);
    if (statusCode != 0)
        runner_fail(ctx, "Command returned with non-zero exit code!");
}

nks_tool *find_tool(runner_ctx *ctx, const char *tool) {
    for (int i = 0; i < ctx->build_script->size; i++) {
        vector_node *node = ctx->build_script->nodes[i];

        if (node->node_type == TYPE_NKS_TOOL) {
            nks_tool *candidate = node->node_ptr;
            if (strcmp(candidate->name, tool) == 0) {
                return candidate;
            }
        }
    }
    return NULL;
}