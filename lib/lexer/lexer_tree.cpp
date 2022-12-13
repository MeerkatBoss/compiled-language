#include <stdlib.h>
#include <ctype.h>

#include "logger.h"

#include "lexer_tree.h"

static inline bool is_name_char(int c) { return isalnum(c) || c == '_'; }
static inline bool is_num_char(int c) { return isdigit(c) || c == '.'; }

void lexer_tree_ctor(lexer_tree* tree)
{
    tree->root = lexer_node_ctor(TOK_ERROR);
    tree->name = lexer_node_ctor(TOK_NAME);
    tree->num  = lexer_node_ctor(TOK_NUM);

    for (int i = 0; i <= CHAR_MAX; i++)
    {
        if (is_name_char(i)) tree->name->next[i] = tree->name;
        if (is_num_char(i))  tree->num->next[i]  = tree->num;
        if (isspace(i))      tree->root->next[i] = tree->root;
    }
    tree->root->next['\0'] = lexer_node_ctor(TOK_EOF);
}

static void delete_subtree(lexer_tree* tree, lexer_node* node);

void lexer_tree_dtor(lexer_tree* tree)
{
    delete_subtree(tree, tree->root);
    free(tree->name);
    
    tree->name = NULL;
    tree->root = NULL;
}

lexer_node *lexer_node_ctor(token_type type)
{
    lexer_node *node = (lexer_node *)calloc(1, sizeof(*node));

    node->type = type;

    return node;
}

void lexer_node_dtor(lexer_node *node)
{
    free(node);
    memset(node, 0, sizeof(*node));
}

void lexer_tree_add_word(lexer_tree *tree, const char *str, token_type type)
{
    bool is_name_pref = true;
    bool is_digit_pref = true;
    lexer_node* cur_node = tree->root;

    for (const char* c = str; *c != '\0'; c++)
    {
        is_name_pref  &= is_name_char(*c);
        is_digit_pref &= is_num_char (*c);

        if (!cur_node->next[*c])
        {
            if (is_digit_pref)
                cur_node->next[*c] = lexer_node_ctor(TOK_NUM);
            else if (is_name_pref)
                cur_node->next[*c] = lexer_node_ctor(TOK_NAME);
            else
                cur_node->next[*c] = lexer_node_ctor(TOK_ERROR);
        }

        cur_node = cur_node->next[*c];
    }

    LOG_ASSERT((cur_node->type == TOK_NAME || cur_node->type == TOK_ERROR) && "Word already added", return);
    cur_node->type = type;
}

static void subtree_add_names(lexer_tree* tree, lexer_node* node);
static void subtree_add_numbers(lexer_tree* tree, lexer_node* node);

void lexer_tree_add_names(lexer_tree* tree)
{
    subtree_add_names(tree, tree->root);
}

void lexer_tree_add_numbers(lexer_tree *tree)
{
    subtree_add_numbers(tree, tree->root);
}

static void delete_subtree(lexer_tree *tree, lexer_node *node)
{
    for (int i = 0; i <= CHAR_MAX; i++)
        if (node->next[i]
            && node->next[i] != tree->name
            && node->next[i] != tree->num
            && node->next[i] != node)
        {
            delete_subtree(tree, node->next[i]);
            node->next[i] = NULL;
        }

    free(node);
}

static void subtree_add_names(lexer_tree *tree, lexer_node *node)
{
    for (int i = 0; i <= CHAR_MAX; i++)
        if (is_name_char(i) && node->next[i] != node)
        {
            if (node->next[i])
                subtree_add_names(tree, node->next[i]);
            else
                node->next[i] = tree->name;
        }
}

void subtree_add_numbers(lexer_tree *tree, lexer_node *node)
{
    for (int i = 0; i <= CHAR_MAX; i++)
        if (is_num_char(i))
        {
            if (node->next[i] && node->next[i] != node)
                subtree_add_numbers(tree, node->next[i]);
            else
                node->next[i] = tree->num;
        }
}
