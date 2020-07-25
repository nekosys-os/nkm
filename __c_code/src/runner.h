//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_RUNNER_H
#define NKM_RUNNER_H

#include <stdbool.h>

#include "model.h"
#include "vector.h"

typedef struct {
    const char *working_dir;
    const char *target;
    vector *build_script;
    bool failed;
} runner_ctx;

runner_ctx *runner_create(const char *working_dir, const char *target, vector *build_script);

void runner_exec(runner_ctx *ctx);

void runner_run_target(runner_ctx *ctx, nks_target *target);

void runner_fail(runner_ctx *ctx, const char *message);

void runner_destroy(runner_ctx *ctx);

void exec_tool(runner_ctx *ctx, nks_tool *tool, vector *params);

nks_tool *find_tool(runner_ctx *ctx, const char *tool);

void exec_command(runner_ctx *ctx, const char *command);

#endif //NKM_RUNNER_H
