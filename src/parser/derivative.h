#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "data_structures/ast/ast.h"

ast_node* get_derivative(const ast_node* node, const char* var);

#endif