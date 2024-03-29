#include "util/logger/logger.h"

#include "derivative.h"
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
    if (peek(state)->type != expected) return false;
    advance(state);
    return true;
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
static ast_node* parse_deriv (parsing_state* state);
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

    CONSUME_WITH_ERROR(TOK_STMT_END, { delete_node(value); free(name); },
        "Expected statement terminator.", CUR_POS);

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

    REPORT_ERROR(seq != NULL, {}, "Empty block statements are not allowed.", CUR_POS);  // Standard compliance

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

        CONSUME_WITH_ERROR(TOK_STMT_END, delete_subtree(call), "Expected statement terminator.", CUR_POS);

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

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, delete_subtree(cond), "Ill-formed condition.", CUR_POS);

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

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, delete_subtree(cond), "Ill-formed condition.", CUR_POS);

    ast_node* stmt = parse_stmt(state);

    REPORT_ERROR(stmt != NULL, delete_subtree(cond), "Expected statement", CUR_POS);

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

static ast_node *parse_op(parsing_state *state)
{
    ast_node* result = parse_deriv(state);
    LOG_ASSERT(result != NULL, return NULL);

    while (consume(state, TOK_PLUS) || consume(state, TOK_MINUS))
    {
        op_type op = (peek_last(state)->type == TOK_PLUS
                        ? OP_ADD
                        : OP_SUB);
        ast_node* operand = parse_deriv(state);
        REPORT_ERROR(operand != NULL, delete_subtree(result), "Expected expression.", CUR_POS);
        result = make_node(NODE_OP, {.op = op}, result, operand);
    }

    return result;
}

ast_node *parse_deriv(parsing_state *state)
{
    if (!consume(state, TOK_DIFFERENTIAL))
        return parse_term(state);
    
    size_t line_num = peek(state)->line_num;
    size_t char_num = peek(state)->char_num;
    ast_node* expr = parse_group(state);
    REPORT_ERROR(expr != NULL, {}, "Expected expression.", line_num, char_num);
    CONSUME_WITH_ERROR(TOK_SLASH, delete_subtree(expr), "Division expected.", CUR_POS);
    CONSUME_WITH_ERROR(TOK_DIFFERENTIAL, delete_subtree(expr), "Differential expected.", CUR_POS);

    ast_node* var = parse_var(state);
    LOG_ASSERT(var, { delete_subtree(expr); return NULL; });

    ast_node* derivative = get_derivative(expr, var->value.name);

    REPORT_ERROR(derivative != NULL, { delete_subtree(var); delete_subtree(expr); },
            "Expression cannot be differentiated.", line_num, char_num);

    delete_subtree(expr);
    delete_subtree(var);

    return derivative;
}

static ast_node *parse_term(parsing_state *state)
{
    ast_node* result = parse_unary(state);
    LOG_ASSERT(result != NULL, return NULL);

    while (consume(state, TOK_STAR) || consume(state, TOK_SLASH))
    {
        op_type op = (peek_last(state)->type == TOK_STAR
                        ? OP_MUL
                        : OP_DIV);
        ast_node* operand = parse_unary(state);
        REPORT_ERROR(operand != NULL, delete_subtree(result), "Expected expression.", CUR_POS);
        result = make_node(NODE_OP, {.op = op}, result, operand);
    }

    return result;
}

static ast_node *parse_unary(parsing_state *state)
{
    if (consume(state, TOK_MINUS))
    {
        ast_node* operand = parse_unary(state);
        LOG_ASSERT(operand != NULL, return NULL);
        return make_node(NODE_OP, { .op = OP_NEG }, NULL, operand);
    }
    return parse_group(state);
}

static ast_node *parse_group(parsing_state *state)
{
    if (!consume(state, TOK_GROUP_LEFT)) return parse_atom(state);

    ast_node* expr = parse_logic(state);        // Standard compliance. This should be `parse_op`

    REPORT_ERROR(expr != NULL, {}, "Expected expression.", CUR_POS);

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, delete_subtree(expr), "Grouping expression not closed.", CUR_POS);

    return expr;
}

static ast_node *parse_atom(parsing_state *state)
{
    if (peek(state)->type == TOK_NAME)
    {
        if (peek_next(state)->type != TOK_GROUP_LEFT)
            return parse_var(state);
        
        ast_node* call = parse_call(state);
        LOG_ASSERT(call != NULL, return NULL);

        return call;
    }
    if (consume(state, TOK_NUM))
        return make_node(NODE_CONST, {.num = peek_last(state)->num}, NULL, NULL);
    
    REPORT_ERROR(0, {}, "Unexpected token '%s'", peek(state)->str, CUR_POS);

    return NULL;
}

static ast_node *parse_var(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_NAME, {}, "Variable name expected.", CUR_POS);

    char* name = strdup(peek_last(state)->str);

    return make_node(NODE_VAR, {.name = name}, NULL, NULL);
}

static ast_node *parse_call(parsing_state *state)
{
    CONSUME_WITH_ERROR(TOK_NAME, {}, "Function name expected.", CUR_POS);

    char* name = strdup(peek_last(state)->str);

    CONSUME_WITH_ERROR(TOK_GROUP_LEFT, free(name), "Function parameter list expected.", CUR_POS);

    ast_node* params = NULL;
    if (peek(state)->type != TOK_GROUP_RIGHT) params = parse_par(state);

    CONSUME_WITH_ERROR(TOK_GROUP_RIGHT, {
        free(name);
        if (params) delete_subtree(params);
    }, "Function parameter list not terminated.", CUR_POS);

    return make_node(NODE_CALL, {.name = name}, NULL, params);
}

static ast_node *parse_par(parsing_state *state)
{
    ast_node* expr = parse_logic(state);    //Standard compliance. This should be `parse_op`

    REPORT_ERROR(expr != NULL, {}, "Expected expression.", CUR_POS);

    ast_node* next_par = NULL;

    if (consume(state, TOK_COMMA)) next_par = parse_par(state);

    return make_node(NODE_PAR, {}, expr, next_par);
}
