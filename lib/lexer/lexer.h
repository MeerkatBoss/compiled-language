#ifndef LEXER_H
#define LEXER_H

#include "lexer_tree.h"
#include "token_list.h"

// TODO: docs
int parse_tokens(const char* str, const lexer_tree* tree, dynamic_array(token)* tokens);

#endif