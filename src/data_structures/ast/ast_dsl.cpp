#include "ast_dsl.h"

ast_node* make_binary_node(op_type op, ast_node * left, ast_node * right)
{
    ast_node* node = make_node(NODE_OP, {.op = op}, left, right);
    return node;
}

ast_node * make_unary_node(op_type op, ast_node * right)
{
    ast_node* node = make_node(NODE_OP, {.op = op}, NULL, right);
    return node;
}

ast_node * make_number_node(double val)
{
    return make_node(NODE_CONST, {.num = val}, NULL, NULL);
}

ast_node * make_var_node(const char* var) 
{
    return make_node(NODE_VAR, {.name = strdup(var)}, NULL, NULL);
}
