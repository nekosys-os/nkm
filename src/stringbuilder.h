//
// Created by Twometer on 14/07/2020.
//

#ifndef NKM_STRINGBUILDER_H
#define NKM_STRINGBUILDER_H

typedef struct {
    char *text;
    int capacity;
    int size;
} string_builder;

string_builder *sb_create(int capacity);

void sb_append_chr(string_builder* sb, char chr);

void sb_append_str(string_builder* sb, const char* str);

void sb_ensure_space(string_builder *sb, int append);

void sb_reset(string_builder *sb);

#endif //NKM_STRINGBUILDER_H
