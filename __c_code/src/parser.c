//
// Created by Twometer on 13/07/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int parser_init(nkm_options *options, parser_ctx **ctxPtr) {
    FILE *handle = fopen(options->build_file, "r");
    if (!handle)
        return -1;

    parser_ctx *ctx = calloc(sizeof(parser_ctx), 1);
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

    char *line = strtok(ctx->build_script_text, "\r\n");
    int lineNo = 0;
    while (line != NULL) {
        lineNo++;
        if (!parser_should_ignore(line)) {
            int indent = parser_get_indent_level(line);
            parser_parse_line(ctx, line, indent, lineNo);
        }

        if (ctx->failed)
            break;

        line = strtok(NULL, "\r\n");
    }

    parser_close_remaining(ctx, 0);
}

void parser_close_remaining(parser_ctx *ctx, int indent_level) {
    // LEVEL-0 closing
    if (indent_level == 0) {
        if (ctx->is_in_target) {
            ctx->is_in_target = false;
            vector_append(ctx->build_script, TYPE_NKS_TARGET, ctx->current_target);
        }
        if (ctx->is_in_tool) {
            ctx->is_in_tool = false;
            vector_append(ctx->build_script, TYPE_NKS_TOOL, ctx->current_tool);
        }
    }

    // LEVEL-1 closing
    if (indent_level <= 1) {
        if (ctx->is_in_tool_invocation) {
            ctx->is_in_tool_invocation = false;
            vector_append(ctx->current_target->commands, TYPE_NKS_TOOL_INVOCATION, ctx->current_tool_invocation);
        }
    }

    // LEVEL-2 closing
    if (ctx->is_in_tool_param) {
        ctx->is_in_tool_param = false;
        vector_append(ctx->current_tool_invocation->params, TYPE_NKS_TOOL_PARAM, ctx->current_tool_param);
    }
}

void parser_parse_line(parser_ctx *ctx, char *line, int indent, int line_no) {
    int len = strlen(line);
    if (indent == 0) {
        parser_close_remaining(ctx, indent);
        char *name = malloc(32);
        int type = 0;
        int i;
        for (i = 0; i < len; i++) {
            if (line[i] == '=') {
                type = TYPE_NKS_CONSTANT;
                break;
            } else if (line[i] == ':') {
                type = TYPE_NKS_TARGET;
                break;
            } else if (i < 32) {
                name[i] = line[i];
            } else {
                parser_error(ctx, line_no, "Name too long");
                return;
            }
        }
        if (name[0] == '[')
            type = TYPE_NKS_TOOL;

        name[i] = 0;

        if (type == TYPE_NKS_CONSTANT) {
            char *value = malloc(len - i);
            strcpy(value, line + i + 1);

            nks_constant *constant = malloc(sizeof(nks_constant));
            constant->name = parser_strip_whitespace(name);
            constant->value = parser_strip_whitespace(value);
            vector_append(ctx->build_script, TYPE_NKS_CONSTANT, constant);

        } else if (type == TYPE_NKS_TARGET) {
            ctx->is_in_target = true;
            ctx->current_target = malloc(sizeof(nks_target));
            ctx->current_target->name = name;
            ctx->current_target->commands = vector_create(4);

        } else if (type == TYPE_NKS_TOOL) {
            ctx->is_in_tool = true;
            ctx->current_tool = malloc(sizeof(nks_tool));
            ctx->current_tool->command = NULL;
            parser_parse_tool_header(ctx, ctx->current_tool, name, line_no);

        }

    } else if (indent == 1) {
        char *line_trimmed = parser_strip_whitespace(line);

        parser_close_remaining(ctx, indent);

        if (ctx->is_in_tool) {
            if (ctx->current_tool->command != NULL) {
                parser_error(ctx, line_no, "A tool can only hold a single command!");
                return;
            }

            ctx->current_tool->command = line_trimmed;
        } else if (ctx->is_in_target) {
            if (line_trimmed[0] == '[') {
                ctx->is_in_tool_invocation = true;
                ctx->current_tool_invocation = malloc(sizeof(nks_tool_invocation));

                int tool_name_len = strlen(line_trimmed) - 2;
                char *tool_name = malloc(tool_name_len + 1);
                memcpy(tool_name, line_trimmed + 1, tool_name_len);
                tool_name[tool_name_len] = '\0';
                ctx->current_tool_invocation->tool_name = tool_name;
                ctx->current_tool_invocation->params = vector_create(4);
            } else if (!ctx->is_in_tool_invocation) {
                vector_append(ctx->current_target->commands, TYPE_NKS_RAW_COMMAND, line_trimmed);
            } else {
                parser_error(ctx, line_no, "Unexpected [");
                return;
            }
        } else {
            parser_error(ctx, line_no, "Unexpected data");
        }
    } else if (indent >= 2) {

        if (ctx->is_in_tool_invocation) {
            char *line_trimmed = parser_strip_whitespace(line);
            int line_trimmed_len = strlen(line_trimmed);

            if (strstr(line_trimmed, ":") != NULL) {
                parser_close_remaining(ctx, indent);

                ctx->is_in_tool_param = true;
                ctx->current_tool_param = malloc(sizeof(nks_tool_param));
                ctx->current_tool_param->values = vector_create(4);
                char *key = calloc(line_trimmed_len, 1);
                char *val = calloc(line_trimmed_len, 1);
                bool in_key = true;
                int key_offset = 0;
                for (int i = 0; i < line_trimmed_len; i++) {
                    char c = line_trimmed[i];
                    if (c == ':') {
                        if (in_key) {
                            in_key = false;
                            key_offset = i + 1;
                        } else {
                            parser_error(ctx, line_no, "Unexpected :");
                            return;
                        }
                    } else if (in_key) {
                        key[i] = c;
                    } else {
                        val[i - key_offset] = c;
                    }
                }
                ctx->current_tool_param->key = key;
                vector_append(ctx->current_tool_param->values, 0, parser_strip_whitespace(val));

            } else if (ctx->is_in_tool_param) {
                vector_append(ctx->current_tool_param->values, 0, line_trimmed);
            } else {
                parser_error(ctx, line_no, "Unexpected tool invocation data");
                return;
            }
        } else {
            parser_error(ctx, line_no, "Unexpected data");
        }

    }
}

void parser_parse_tool_header(parser_ctx *ctx, nks_tool *tool, char *name, int line_no) {
    int len = strlen(name);

    if (name[len - 1] != ']') {
        parser_error(ctx, line_no, "Expected ]");
        return;
    }

    char *tool_name = calloc(len, 1);
    char *tool_meta = calloc(len, 1);
    int name_base = 0;
    bool in_name = true;

    for (int i = 1; i < len; i++) {
        char c = name[i];
        if (c == ',') {
            in_name = false;
            name_base = i + 1;
        } else if (c == ']') {
            if (i != len - 1) {
                parser_error(ctx, line_no, "Unexpected ]");
                return;
            }
            break;
        } else if (c != ' ') {
            if (in_name)
                tool_name[i - 1] = c;
            else
                tool_meta[i - 1 - name_base] = c;
        }
    }

    if (strlen(tool_name) == 0) {
        parser_error(ctx, line_no, "Cannot have empty tool name!");
        return;
    }

    tool->name = tool_name;
    if (strcmp(tool_meta, "single") == 0) {
        tool->invoke_type = invoke_single;
    } else if (strcmp(tool_meta, "multi") == 0) {
        tool->invoke_type = invoke_multi;
    } else {
        parser_error(ctx, line_no, "Unknown parser type");
        return;
    }

    free(tool_meta);
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

void parser_destroy(parser_ctx *ctx) {
    vector_destroy(ctx->build_script);
    free(ctx->build_script_text);
    free(ctx);
}

void parser_error(parser_ctx *ctx, int line_no, const char *error) {
    ctx->failed = true;
    printf("error: %s (line %d)\n", error, line_no);
}

char *parser_strip_whitespace(char *line) {
    int len = strlen(line);
    char *new = calloc(len + 1, 1);

    int min = 0;
    int max = len;
    while (line[min] == ' ') min++;
    while (line[max-1] == ' ') max--;
    memcpy(new, line + min, max - min);
    return new;
}