#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"

/**
 * @brief Compile AST to MeerkatVM's assembly language
 * @param[in] tree Abstract syntax tree
 * @param[inout] output Output file
 * @return 0 upon successful compilation, -1 otherwise
 */
int compiler_tree_to_asm(const abstract_syntax_tree* tree, FILE* output);

#endif