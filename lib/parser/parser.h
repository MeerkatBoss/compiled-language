#ifndef PARSER_H
#define PARSER_H

#include "token_list.h"
#include "ast.h"

/**
 * @brief Build abstract syntax tree from list of tokens
 * @param[in] tokens List of lexemes
 * @param[out] tree  Built tree
 * @return 0 upon successful compilation, -1 otherwise
 */
int parser_build_tree(const dynamic_array(token)* tokens, abstract_syntax_tree* tree);

#endif