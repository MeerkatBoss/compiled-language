#include "decompiler.h"

struct decompilation_state
{
    dynamic_array(token)* tokens;
};

static void decompile_node (const ast_node* node, decompilation_state* state);

void decompile_ast_to_tokens(const abstract_syntax_tree* tree, dynamic_array(token)* tokens)
{
    decompilation_state state = {
        .tokens = tokens
    };

    decompile_node(tree->root, &state);
}

enum decompilation_stage
{
    STAGE_DECOMPILING_LEFT,
    STAGE_DECOMPILED_LEFT,
    STAGE_DECOMPILING_RIGHT,
    STAGE_DECOMPILED_RIGHT
};

static void on_decompiling_left   (const ast_node* node, decompilation_state* state);
static void on_decompiled_left    (const ast_node* node, decompilation_state* state);
static void on_decompiling_right  (const ast_node* node, decompilation_state* state);
static void on_decompiled_right   (const ast_node* node, decompilation_state* state);

void decompile_node(const ast_node *node, decompilation_state *state)
{
    if (node == NULL) return;

    on_decompiling_left(node, state);
    decompile_node(node->left, state);
    on_decompiled_left(node, state);

    on_decompiling_right(node, state);
    decompile_node(node->right, state);
    on_decompiled_right(node, state);
}

#define decompile(name, stage) decompile_##name(node, state, STAGE_##stage)
#define define_decompile(name)                                                      \
    static void decompile_##name (const ast_node* node __attribute__((unused)),     \
                                decompilation_state* state __attribute__((unused)), \
                                 decompilation_stage stage __attribute__((unused)))

#define NODE_TYPE(name, ...) define_decompile(name);
#include "node_types.h"
#undef NODE_TYPE

/* Wow, four almost identical functions :(
    Sadly, C preprocessor cannot fix this problem */
static void on_decompiling_left(const ast_node *node, decompilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: decompile(name, DECOMPILING_LEFT); return;
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return;
    }
    return;
}
static void on_decompiled_left(const ast_node *node, decompilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: decompile(name, DECOMPILED_LEFT); return;
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return;
    }
    return;
}
static void on_decompiling_right(const ast_node *node, decompilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: decompile(name, DECOMPILING_RIGHT); return;
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return;
    }
    return;
}
static void on_decompiled_right(const ast_node *node, decompilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: decompile(name, DECOMPILED_RIGHT); return;
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return;
    }
    return;
}

define_decompile(DEFS)
{
    return; // No lexeme produced
}

define_decompile(NVAR)
{
    switch(stage)
    {
        case STAGE_DECOMPILING_LEFT:
            array_push(state->tokens, make_token(TOK_VAR));
            array_push(state->tokens, make_token(node->value.name));
            return;
        case STAGE_DECOMPILED_LEFT:
            array_push(state->tokens, make_token(TOK_INIT));
            return;
        case STAGE_DECOMPILING_RIGHT:
            return;     // No lexeme produced here
        case STAGE_DECOMPILED_RIGHT:
            array_push(state->tokens, make_token(TOK_STMT_END));
            return;
        default:
            return;
    }
    return;
}

define_decompile(NFUN)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            array_push(state->tokens, make_token(TOK_FUNC));
            array_push(state->tokens, make_token(node->value.name));
            array_push(state->tokens, make_token(TOK_GROUP_LEFT));
            return;
        case STAGE_DECOMPILED_LEFT:
            array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
            return;
        case STAGE_DECOMPILING_RIGHT:
        case STAGE_DECOMPILED_RIGHT:
        default:
            return;
    }
    return;
}

define_decompile(ARG)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            array_push(state->tokens, make_token(TOK_VAR));
            array_push(state->tokens, make_token(node->value.name));
            return;
        case STAGE_DECOMPILED_LEFT:
            if (node->right) return array_push(state->tokens, make_token(TOK_COMMA));
            return;
        case STAGE_DECOMPILING_RIGHT:
        case STAGE_DECOMPILED_RIGHT:
        default:
            return;
    }
    return;
}

define_decompile(BLOCK)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_RIGHT:
            return array_push(state->tokens, make_token(TOK_BLOCK_START));
        case STAGE_DECOMPILED_RIGHT:
            return array_push(state->tokens, make_token(TOK_BLOCK_END));
        case STAGE_DECOMPILING_LEFT:
        case STAGE_DECOMPILED_LEFT:
        default:
            return;
    }
    return;
}

define_decompile(SEQ)
{
    if (stage == STAGE_DECOMPILED_LEFT && node->left->type == NODE_CALL)
        return array_push(state->tokens, make_token(TOK_STMT_END));
    return;
}

define_decompile(ASS)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            return array_push(state->tokens, make_token(node->value.name));
        case STAGE_DECOMPILED_LEFT:
            return array_push(state->tokens, make_token(TOK_ASSIGN));
        case STAGE_DECOMPILED_RIGHT:
            return array_push(state->tokens, make_token(TOK_STMT_END));
        case STAGE_DECOMPILING_RIGHT:
        default:
            return;
    }
    return;
}

define_decompile(IF)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            array_push(state->tokens, make_token(TOK_IF));
            array_push(state->tokens, make_token(TOK_GROUP_LEFT));
            return;
        case STAGE_DECOMPILED_LEFT:
            return array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
        case STAGE_DECOMPILING_RIGHT:
        case STAGE_DECOMPILED_RIGHT:
        default:
            return;
    }
    return;
}

define_decompile(BRANCH)
{
    if (stage == STAGE_DECOMPILED_LEFT && node->right != NULL)
        return array_push(state->tokens, make_token(TOK_ELSE));
    return;
}

define_decompile(WHILE)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            array_push(state->tokens, make_token(TOK_WHILE));
            array_push(state->tokens, make_token(TOK_GROUP_LEFT));
            return;
        case STAGE_DECOMPILED_LEFT:
            return array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
        case STAGE_DECOMPILING_RIGHT:
        case STAGE_DECOMPILED_RIGHT:
        default:
            return;
    }
    return;
}

define_decompile(RET)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_RIGHT:
            return array_push(state->tokens, make_token(TOK_RETURN));
        case STAGE_DECOMPILED_RIGHT:
            return array_push(state->tokens, make_token(TOK_STMT_END));
        case STAGE_DECOMPILING_LEFT:
        case STAGE_DECOMPILED_LEFT:
        default:
            return;
    }
    return;
}

define_decompile(CALL)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            return array_push(state->tokens, make_token(node->value.name));
        case STAGE_DECOMPILED_LEFT:
            return;
        case STAGE_DECOMPILING_RIGHT:
            return array_push(state->tokens, make_token(TOK_GROUP_LEFT));
        case STAGE_DECOMPILED_RIGHT:
            return array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
        default:
            return;
    }
    return;
}

define_decompile(PAR)
{
    if (stage == STAGE_DECOMPILED_LEFT && node->right != NULL)
        return array_push(state->tokens, make_token(TOK_COMMA));
    return;
}

static token_type get_op_token(op_type type);
static bool require_left_group(const ast_node* node);
static bool require_right_group(const ast_node* node);

define_decompile(OP)
{
    switch (stage)
    {
        case STAGE_DECOMPILING_LEFT:
            if (require_left_group(node))
                return array_push(state->tokens, make_token(TOK_GROUP_LEFT));
            return;
        case STAGE_DECOMPILED_LEFT:
            if (require_left_group(node))
                return array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
            return array_push(state->tokens, make_token(get_op_token(node->value.op)));
        case STAGE_DECOMPILING_RIGHT:
            if (require_right_group(node))
                return array_push(state->tokens, make_token(TOK_GROUP_LEFT));
            return;
        case STAGE_DECOMPILED_RIGHT:
            if (require_right_group(node))
                return array_push(state->tokens, make_token(TOK_GROUP_RIGHT));
        default:
            return;
    }
}

define_decompile(CMP) { return; }   // Standard compliance
define_decompile(LOGIC) { return; }   // Standard compliance

define_decompile(VAR)
{
    if (stage == STAGE_DECOMPILING_LEFT)
        return array_push(state->tokens, make_token(node->value.name));
    return;
}

define_decompile(CONST)
{
    if (stage == STAGE_DECOMPILING_LEFT)
        return array_push(state->tokens, make_token(node->value.num));
    return;
}

static token_type get_op_token(op_type type)
{
    switch (type)
    {
        case OP_ADD: return TOK_PLUS;
        case OP_SUB:
        case OP_NEG: return TOK_MINUS;
        case OP_MUL: return TOK_STAR;
        case OP_DIV: return TOK_SLASH;
        case OP_GT:  return TOK_GREATER;
        case OP_LT:  return TOK_LESS;
        case OP_GEQ: return TOK_GEQ;
        case OP_LEQ: return TOK_LEQ;
        case OP_EQ:  return TOK_EQ;
        case OP_NEQ: return TOK_NEQ;
        case OP_AND: return TOK_AND;
        case OP_OR:  return TOK_OR;
        case OP_NOT: return TOK_NOT;
        default:     return TOK_ERROR;
    }
    return TOK_ERROR;
}

static inline bool is_cmp(op_type op)
{
    #define CMP_TYPE(name, ...) if (op == OP_##name) return true;
    #include "cmp_types.h"
    #undef CMP_TYPE
    return false;
}

static inline bool is_logic(op_type op)
{
    #define LOGIC_TYPE(name, ...) if (op == OP_##name) return true;
    #include "logic_types.h"
    #undef LOGIC_TYPE
    return false;
}

static bool require_left_group(const ast_node* node)
{
    if (!node->left || node->left->type != NODE_OP) return false;
    switch (node->value.op)
    {
        case OP_OR: return false;
        case OP_AND: return node->left->value.op == OP_OR;
        case OP_NOT: return node->left->value.op == OP_OR || node->left->value.op == OP_AND;

        case OP_LT: case OP_LEQ:
        case OP_GT: case OP_GEQ:
        case OP_EQ: case OP_NEQ:
            return is_logic(node->left->value.op);

        case OP_ADD:
        case OP_SUB:
            return is_logic(node->left->value.op) || is_cmp(node->left->value.op);
        
        case OP_MUL:
        case OP_DIV:
            return is_logic(node->left->value.op) || is_cmp(node->left->value.op)
                    || node->left->value.op == OP_ADD || node->left->value.op == OP_SUB;
        
        case OP_NEG: return false;
        default: return true;
    }
    return true;
}

static bool require_right_group(const ast_node* node)
{
    if (!node->right || node->right->type != NODE_OP) return false;
    switch (node->value.op)
    {
        case OP_OR: return false;
        case OP_AND: return node->right->value.op == OP_OR;
        case OP_NOT: return node->right->value.op == OP_OR || node->right->value.op == OP_AND;

        case OP_LT: case OP_LEQ:
        case OP_GT: case OP_GEQ:
        case OP_EQ: case OP_NEQ:
            return is_logic(node->right->value.op);

        case OP_ADD:
            return is_logic(node->right->value.op) || is_cmp(node->right->value.op);
        
        case OP_MUL:
        case OP_SUB:
            return is_logic(node->right->value.op) || is_cmp(node->right->value.op)
                    || node->right->value.op == OP_ADD || node->right->value.op == OP_SUB;

        case OP_NEG:
        case OP_DIV:
            return node->right->value.op != OP_NEG;
        
        default: return true;
    }
    return true;
}
