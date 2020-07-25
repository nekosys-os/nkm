//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_DEBUG_H
#define NKM_DEBUG_H

#include <stdio.h>

#include "vector.h"
#include "model.h"

void debug_dump_all(vector *build_script) {
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
}

#endif //NKM_DEBUG_H
