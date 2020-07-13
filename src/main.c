#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "options.h"
#include "version.h"
#include "parser.h"

void print_version() {
    printf("%s (%s) v%s by %s \n", NKM_NAME_LONG, NKM_NAME_SHORT, NKM_VERSION, NKM_AUTHOR);
}

void print_help() {
    printf("%s\n", NKM_NAME_LONG);
    printf("  nkm -version: Show version information\n");
    printf("  nkm -help: Show this help page\n");
    printf("  nkm <target>: Build a target in the current directory\n");
    printf("  nkm: Runs all targets in the current directory\n");
}

void run_builder(nkm_options *options) {
    printf("Building target %s in script %s...\n", options->target, options->build_file);

    if (access(options->build_file, F_OK) == -1) {
        printf("error: No build file found in current directory\n");
        return;
    }

    parser_ctx *ctx;
    int status = parser_init(options, &ctx);

    if (status != 0) {
        printf("error: Cannot open build file");
        return;
    }

    parser_parse(ctx);

    if (ctx->failed) {
        printf("fatal: Invalid build script!");
        return;
    }
    printf("Parser completed!\n");

    vector *build_script = ctx->build_script;
    for (int i = 0; i < build_script->size; i++) {
        vector_node *node = build_script->nodes[i];

        if (node->node_type == TYPE_NKS_CONSTANT) {
            nks_constant *constant = node->node_ptr;
            printf("CONSTANT: %s=%s\n", constant->name, constant->value);
        } else if (node->node_type == TYPE_NKS_TOOL) {
            nks_tool *tool = node->node_ptr;
            printf("TOOLDEF: %s (%d): %s\n", tool->name, tool->invoke_type, tool->command);
        } else if (node->node_type == TYPE_NKS_TARGET) {
            nks_target *target = node->node_ptr;
            vector *commands = target->commands;
            printf("TARGET: %s (%d commands)\n", target->name, commands->size);

            for (int j = 0; j < commands->size; j++) {
                vector_node *command_node = commands->nodes[j];
                if (command_node->node_type == TYPE_NKS_RAW_COMMAND) {
                    const char *raw_data = command_node->node_ptr;
                    printf("  raw: %s\n", raw_data);
                } else if (command_node->node_type == TYPE_NKS_TOOL_INVOCATION) {
                    nks_tool_invocation *invocation = command_node->node_ptr;
                    vector *params = invocation->params;
                    printf("  invoke: %s (%d params)\n", invocation->tool_name, invocation->params->size);

                    for (int k = 0; k < params->size; k++) {
                        nks_tool_param *param = params->nodes[k]->node_ptr;
                        vector *values = param->values;
                        printf("    %s = [%d]\n", param->key, values->size);
                        for (int l = 0; l < values->size; l++) {
                            const char *value = values->nodes[l]->node_ptr;
                            printf("     - %s\n", value);
                        }
                    }
                }
            }

        } else {
            printf("warn: Unknown node_type %d\n", node->node_type);
        }

    }

    parser_destroy(ctx);
}

int main(int argc, char **argv) {
    nkm_options options;

    char workingDir[PATH_MAX];
    getcwd(workingDir, sizeof(workingDir));
    options.working_dir = workingDir;

    char buildFile[strlen(workingDir) + strlen("Nekofile") + 2];
    strcpy(buildFile, workingDir);
    strcat(buildFile, "/Nekofile");
    options.build_file = buildFile;

    if (argc > 1) {
        const char *arg = argv[1];

        if (strcmp(arg, "-version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(arg, "-help") == 0) {
            print_help();
            return 0;
        } else {
            options.target = arg;
        }

    } else {
        options.target = NULL;
    }

    run_builder(&options);

    return 0;
}
