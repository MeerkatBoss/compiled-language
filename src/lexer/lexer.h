#ifndef LEXER_H
#define LEXER_H

#include "data_structures/token_list/token_list.h"
#include "lexer_tree.h"

// TODO: docs
bool lexer_parse_tokens(const char* str, const lexer_tree* tree, dynamic_array(token)* tokens);

#endif