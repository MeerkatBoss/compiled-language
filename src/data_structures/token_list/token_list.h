#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

// TODO: docs
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum token_type
{
#define LEXEME(name, ...) TOK_##name,
#include "data_structures/types/lexemes.h"
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
    double num;
    char* str;
};

#define ARRAY_ELEMENT token
#include "array/dynamic_array.h"

#undef ARRAY_ELEMENT

void token_array_print(const dynamic_array(token)* tokens, FILE* stream);
void token_array_generate_source(const dynamic_array(token)* tokens, FILE* stream);
token token_ctor(char* str, token_type type, size_t line_num, size_t char_num);
token make_token(token_type type);
token make_token(double num);
token make_token(const char* str);

#endif