#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "ast.h"

ast_node* get_differential(const ast_node* node, const char* var);

#endif