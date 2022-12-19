#include "logger.h"

#include "ast_dsl.h"

#include "simplifier.h"

static bool simplify_node(ast_node * node);

bool simplify_tree(abstract_syntax_tree* tree)
{
    return simplify_node(tree->root);
}

#define LEFT node->left
#define RIGHT node->right

static inline ast_node* NUM(double num)       { return make_number_node(num); }
static inline ast_node* VAR(const char* var)  { return make_var_node(var);    }

static inline ast_node* ADD (ast_node* left, ast_node* right) { return make_binary_node(OP_ADD, left, right); }
static inline ast_node* SUB (ast_node* left, ast_node* right) { return make_binary_node(OP_SUB, left, right); }
static inline ast_node* MUL (ast_node* left, ast_node* right) { return make_binary_node(OP_MUL, left, right); }
static inline ast_node* FRAC(ast_node* left, ast_node* right) { return make_binary_node(OP_DIV, left, right); }

static inline ast_node* NEG (ast_node* right) { return make_unary_node(OP_NEG, right); }

#define D(x) get_differential(x, var)

static inline ast_node* CPY(const ast_node* node) { return copy_subtree(node); }

static void replace_with(ast_node* dest, ast_node* src);

static inline int is_neg (ast_node* node) { return op_cmp(node, OP_NEG); }
static inline int is_zero(ast_node* node) { return num_cmp(node, 0); }
static inline int is_one (ast_node* node) { return num_cmp(node, 1); }
static inline int get_int(ast_node* node) { return (int) round(get_num(node)); }
static inline int is_int (ast_node* node) { return num_cmp(node, get_int(node)); }

static inline int is_same_var(ast_node* node1, ast_node* node2)
{
    return is_var(node1) && var_cmp(node2, get_var(node1));
}

static bool extract_negative      (ast_node* node);
static bool extract_left_negative (ast_node* node);
static bool extract_right_negative(ast_node* node);
static bool extract_left_zero     (ast_node* node);
static bool extract_right_zero    (ast_node* node);
static bool extract_left_one      (ast_node* node);
static bool extract_right_one     (ast_node* node);
static bool collapse_var          (ast_node* node);
static bool collapse_const        (ast_node* node);

static bool simplify_node(ast_node * node)
{
    if (is_num(node)) extract_negative(node);
    if (!is_op(node)) return true;;

    LOG_ASSERT(simplify_node(node-> left), return false);
    LOG_ASSERT(simplify_node(node->right), return false);

    if (is_neg(LEFT) && is_neg(RIGHT)) LOG_ASSERT(extract_negative      (node), return false);
    if (is_neg(LEFT))                  LOG_ASSERT(extract_left_negative (node), return false);
    if (is_neg(RIGHT))                 LOG_ASSERT(extract_right_negative(node), return false);
    if (is_zero(LEFT))                 LOG_ASSERT(extract_left_zero     (node), return false);
    if (is_zero(RIGHT))                LOG_ASSERT(extract_right_zero    (node), return false);
    if (is_one(LEFT))                  LOG_ASSERT(extract_left_one      (node), return false);
    if (is_one(RIGHT))                 LOG_ASSERT(extract_right_one     (node), return false);
    if (is_num(LEFT) && is_num(RIGHT)) LOG_ASSERT(collapse_const        (node), return false);
    if (is_same_var(LEFT, RIGHT))      LOG_ASSERT(collapse_var          (node), return false);

    return true;
}

static void replace_with(ast_node* dest, ast_node* src)
{
    if (dest->type == NODE_VAR)
        free(dest->value.name);

    dest->left  = src->left;
    dest->right = src->right;
    if (dest->left)  dest->left ->parent = dest;
    if (dest->right) dest->right->parent = dest;
    dest->type = src->type;
    dest->value = src->value;
    src->left  = NULL;
    src->right = NULL;

    delete_node(src);
}

static inline void assign_num(ast_node* dest, double num)
{
    if (dest->type == NODE_VAR) free(dest->value.name);

    dest->value.num = num;
    dest->type = NODE_CONST;
    if (dest-> left) delete_subtree(dest-> left);
    if (dest->right) delete_subtree(dest->right);
    dest->left = NULL;
    dest->right = NULL;
}

static bool collapse_const(ast_node* node)
{
    #define ASSIGN_NODE(num) assign_num(node, num)
    #define COMBINE_CHILDREN(op) ASSIGN_NODE(get_num(LEFT) op get_num(RIGHT))

    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_num(LEFT), return false);
    LOG_ASSERT(is_num(RIGHT), return false);

    switch (get_op(node))
    {
    case OP_ADD: COMBINE_CHILDREN(+); break;
    case OP_SUB: COMBINE_CHILDREN(-); break;
    case OP_MUL: COMBINE_CHILDREN(*); break;
    case OP_DIV:
        LOG_ASSERT_ERROR(!num_cmp(LEFT, 0), return false, "Division by zero", NULL);
        COMBINE_CHILDREN(/);
        break;
    default:
        break;
    }

    #undef COMBINE_CHILDREN
    #undef ASSIGN_NODE
}

static bool extract_negative(ast_node* node)
{
    LOG_ASSERT(node, return false);
    if (is_num(node))
    {
        if (get_num(node) < 0)
        {
            node->right = NUM(-1 * get_num(node));
            node->type = NODE_OP;
            node->value.op = OP_NEG;
        }
        return;
    }

    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(op_cmp(LEFT,  OP_NEG), return false);
    LOG_ASSERT(op_cmp(RIGHT, OP_NEG), return false);

    switch(get_op(node))
    {
    case OP_ADD:
    case OP_SUB:
        replace_with(LEFT,  LEFT ->right);
        replace_with(RIGHT, RIGHT->right);
        replace_with(node, NEG(
            make_binary_node(
                get_op(node),
                LEFT,
                RIGHT)));
        simplify_node(RIGHT);
        break;
    case OP_MUL:
    case OP_DIV:
        replace_with(LEFT,  LEFT ->right);
        replace_with(RIGHT, RIGHT->right);
        break;
    default:
        break;
    }
}

static bool extract_left_negative(ast_node* node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(op_cmp(LEFT, OP_NEG), return false);

    switch(get_op(node))
    {
    case OP_ADD:
        replace_with(LEFT, LEFT->right);
        replace_with(node, SUB(RIGHT, LEFT));
        break;
    case OP_SUB:
        replace_with(LEFT, LEFT->right);
        replace_with(node, NEG(ADD(LEFT, RIGHT)));
        break;
    case OP_MUL:
    case OP_DIV:
        replace_with(LEFT, LEFT->right);
        replace_with(node, NEG(
            make_binary_node(
                get_op(node),
                LEFT,
                RIGHT)));
        break;
    default:
        break;
    }
}

static bool extract_right_negative(ast_node* node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(op_cmp(RIGHT, OP_NEG), return false);

    switch (get_op(node))
    {
    case OP_ADD:
        replace_with(RIGHT, RIGHT->right);
        replace_with(node, SUB(LEFT, RIGHT));
        break;;
    case OP_SUB:
        replace_with(RIGHT, RIGHT->right);
        replace_with(node, ADD(LEFT, RIGHT));
        break;
    case OP_MUL:
    case OP_DIV:
        replace_with(RIGHT, RIGHT->right);
        replace_with(node, NEG(
            make_binary_node(
                get_op(node),
                LEFT,
                RIGHT)));
        break;
    case OP_NEG:
        replace_with(RIGHT, RIGHT->right);
        replace_with(node, RIGHT);
        break;
    default:
        break;
    }

}

static bool extract_left_zero(ast_node* node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_zero(LEFT), return false);

    switch(get_op(node))
    {
    case OP_ADD:
        delete_node(LEFT);
        replace_with(node, RIGHT);
        break;
    case OP_SUB:
        delete_node(LEFT);
        replace_with(node, NEG(RIGHT));
        break;
    case OP_MUL:
    case OP_DIV:
        assign_num  (node, 0);
        break;
    default:
        break;
    }
}

static bool extract_right_zero(ast_node* node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_zero(RIGHT), return false);
    LOG_ASSERT_ERROR(!op_cmp(node, OP_DIV), return, "Division by zero", NULL);

    switch (get_op(node))
    {
    case OP_ADD:
    case OP_SUB:
        delete_node(RIGHT);
        replace_with(node, LEFT);
        break;
    case OP_MUL:
    case OP_NEG:
        assign_num(node, 0);
        break;
    default:
        break;
    }
}

static bool extract_left_one(ast_node *node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_one(LEFT), return false);

    switch (get_op(node))
    {
    case OP_MUL:
        delete_node(LEFT);
        replace_with(node, RIGHT);
        break;
    default:
        break;
    }
}

static bool extract_right_one(ast_node *node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_one(RIGHT), return false);

    switch (get_op(node))
    {
    case OP_MUL:
    case OP_DIV:
        delete_node(RIGHT);
        replace_with(node, LEFT);
        break;
    default:
        break;
    }
}

static bool collapse_var(ast_node *node)
{
    LOG_ASSERT(is_op(node), return false);
    LOG_ASSERT(is_same_var(LEFT, RIGHT), return false);

    switch (get_op(node))
    {
    case OP_ADD:
        delete_node(LEFT);
        replace_with(node, MUL(NUM(2), RIGHT));
        break;
    case OP_SUB:
        assign_num(node, 0);
        break;
    case OP_DIV:
        assign_num(node, 1);
        break;
    default:
        break;
    }
}