#include <stdlib.h>
#include <string.h>

#include "logger.h"

#include "ast.h"

ast_node* make_node(node_type type, node_value val, ast_node* parent)
{
    ast_node* node = (ast_node*)calloc(1, sizeof(*node));
    *node = {
        .type = type,
        .value = val,
        .parent = parent,
        .left = NULL,
        .right = NULL
    };

    return node;
}

ast_node* copy_subtree(ast_node * node)
{
    if (!node) return NULL;

    ast_node* res = make_node(node->type, node->value);

    res->left  = copy_subtree(node->left);
    res->right = copy_subtree(node->right);

    if (res-> left) res-> left->parent = res;
    if (res->right) res->right->parent = res;

    return res;
}

void delete_node(ast_node* node)
{
    LOG_ASSERT(node != NULL, return);
    LOG_ASSERT(node-> left == NULL, return);
    LOG_ASSERT(node->right == NULL, return);

    if (node->type == NODE_NVAR ||
        node->type == NODE_NFUN ||
        node->type == NODE_ARG  ||
        node->type == NODE_ASS  ||
        node->type == NODE_CALL ||
        node->type == NODE_VAR)
    {
        free(node->value.name)
    }

    free(node);
}

void delete_subtree(ast_node* node)
{
    LOG_ASSERT(node != NULL, return);

    if (node-> left) delete_subtree(node->left);
    if (node->right) delete_subtree(node->right);

    node-> left = NULL;
    node->right = NULL;

    delete_node(node);
}

abstract_syntax_tree* tree_ctor(void)
{
    abstract_syntax_tree* result = (abstract_syntax_tree*) calloc(1, sizeof(*result));
    result->root = NULL;
    return result;
}

void tree_dtor(abstract_syntax_tree *tree)
{
    LOG_ASSERT(tree != NULL, return);

    if (tree->root) delete_subtree(tree->root);
    tree->root = NULL;


    free(tree);
}

tree_iterator tree_begin(abstract_syntax_tree *tree)
{
    LOG_ASSERT(tree != NULL, return NULL);
    LOG_ASSERT(tree->root != NULL, return NULL);

    tree_iterator node = tree->root;
    while (node->left)
        node = node->left;
    
    return node;
}

tree_iterator tree_get_next(tree_iterator node)
{
    LOG_ASSERT(node, return NULL);

    if (node->right)
    {
        node = node->right;
        while (node->left)
            node = node->left;
        return node;
    }

    tree_iterator parent = node->parent;
    
    while (parent && parent->left != node)
    {
        node = parent;
        parent = node->parent;
    }

    return parent;
}

tree_iterator tree_get_prev(tree_iterator node)
{
    LOG_ASSERT(node, return NULL);

    if (node->left)
    {
        node = node->left;
        while (node->right)
            node = node->right;
        return node;
    }

    tree_iterator parent = node->parent;
    while (parent && parent->right != node)
    {
        node = parent;
        parent = node->parent;
    }

    return parent;
}

tree_iterator tree_end(abstract_syntax_tree* tree)
{
    LOG_ASSERT(tree != NULL, return NULL);
    LOG_ASSERT(tree->root != NULL, return NULL);

    tree_iterator node = tree->root;
    while (node->right)
        node = node->right;
    
    return node;
}