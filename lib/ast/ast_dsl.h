#ifndef AST_DSL_H
#define AST_DSL_H

#include <string.h>

#include "ast.h"

/* TODO: docs */

inline double   get_num(const ast_node* node)               { return node->value.num; }
inline bool     is_num (const ast_node* node)               { return node && node->type == NODE_CONST;}
inline bool     num_cmp(const ast_node* node, double num)   { return is_num(node) && compare_double(get_num(node), num) == 0; }

inline op_type  get_op (const ast_node* node)               { return node->value.op; }
inline bool     is_op  (const ast_node* node)               { return node && node->type == NODE_OP; }
inline bool     op_cmp (const ast_node* node, op_type op)   { return is_op(node) && get_op(node) == op; }

inline char*    get_var(const ast_node* node)               { return node->value.name; }
inline bool     is_var (const ast_node* node)               { return node && node->type == NODE_VAR; }
inline bool     var_cmp(const ast_node* node, const char* var)
{
    return is_var(node) && strcmp(get_var(node), var) == 0;
}

/**
 * @brief Create syntax tree node for binary operation
 * @param[in] op Operation type
 * @param[inout] left Left operand
 * @param[inout] right Right operand
 * @return Created node
 */
ast_node* make_binary_node(op_type op, ast_node* left, ast_node* right);

/**
 * @brief Create syntax tree node for unary operation
 * @param[in] op Operation type
 * @param[inout] right Right operand
 * @return Created node
 */
ast_node* make_unary_node(op_type op, ast_node* right);

/**
 * @brief Create syntax tree node for number constant
 * @param[in] op Operation type
 * @param[in] val Stored value
 * @return Created node
 */
ast_node* make_number_node(double val);

/**
 * @brief Create syntax tree node for variable
 * @param[in] op Operation type
 * @param[in] id Variable id
 * @return Created node
 */
ast_node* make_var_node(const char* var);


#endif