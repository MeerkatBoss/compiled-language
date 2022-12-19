#include "logger.h"

#include "ast_dsl.h"

#include "derivative.h"

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

static bool is_const(const ast_node* node, const char* var);

ast_node* get_differential(const ast_node * node, const char* var)
{
    if (var_cmp(node, var))
        return NUM(1);

    if (is_const(node, var))
        return NUM(0);

    LOG_ASSERT_ERROR(is_op(node), return NULL,
        "Cannot differentiate given node", NULL);
    
    switch(node->value.op)
    {
        case OP_ADD:
            return ADD(D(LEFT), D(RIGHT));
        case OP_SUB:
            return SUB(D(LEFT), D(RIGHT));
        case OP_MUL:
            return ADD(
                MUL(D(LEFT), CPY(RIGHT)),
                MUL(CPY(LEFT), D(RIGHT))
            );
        case OP_DIV:
            return FRAC(
                SUB(
                    MUL(D(LEFT), CPY(RIGHT)),
                    MUL(CPY(LEFT), D(RIGHT))
                ),
                MUL(CPY(RIGHT), CPY(RIGHT))
            );
        case OP_NEG:
            return NEG(D(RIGHT));

        #define CMP_TYPE(name, ...) case OP_##name:
        #define LOGIC_TYPE(name, ...) case OP_##name:
        #include "cmp_types.h"
        #include "logic_types.h"
        #undef LOGIC_TYPE
        #undef CMP_TYPE
        default:
            LOG_ASSERT_ERROR(0, return NULL,
                "Cannot differentiate given node", NULL);
    }
    return NULL;
}

static bool is_const(const ast_node * node, const char* var)
{
    if (!node) return true; /* Vacuous truth */

    if (is_num(node)) return true;
    if (is_var(node)) return !var_cmp(node, var);
    if (is_op (node)) return is_const(LEFT, var) && is_const(RIGHT, var);

    return false;
}