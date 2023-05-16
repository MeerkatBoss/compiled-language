#ifndef LEXER_TREE_H
#define LEXER_TREE_H
// TODO: docs
#include <limits.h>

#include "util/text_lines/text_lines.h"

#include "data_structures/token_list/token_list.h"

struct lexer_node
{
    token_type type;
    lexer_node* next[CHAR_MAX + 1];
};

struct lexer_tree
{
    lexer_node* root;
    lexer_node* name;
    lexer_node* num;
};

void lexer_tree_ctor(lexer_tree* tree);
void lexer_tree_dtor(lexer_tree* tree);
lexer_node* lexer_node_ctor(token_type type);
void lexer_node_dtor(lexer_node* node);
void lexer_tree_add_word(lexer_tree* tree, const char* str, token_type type);
void lexer_tree_add_names(lexer_tree* tree);
void lexer_tree_add_numbers(lexer_tree* tree);


#endif