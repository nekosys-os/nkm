//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_PARSER_H
#define NKM_PARSER_H

#include <stdbool.h>
#include "options.h"
#include "vector.h"
#include "model.h"

typedef struct {
    char *build_script_text;
    vector *build_script;
    bool failed;

    bool is_in_tool;
    nks_tool *current_tool;

    bool is_in_target;
    nks_target *current_target;

    bool is_in_tool_invocation;
    nks_tool_invocation *current_tool_invocation;

    bool is_in_tool_param;
    nks_tool_param *current_tool_param;

} parser_ctx;

int parser_init(nkm_options *options, parser_ctx **ctx);

void parser_parse(parser_ctx *ctx);

void parser_destroy(parser_ctx *ctx);

bool parser_should_ignore(char *line);

int parser_get_indent_level(char *line);

void parser_parse_line(parser_ctx *ctx, char *line, int indent, int line_no);

void parser_close_remaining(parser_ctx *ctx, int indent_level);

void parser_parse_tool_header(parser_ctx *ctx, nks_tool *tool, char *name, int line_no);

void parser_error(parser_ctx *ctx, int line_no, const char *error);

char* parser_strip_whitespace(char *line);

#endif //NKM_PARSER_H
