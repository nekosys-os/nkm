//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_MODEL_H
#define NKM_MODEL_H

#include "vector.h"

#define TYPE_NKS_CONSTANT 1
typedef struct {
    const char *name;
    const char *value;
} nks_constant;

typedef enum {
    invoke_single,
    invoke_multi
} nks_invoke_type;

#define TYPE_NKS_TOOL 2
typedef struct {
    const char *name;
    nks_invoke_type invoke_type;
} nks_tool;

#define TYPE_NKS_TARGET 3
typedef struct {
    const char *name;
    vector *commands;
} nks_target;

#define TYPE_NKS_TOOL_INVOCATION 4
typedef struct {
    const char *toolName;
    vector *params;
} nks_tool_invocation;

#define TYPE_NKS_TOOL_PARAM 5
typedef struct {
    const char *key;
    const char *val;
} nks_tool_param;

#endif //NKM_MODEL_H
