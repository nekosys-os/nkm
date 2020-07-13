//
// Created by Twometer on 13/07/2020.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "runner.h"

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

void exec_tool(runner_ctx *ctx, nks_tool *tool, vector *params) {
    
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