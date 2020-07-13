//
// Created by Twometer on 13/07/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "model.h"

int parser_init(nkm_options *options, parser_ctx **ctxPtr) {
    FILE *handle = fopen(options->build_file, "r");
    if (!handle)
        return -1;

    parser_ctx *ctx = malloc(sizeof(parser_ctx));
    *ctxPtr = ctx;

    fseek(handle, 0, SEEK_END);
    int size = ftell(handle);
    rewind(handle);

    ctx->build_script_text = malloc(sizeof(char) * (size + 1));
    fread(ctx->build_script_text, sizeof(char), size, handle);
    ctx->build_script_text[size] = '\0'; // Null-byte

    fclose(handle);

    return 0;
}

void parser_parse(parser_ctx *ctx) {
    ctx->build_script = vector_create(16);

    char *line = strtok(ctx->build_script_text, "\n");
    while (line != NULL) {
        if (!parser_should_ignore(line)) {
            int indent = parser_get_indent_level(line);
            parser_parse_line(ctx->build_script, line, indent);
        }
        line = strtok(NULL, "\n");
    }
}

void parser_parse_line(vector *dst, char *line, int indent) {
    
}

bool parser_should_ignore(char *line) {
    if (line[0] == ';') return true; // a comment

    for (int i = 0; i < strlen(line); i++) { // empty line?
        char chr = line[i];
        if (chr != '\t' && chr != ' ')
            return false;
    }

    return true;
}

int parser_get_indent_level(char *line) {
    char baseChar = line[0];
    if (baseChar == '\t' || baseChar == ' ') {
        int num = 0;
        for (int i = 0; i < strlen(line); i++)
            if (line[i] == baseChar)
                num++;
            else break;

        if (baseChar == ' ')
            num /= 4;

        return num;
    } else return 0;
}

void parser_destroy(parser_ctx **ctxPtr) {
    vector_destroy((*ctxPtr)->build_script);
    free((*ctxPtr)->build_script_text);
    free(*ctxPtr);
    *ctxPtr = NULL;
}