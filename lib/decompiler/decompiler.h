#ifndef DECOMPILER_H
#define DECOMPILER_H

#include "ast.h"
#include "token_list.h"

void decompile_ast_to_tokens(const abstract_syntax_tree* tree, dynamic_array(token)* tokens);


#endif