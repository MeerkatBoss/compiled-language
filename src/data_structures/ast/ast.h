/**
 * @file ast.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * @brief Abstract syntax tree struct
 * @version 0.1
 * @date 2022-11-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdio.h>

#include "util/math/math_utils.h"

/**
 * @brief AST node type
 */
enum node_type
{
    #define NODE_TYPE(name, ...) NODE_##name,
    #include "data_structures/types/node_types.h"
    #undef NODE_TYPE
};

/**
 * @brief Operation type for AST nodes of type `NODE_OP`
 */
enum op_type
{
    #define OP_TYPE(name, ...) OP_##name,

    #define CMP_TYPE(name, ...) OP_##name,      // Standard compliance
    #define LOGIC_TYPE(name, ...) OP_##name,    // Standard compliance

    #include "data_structures/types/op_types.h"
    #include "data_structures/types/cmp_types.h"    // Standard compliance
    #include "data_structures/types/logic_types.h"  // Standard compliance

    #undef OP_TYPE

    #undef CMP_TYPE                             // Standard compliance
    #undef LOGIC_TYPE                           // Standard compliance
};

/**
 * @brief Operation type for AST nodes of type `NODE_CMP`
 */
enum cmp_type
{
    #define CMP_TYPE(name, ...) CMP_##name,
    #include "data_structures/types/cmp_types.h"
    #undef CMP_TYPE
};

/**
 * @brief Operation type for AST nodes of type `NODE_LOGIC`
 */
enum logic_type
{
    #define LOGIC_TYPE(name, ...) LOGIC_##name,
    #include "data_structures/types/logic_types.h"
    #undef LOGIC_TYPE
};


/**
 * @brief AST node stored value
 */
union node_value{
    double num;
    char* name;
    op_type op;
    cmp_type cmp;
    logic_type logic;
}; 

/**
 * @brief Abstract syntax tree node
 */
struct ast_node
{
    /**
     * @brief Node type
     */
    node_type   type;
    /**
     * @brief Stored value
     */
    node_value  value;

    /**
     * @brief Pointer to parent node (used for iteration)
     */
    ast_node*   parent;
    /**
     * @brief Pointer to left child
     */
    ast_node*   left;
    /**
     * @brief Pointer to right child
     */
    ast_node*   right;
};

typedef ast_node* tree_iterator;

/**
 * @brief Abstract syntax tree
 */
struct abstract_syntax_tree
{
    /**
     * @brief Tree root node
     */
    ast_node*   root;
};

/**
 * @brief Create new tree node with specified parent and no children.
 * 
 * @param[in] type Node type
 * @param[in] data Data stored in node
 * @param[inout] left Left child node
 * @param[inout] right Right child node
 * @return Allocated `ast_node` instance
 */
ast_node* make_node(node_type type, node_value val, ast_node* left, ast_node* right);

/* TODO: docs */ 

ast_node* copy_subtree(const ast_node* node);

/**
 * @brief Delete ast node. Free associated resources.
 * 
 * @param[inout] node `ast_node` instance to be deleted
 * @warning This function will fail upon attempting to delete node, which has
 * non-`NULL` children.
 */
void delete_node(ast_node* node);

/**
 * @brief Delete subtree of chosen node. Free associated resources.
 * 
 * @param[inout] node Subtree root
 */
void delete_subtree(ast_node* node);

/**
 * @brief Create `abstract_syntax_tree` instance
 * 
 * @param[out] tree Constructed instance
 */
void tree_ctor(abstract_syntax_tree* tree);

/**
 * @brief Destroy `abstract_syntax_tree`
 * 
 * @param[inout] tree `abstract_syntax_tree` instance to be destroyed
 */
void tree_dtor(abstract_syntax_tree* tree);

// TODO: docs
void tree_print(const abstract_syntax_tree* tree, FILE* output);

// TODO: docs
void tree_read(abstract_syntax_tree* tree,  FILE* input);

// TODO: docs, implementation
bool tree_check(const abstract_syntax_tree* tree);

/**
 * @brief Get iterator to first tree element
 * 
 * @param[in] tree 
 * @return Tree iterator
 */
tree_iterator tree_begin(abstract_syntax_tree* tree);

/**
 * @brief Get iterator to next tree node, following in-order traversal.
 * 
 * @param[in] node Current tree node
 * @return Tree iterator
 */
tree_iterator tree_get_next(tree_iterator it);

/**
 * @brief Get iterator to previous tree node
 * 
 * @param[in] node Current tree node
 * @return Tree iterator
 */
tree_iterator tree_get_prev(tree_iterator it);

/**
 * @brief Get iterator to last tree element
 * 
 * @param[in] tree 
 * @return Tree iterator 
 */
tree_iterator tree_end(abstract_syntax_tree* tree);

#endif