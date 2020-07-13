//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_PARSER_H
#define NKM_PARSER_H

#include <stdbool.h>
#include "options.h"
#include "vector.h"

typedef struct {
    char *build_script_text;
    vector *build_script;
} parser_ctx;

int parser_init(nkm_options *options, parser_ctx **ctx);

void parser_parse(parser_ctx *ctx);

void parser_destroy(parser_ctx **ctxPtr);

bool parser_should_ignore(char *line);

int parser_get_indent_level(char *line);

void parser_parse_line(vector *dst, char *line, int indent);

#endif //NKM_PARSER_H
