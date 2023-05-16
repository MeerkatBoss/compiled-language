#ifndef DECOMPILER_H
#define DECOMPILER_H

#include "data_structures/ast/ast.h"
#include "data_structures/token_list/token_list.h"

void decompile_ast_to_tokens(const abstract_syntax_tree* tree, dynamic_array(token)* tokens);


#endif