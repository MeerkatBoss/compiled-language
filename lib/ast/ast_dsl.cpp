#include "ast_dsl.h"

ast_node* make_binary_node(op_type op, ast_node * left, ast_node * right)
{
    ast_node* node = make_node(NODE_OP, {.op = op});
    node->left = left;
    node->right = right;
    left->parent = node;
    right->parent = node;
    return node;
}

ast_node * make_unary_node(op_type op, ast_node * right)
{
    ast_node* node = make_node(NODE_OP, {.op = op});
    node->right = right;
    right->parent = node;
    return node;
}

ast_node * make_number_node(double val)
{
    return make_node(NODE_NUM, {.num = val});
}

ast_node * make_var_node(char* var) 
{
    return make_node(NODE_VAR, {.name = var});
}
