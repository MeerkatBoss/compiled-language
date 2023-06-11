#ifndef BACK_UTILS
#define BACK_UTILS

#include "data_structures/ast/ast.h"

const char BACK_DEFAULT_OUTPUT[] = "out.asm";

bool get_tree_from_file(const char* filename, abstract_syntax_tree* tree);
bool compile_tree_to_file(const abstract_syntax_tree* tree,
                          const char* filename,
                          bool use_stdlib = true);

#endif