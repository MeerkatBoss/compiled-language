#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

// TODO: docs
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum token_type
{
#define LEXEME(name, ...) TOK_##name,
#include "lexemes.h"
#undef LEXEME
    TOK_NUM,
    TOK_NAME,
    TOK_EOF,
    TOK_ERROR
};

struct token
{
    token_type type;
    size_t line_num;
    size_t char_num;
    char* str;
};

#define ARRAY_ELEMENT token

inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src) { memcpy(dest, src, sizeof(token)); }
inline void delete_element(ARRAY_ELEMENT* element) { free(element->str); memset(element, 0, sizeof(token)); }

#include "dynamic_array.h"

#undef ARRAY_ELEMENT

void token_array_print(const dynamic_array(token)* tokens, FILE* stream);

#endif