#ifndef MID_UTILS_H
#define MID_UTILS_H

#include "data_structures/ast/ast.h"

const char MID_DEFAULT_OUTPUT[] = "out-opt.ast";

bool input_tree_from_file(const char* filename, abstract_syntax_tree* tree);
bool try_simplify_tree(abstract_syntax_tree* tree);
bool write_tree_to_file(const abstract_syntax_tree* tree, const char* filename);


#endif