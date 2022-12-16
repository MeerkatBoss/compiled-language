#include "logger.h"

#include "parser.h"

struct parsing_state
{
    const dynamic_array(token)* tokens;
    size_t pos;
};

static inline token* peek     (parsing_state* state) { return array_get_element(state->tokens, state->pos); }
static inline token* peek_last(parsing_state* state) { return array_get_element(state->tokens, state->pos - 1); }
static inline token* peek_next(parsing_state* state) { return array_get_element(state->tokens, state->pos + 1); }
static inline void   advance  (parsing_state* state) { state->pos++; }
static inline bool   consume  (parsing_state* state, token_type expected)
{
    if (peek(state)->type != expected) return 0;
    advance(state);
    return 0;
}

#define ERROR_POS " [Ln. %zu, Col. %zu]"
#define REPORT_ERROR(condition, cleanup, message, ...) \
    LOG_ASSERT_ERROR(condition, { cleanup; return NULL; }, message ERROR_POS, __VA_ARGS__)
#define CONSUME_WITH_ERROR(type, cleanup, message, ...) \
    REPORT_ERROR(consume(state, type), cleanup, message, __VA_ARGS__)
#define CUR_POS peek(state)->line_num, peek(state)->char_num 

static ast_node* parse_defs  (parsing_state* state);
static ast_node* parse_nvar  (parsing_state* state);
static ast_node* parse_nfun  (parsing_state* state);
static ast_node* parse_arg   (parsing_state* state);
static ast_node* parse_block (parsing_state* state);
static ast_node* parse_seq   (parsing_state* state);
static ast_node* parse_stmt  (parsing_state* state);
static ast_node* parse_ass   (parsing_state* state);
static ast_node* parse_if    (parsing_state* state);
static ast_node* parse_branch(parsing_state* state);
static ast_node* parse_while (parsing_state* state);
static ast_node* parse_ret   (parsing_state* state);
static ast_node* parse_logic (parsing_state* state);
static ast_node* parse_and   (parsing_state* state);
static ast_node* parse_not   (parsing_state* state);
static ast_node* parse_cmp   (parsing_state* state);
static ast_node* parse_op    (parsing_state* state);
static ast_node* parse_term  (parsing_state* state);
static ast_node* parse_unary (parsing_state* state);
static ast_node* parse_group (parsing_state* state);
static ast_node* parse_atom  (parsing_state* state);
static ast_node* parse_var   (parsing_state* state);
static ast_node* parse_call  (parsing_state* state);
static ast_node* parse_par   (parsing_state* state);

int parser_build_tree(const dynamic_array(token) * tokens, abstract_syntax_tree *tree)
{
    parsing_state state = {tokens, 0};
    tree->root = parse_defs(&state);
    LOG_ASSERT(tree->root != NULL, return -1);
    return 0;
}

static ast_node *parse_defs(parsing_state *state)
{
    token* cur = peek(state);
    ast_node* def = NULL;
    if (cur->type == TOK_VAR)
        def = parse_nvar(state);
    else if (cur->type == TOK_FUNC)
        def = parse_nfun(state);
    else
        return NULL;
    
    return make_node(NODE_DEFS, {}, def, parse_defs(state));
}

static ast_node *parse_nvar(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_VAR, {}, "Variable declaration expected.", CUR_POS);

    size_t cur_line = peek(state)->line_num;
    size_t cur_char = peek(state)->char_num;

    CONSUME_WITH_ERROR(TOK_NAME, {}, "Expected variable name.", cur_line, cur_char);

    char* name = strdup(peek_last(state)->str);

    CONSUME_WITH_ERROR(TOK_INIT, free(name), "Variable '%s' is not initialized.",
        name, cur_line, cur_char);
    
    ast_node* value = parse_logic(state);   // standard compliance, this should be `parse_op`
    LOG_ASSERT(value != NULL, {free(name); return NULL; });

    return make_node(NODE_NVAR, {.name = name}, NULL, value);
}

static ast_node *parse_nfun(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_FUNC, {}, "Function declaration expected.", CUR_POS);
    size_t cur_line = peek(state)->line_num;
    size_t cur_char = peek(state)->char_num;

    CONSUME_WITH_ERROR(TOK_NAME, {}, "Expected function name.", cur_line, cur_char);

    char* name = strdup(peek_last(state)->str);

    CONSUME_WITH_ERROR(TOK_GROUP_LEFT, free(name), "Function '%s': ill-formed argument list.",
        name, cur_line, cur_char);
    
    ast_node* args = NULL;
    if (peek(state)->type == TOK_VAR) args = parse_arg(state);

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, {
            if(args) delete_subtree(args);
            free(name);
        },
        "Function '%s': ill-formed argument list.",
        name, cur_line, cur_char);

    ast_node* block = parse_block(state);
    LOG_ASSERT(block != NULL, { free(name); delete_subtree(args); return NULL; });

    return make_node(NODE_NFUN, {.name = name}, args, block);
}

static ast_node *parse_arg(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_VAR, {}, "Argument expected.", CUR_POS);

    size_t cur_line = peek(state)->line_num;
    size_t cur_char = peek(state)->char_num;

    CONSUME_WITH_ERROR(TOK_NAME, {}, "Expected argument name.", cur_line, cur_char);
    
    ast_node* next_arg = NULL;
    char* name = strdup(peek_last(state)->str);

    if (consume(state, TOK_COMMA))
    {
        next_arg = parse_arg(state);
        LOG_ASSERT(next_arg != NULL, { free(name); return NULL; });
    }

    return make_node(NODE_ARG, {.name = name}, NULL, next_arg);
}

static ast_node *parse_block(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_BLOCK_START, {}, "Expected block.", CUR_POS);
    
    ast_node* seq = parse_seq(state);

    CONSUME_WITH_ERROR(TOK_BLOCK_END, delete_subtree(seq), "Block not closed.", CUR_POS);
    
    return make_node(NODE_BLOCK, {}, NULL, seq);
}

static ast_node *parse_seq(parsing_state *state)
{
    ast_node* stmt = parse_stmt(state);

    if (!stmt) return NULL;

    return make_node(NODE_SEQ, {}, stmt, parse_seq(state));
}

static ast_node *parse_stmt(parsing_state *state)
{
    token* cur = peek(state);

    if (cur->type == TOK_NAME)
    {
        if (peek_next(state)->type != TOK_GROUP_LEFT)
            return parse_ass(state);
        
        ast_node* call = parse_call(state);
        LOG_ASSERT(call != NULL, return NULL);

        CONSUME_WITH_ERROR(TOK_STMT_END, delete_node(call), "Expected statement terminator.", CUR_POS);

        return call;
    }
    if (cur->type == TOK_VAR)         return parse_nvar (state);
    if (cur->type == TOK_BLOCK_START) return parse_block(state);
    if (cur->type == TOK_IF)          return parse_if   (state);
    if (cur->type == TOK_WHILE)       return parse_while(state);
    if (cur->type == TOK_RETURN)      return parse_ret  (state);

    return NULL;
}

static ast_node *parse_ass(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_NAME, {}, "Variable name expected.", CUR_POS);

    char* name = strdup(peek_last(state)->str);

    CONSUME_WITH_ERROR(TOK_ASSIGN, {}, "Assignment expected.", CUR_POS);

    ast_node* value = parse_logic(state);   // standard compliance, this should be `parse_op`
    REPORT_ERROR(value != NULL, {}, "Expected expression.", CUR_POS);

    CONSUME_WITH_ERROR(TOK_STMT_END, {free(name); delete_subtree(value); },
        "Statement terminator expected.", CUR_POS);

    return make_node(NODE_ASS, {.name = name}, NULL, value);
}

static ast_node *parse_if(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_IF, {}, "Expected conditional statement.", CUR_POS);
    CONSUME_WITH_ERROR(TOK_GROUP_LEFT, {}, "Ill-formed condition.", CUR_POS);

    ast_node* cond = parse_logic(state);     /* For once, this is not because of the shitty standard.
                                                It's actually supposed to be `parse_logic` */
    REPORT_ERROR(cond != NULL, {}, "Expected condition.", CUR_POS);

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, {}, "Ill-formed condition.", CUR_POS);

    ast_node* branch = parse_branch(state);
    LOG_ASSERT(branch != NULL, { delete_subtree(cond); return NULL; });

    return make_node(NODE_IF, {}, cond, branch);
}

static ast_node *parse_branch(parsing_state *state)
{
    ast_node* stmt_pos = parse_stmt(state);

    REPORT_ERROR(stmt_pos != NULL, {}, "Expected statement.", CUR_POS);

    ast_node* stmt_neg = NULL;

    if (consume(state, TOK_ELSE))
    {
        stmt_neg = parse_stmt(state);
        REPORT_ERROR(stmt_neg != NULL, delete_subtree(stmt_pos), "Expected statement.", CUR_POS);
    }
    
    return make_node(NODE_BRANCH, {}, stmt_pos, stmt_neg);
}

static ast_node *parse_while(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_WHILE, {}, "Expected loop statement.", CUR_POS);
    CONSUME_WITH_ERROR(TOK_GROUP_LEFT, {}, "Ill-formed condition.", CUR_POS);

    ast_node* cond = parse_logic(state);     /* For once, this is not because of the shitty standard.
                                                It's actually supposed to be `parse_logic` */

    REPORT_ERROR(cond != NULL, {}, "Expected condition.", CUR_POS);

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, {}, "Ill-formed condition.", CUR_POS);

    ast_node* stmt = parse_stmt(state);

    LOG_ASSERT_ERROR(stmt != NULL, return NULL, "Statement expected." ERROR_POS, CUR_POS);

    return make_node(NODE_WHILE, {}, cond, stmt);
}

static ast_node *parse_ret(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_RETURN, {}, "Return statement expected.", CUR_POS);

    ast_node* value = parse_logic(state);    /* Ah shit, here we go again. Yes, it's standard compliance,
                                                it's supposed to be `parse_op` */
    REPORT_ERROR(value != NULL, {}, "Expected return value.", CUR_POS);
    
    CONSUME_WITH_ERROR(TOK_STMT_END, delete_subtree(value), "Statement terminator expected.", CUR_POS);

    return make_node(NODE_RET, {}, NULL, value);
}

static ast_node *parse_logic(parsing_state *state)
{
    ast_node* result = parse_and(state);
    LOG_ASSERT(result != NULL, return NULL);

    while (consume(state, TOK_OR))
    {
        ast_node* operand = parse_and(state);
        REPORT_ERROR(operand != NULL, delete_subtree(result), "Expected expression.", CUR_POS);
        result = make_node(NODE_OP, {.op = OP_OR}, result, operand); /* Standard compliance.
                                                                        This should be `NODE_LOGIC` with value
                                                                        {.logic = LOGIC_OR}*/
    }

    return result;
}

static ast_node *parse_and(parsing_state *state)
{
    ast_node* result = parse_not(state);
    LOG_ASSERT(result != NULL, return NULL);

    while (consume(state, TOK_AND))
    {
        ast_node* operand = parse_not(state);
        REPORT_ERROR(operand != NULL, delete_subtree(result), "Expected expression.", CUR_POS);
        result = make_node(NODE_OP, {.op = OP_AND}, result, operand);/* Standard compliance.
                                                                        This should be `NODE_LOGIC` with value
                                                                        {.logic = LOGIC_AND}*/
    }

    return result;
}

static ast_node *parse_not(parsing_state *state)
{
    if (!consume(state, TOK_NOT))
        return parse_cmp(state);
    
    ast_node* operand = parse_cmp(state);
    LOG_ASSERT(operand != NULL, return NULL);

    return make_node(NODE_OP, {.op = OP_NOT}, NULL, operand);/* Standard compliance.
                                                                This should be `NODE_LOGIC` with value
                                                                {.logic = LOGIC_NOT}*/
}

static inline bool consume_cmp(parsing_state* state)
{
    return consume(state, TOK_GREATER) || consume(state, TOK_LESS)
        || consume(state, TOK_GEQ)     || consume(state, TOK_LEQ)
        || consume(state, TOK_EQ)      || consume(state, TOK_NEQ);

}

// Standard compliance. Return value should be cmp_type
static inline op_type get_cmp(token_type token)
{
    if (token == TOK_GREATER) return OP_GT;
    if (token == TOK_LESS)    return OP_LT;
    if (token == TOK_GEQ)     return OP_GEQ;
    if (token == TOK_LEQ)     return OP_LEQ;
    if (token == TOK_EQ)      return OP_EQ;
    if (token == TOK_NEQ)     return OP_NEQ;
    
    return (op_type)-1;
}

static ast_node *parse_cmp(parsing_state *state)
{
    ast_node* result = parse_op(state);
    LOG_ASSERT(result != NULL, return NULL);

    while (consume_cmp(state))
    {
        op_type op = get_cmp(peek_last(state)->type);   // Standard compliance. This should be `cmp_type cmp = ...`
        ast_node* operand = parse_op(state);
        REPORT_ERROR(operand != NULL, delete_subtree(result), "Expected expression.", CUR_POS);
        result = make_node(NODE_OP, {.op = op}, result, operand);/* Standard compliance.
                                                                    This should be `NODE_LOGIC` with value
                                                                    {.cmp = cmp}*/
    }

    return result;
}
