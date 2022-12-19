#ifndef MID_UTILS_H
#define MID_UTILS_H

#include "ast.h"

bool input_tree_from_file(const char* filename, abstract_syntax_tree* tree);
bool try_simplify_tree(abstract_syntax_tree* tree);


#endif