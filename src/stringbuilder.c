//
// Created by Twometer on 14/07/2020.
//

#include <stdlib.h>
#include <string.h>

#include "stringbuilder.h"

string_builder *sb_create(int capacity) {
    string_builder *sb = calloc(sizeof(string_builder), 1);
    sb->capacity = capacity;
    sb_reset(sb);
    return sb;
}

void sb_append_chr(string_builder *sb, char chr) {
    sb_ensure_space(sb, 1);
    sb->text[sb->size] = chr;
    sb->size++;
    sb->text[sb->size] = '\0';
}

void sb_append_str(string_builder *sb, const char *str) {
    int len = strlen(str);
    sb_ensure_space(sb, len);
    memcpy(sb->text + sb->size, str, len);
    sb->size += len;
    sb->text[sb->size] = '\0';
}

void sb_ensure_space(string_builder *sb, int append) {
    if (sb->size + (append - 1) >= sb->capacity) {
        sb->capacity += append * 2;
        sb->text = realloc(sb->text, sizeof(char) * sb->capacity);
    }
}

void sb_reset(string_builder *sb) {
    free(sb->text);
    sb->size = 0;
    sb->text = calloc(sb->capacity * sizeof(char), 1);
}