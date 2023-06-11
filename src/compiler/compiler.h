#ifndef COMPILER_H
#define COMPILER_H

#include "data_structures/ast/ast.h"

/**
 * @brief Compile AST to MeerkatVM's assembly language
 * @param[in] tree Abstract syntax tree
 * @param[inout] output Output file
 * @param[in] use_stdlib `true` if program is allowed to use stdlib functions,
 *              `false` otherwise
 * @return `true` upon successful compilation, `false` otherwise
 */
bool compiler_tree_to_asm(const abstract_syntax_tree* tree,
                          FILE* output, bool use_stdlib = false);

#endif