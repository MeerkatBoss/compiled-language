#include "logger.h"

#include "table_stack.h"
#include "func_array.h"
#include "size_array.h"

#include "compiler.h"

struct compilation_state
{
    func_array functions;
    table_stack name_scope;

    const char* func_name;
    size_t block_depth;
    bool has_return;
    size_t control_flow_cnt;
    dynamic_array(size_t) control_flow_stack;
    FILE* output;
};

static inline void state_destroy(compilation_state* state)
{
    func_array_dtor(&state->functions);
    table_stack_dtor(&state->name_scope);
    array_dtor(&state->control_flow_stack);
    fflush(state->output);
    state = {};
}

static bool extract_declarations(const ast_node* node, compilation_state* state);
static bool compile_node        (const ast_node* node, compilation_state* state);

#define STEP_WITH_CLEANUP(action, cleanup) LOG_ASSERT(action, { cleanup; return false; })
#define STEP(action) STEP_WITH_CLEANUP(action, {})
#define AST_ASSERT_WITH_CLEANUP(condition, cleanup, format, ...) \
    LOG_ASSERT_ERROR(condition, { cleanup; return false; }, "Malformed AST:" format, __VA_ARGS__)
#define AST_ASSERT(condition, format, ...) AST_ASSERT_WITH_CLEANUP(condition, {}, format, __VA_ARGS__)


bool compiler_tree_to_asm(const abstract_syntax_tree *tree, FILE *output, bool use_stdlib)
{
    compilation_state state = {
        .functions = {},
        .name_scope = {},
        .func_name = NULL,
        .block_depth = 0,
        .has_return = false,
        .control_flow_cnt = 0,
        .control_flow_stack = {},
        .output = output
    };
    func_array_ctor(&state.functions, use_stdlib);
    table_stack_ctor(&state.name_scope);
    array_ctor(&state.control_flow_stack);

    STEP_WITH_CLEANUP(extract_declarations(tree->root, &state), state_destroy(&state));

    AST_ASSERT_WITH_CLEANUP(
        func_array_find_func(&state.functions, "main") != NULL,
        state_destroy(&state),
        "Function 'main' was not defined. Cannot create program entry point.", NULL
    );

    fputs("\t\tcall main\t; Program entry point\n", output);
    fputs("\t\tpop rdx\t\t; Ignore return status\n", output);
    fputs("\t\thalt\t\t; Stop program execution\n\n\n", output);

    for (size_t i = 0; i < state.functions.size; i++)
    {
        if (state.functions.data[i].node == NULL)   // stdlib function
            continue;
        STEP_WITH_CLEANUP(
            compile_node(state.functions.data[i].node, &state),
            state_destroy(&state)
        );
        fputc('\n', output);
    }

    state_destroy(&state);
    return true;
}

bool extract_declarations(const ast_node *node, compilation_state *state)
{
    AST_ASSERT(node != NULL, "Expected DEFS, got empty node.", NULL);

    if (node->left->type == NODE_NVAR)
        STEP(compile_node(node->left, state));
    else if (node->left->type == NODE_NFUN)
        AST_ASSERT(
            func_array_add_func(&state->functions, node->left) == 0,
            "Function '%s' redefinition.", node->value.name);
    else    
        STEP(0 && "Unreachable code");
    
    if (node->right != NULL)
        return extract_declarations(node->right, state);
    
    return true;
}

enum compilation_stage
{
    STAGE_COMPILING_LEFT,
    STAGE_COMPILED_LEFT,
    STAGE_COMPILING_RIGHT,
    STAGE_COMPILED_RIGHT
};

static bool on_compiling_left   (const ast_node* node, compilation_state* state);
static bool on_compiled_left    (const ast_node* node, compilation_state* state);
static bool on_compiling_right  (const ast_node* node, compilation_state* state);
static bool on_compiled_right   (const ast_node* node, compilation_state* state);

bool compile_node(const ast_node *node, compilation_state *state)
{
    if (node == NULL) return true;

    STEP(on_compiling_left(node, state));
    STEP(compile_node(node->left, state));
    STEP(on_compiled_left(node, state));

    STEP(on_compiling_right(node, state));
    STEP(compile_node(node->right, state));
    STEP(on_compiled_right(node, state));

    return true;
}

#define compile(name, stage) compile_##name(node, state, STAGE_##stage)
#define define_compile(name) \
    static bool compile_##name (const ast_node* node __attribute__((unused)),\
                                compilation_state* state __attribute__((unused)),\
                                 compilation_stage stage __attribute__((unused)))

define_compile(DEFS);
define_compile(NVAR);
define_compile(NFUN);
define_compile(BLOCK);
define_compile(ARG);
define_compile(OP);
define_compile(SEQ);
define_compile(ASS);
define_compile(WHILE);
define_compile(IF);
define_compile(BRANCH);
define_compile(CALL);
define_compile(PAR);
define_compile(RET);
define_compile(CONST);
define_compile(VAR);

define_compile(CMP);
define_compile(LOGIC);

/* Wow, four almost identical functions :(
    Sadly, C preprocessor cannot fix this problem */
static bool on_compiling_left(const ast_node *node, compilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: return compile(name, COMPILING_LEFT);
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return false;
    }
    return false;
}
static bool on_compiled_left(const ast_node *node, compilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: return compile(name, COMPILED_LEFT);
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return false;
    }
    return false;
}
static bool on_compiling_right(const ast_node *node, compilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: return compile(name, COMPILING_RIGHT);
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return false;
    }
    return false;
}
static bool on_compiled_right(const ast_node *node, compilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: return compile(name, COMPILED_RIGHT);
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        return false;
    }
    return false;
}

define_compile(DEFS)
{
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false; /* this function should not be called */
}

define_compile(NVAR)
{
    switch (stage)
    {
        case STAGE_COMPILING_RIGHT:
        {
            size_t addr = 0;
            const char* def_label = table_stack_add_var(&state->name_scope, node->value.name, &addr);
            fprintf(state->output, "%%def %s %zu\n", def_label, addr);
            fputs(
                "\t\tpush rsp\n"
                "\t\tinc\n"
                "\t\tpop rsp\n",
                state->output
            );
            return true;
        }
        case STAGE_COMPILED_RIGHT:
            return compile(ASS, COMPILED_RIGHT);    // Initialization is compiled the same way as assignment
        case STAGE_COMPILING_LEFT:
        case STAGE_COMPILED_LEFT:
            return true;    // Nothing to do here
        default:
            LOG_ASSERT(0 && "Invalid enum value", return false);
            return false;
    }
    LOG_ASSERT(0 && "Invalid enum value", return false);
    return false;
}

define_compile(NFUN)
{
    switch (stage)
    {
        case STAGE_COMPILING_LEFT:
            fprintf(state->output, "%s:\n", node->value.name);
            state->func_name = node->value.name;
            state->has_return = false;
            table_stack_add_table(&state->name_scope);  // Function arguments in separate scope
            fputs(
                "\t\tpush rbp\n"
                "\t\tpop [rsp]\n"
                "\t\tpush rsp\n"
                "\t\tinc\n"
                "\t\tdup\n"
                "\t\tpop rsp\n"
                "\t\tpop rbp\n",
                state->output
            );
            return true;
        case STAGE_COMPILED_RIGHT:
            AST_ASSERT(state->has_return, "Control reaches end of function '%s'", node->value.name);
            state->func_name = NULL;
            state->has_return = false;
            table_stack_pop_table(&state->name_scope);
            fprintf(state->output, ".%s.end:\n", node->value.name);
            fputs(
                "\t\tpush rbp\n"
                "\t\tdec\n"
                "\t\tdup\n"
                "\t\tpop rbp\n"
                "\t\tpop rsp\n"
                "\t\tpush [rbp]\n"
                "\t\tpop rbp\n"
                "\t\tret\n",
                state->output
            );
            return true;
        case STAGE_COMPILED_LEFT:
        case STAGE_COMPILING_RIGHT:
            return true;    // Nothing to do here
        default:
            LOG_ASSERT(0 && "Invalid enum value", return false);
            return false;
    }
    LOG_ASSERT(0 && "Invalid enum value", return false);
    return false;
}

define_compile(BLOCK)
{
    switch (stage)
    {
        case STAGE_COMPILING_RIGHT:
            state->block_depth++;
            table_stack_add_table(&state->name_scope);       
            return true;
        case STAGE_COMPILED_RIGHT:
            state->block_depth--;
            table_stack_pop_table(&state->name_scope);
            return true;
        case STAGE_COMPILING_LEFT:
        case STAGE_COMPILED_LEFT:
            return true; // Nothing to do here
        default:
            LOG_ASSERT(0 && "Unreachable code", return false);
            return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(ARG)
{
    switch (stage)
    {
        case STAGE_COMPILING_RIGHT:
            return compile(NVAR, COMPILING_RIGHT);  // Argument creation is simply variable definition
        case STAGE_COMPILED_RIGHT:
        {
            bool global = false;
            const char* def_label = table_stack_find_var(&state->name_scope, node->value.name, &global);
            LOG_ASSERT(!global, return false);
            LOG_ASSERT(def_label, return false);
            fprintf(state->output, "\t\tpop [rbp+%s]\n", def_label);
            return true;
        }
        case STAGE_COMPILING_LEFT:
        case STAGE_COMPILED_LEFT:
            return true; // Nothing to do here
        default:
            LOG_ASSERT(0 && "Unreachable code", return false);
            return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(OP)
{
    if (stage == STAGE_COMPILED_LEFT && node->value.op == OP_DIV)
    {
        fputs("\t\tpush 1000\n", state->output);
        fputs("\t\tmul\n", state->output);
        return true;
    }

    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    switch (node->value.op)
    {
    case OP_ADD: fputs("\t\tadd\n", state->output); return true;
    case OP_SUB: fputs("\t\tsub\n", state->output); return true;
    case OP_MUL:
        fputs("\t\tmul\n", state->output);
        fputs("\t\tpush 1000\n", state->output);
        fputs("\t\tdiv\n", state->output);
        return true;
    case OP_DIV:
        fputs("\t\tdiv\n", state->output);
        return true;
    case OP_NEG:
        fputs("\t\tpush -1\n", state->output);
        fputs("\t\tmul\n", state->output);
        return true;
    case OP_LT:  fputs("\t\tcall __op.lt\n" , state->output); return true;
    case OP_GT:  fputs("\t\tcall __op.gt\n" , state->output); return true;
    case OP_LEQ: fputs("\t\tcall __op.leq\n", state->output); return true;
    case OP_GEQ: fputs("\t\tcall __op.geq\n", state->output); return true;
    case OP_EQ:  fputs("\t\tcall __op.eq\n" , state->output); return true;
    case OP_NEQ: fputs("\t\tcall __op.neq\n", state->output); return true;
    case OP_AND: fputs("\t\tcall __op.and\n", state->output); return true;
    case OP_OR:  fputs("\t\tcall __op.or\n" , state->output); return true;
    case OP_NOT: fputs("\t\tcall __op.not\n", state->output); return true;
    default:
        LOG_ASSERT(0 && "Unreachable code", return false);
        return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(SEQ)
{
    if (stage != STAGE_COMPILED_LEFT) return true; // Nothing to do here

    if (node->left->type == NODE_CALL)
        fputs("\t\tpop rdx\t\t; Discard function call result.\n", state->output);
    return true;
}

define_compile(ASS)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    bool is_global = false;
    const char* def_label = table_stack_find_var(&state->name_scope, node->value.name, &is_global);

    AST_ASSERT(def_label != NULL, "Undefined reference to variable '%s'.", node->value.name);

    fprintf(state->output, "\t\tpop [%s%s]\n", is_global ? "" : "rbp+", def_label);
    return true;
}

define_compile(WHILE)
{
    switch (stage)
    {
    case STAGE_COMPILING_LEFT:
        fprintf(state->output, ".while_%s_0x%zX_start:\n", state->func_name, state->control_flow_cnt);
        return true;
    case STAGE_COMPILED_LEFT:
        fputs("\t\tpush 0", state->output);
        fprintf(state->output, "\t\t je .while_%s_0x%zX_end\n\n", state->func_name, state->control_flow_cnt);
        return true;
    case STAGE_COMPILING_RIGHT:
        array_push(&state->control_flow_stack, state->control_flow_cnt);
        state->control_flow_cnt++;
        return true;
    case STAGE_COMPILED_RIGHT:
        fprintf(state->output, ".while_%s_0x%zX_end:\n\n", state->func_name, state->control_flow_cnt);
        state->control_flow_cnt = *array_back(&state->control_flow_stack);
        array_pop(&state->control_flow_stack);
        return true;
    default:
        LOG_ASSERT(0 && "Unreachable code", return false);
        return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(IF)
{
    switch (stage)
    {
    case STAGE_COMPILING_LEFT:
        fprintf(state->output, "; Start of .if_%s_0x%zX\n", state->func_name, state->control_flow_cnt);
        return true;
    case STAGE_COMPILED_LEFT:
        fputs("\t\tpush 0\n", state->output);
        fprintf(state->output, "\t\tje .if_%s_0x%zX_false\n\n", state->func_name, state->control_flow_cnt);
        return true;
    case STAGE_COMPILING_RIGHT:
        return true;
    case STAGE_COMPILED_RIGHT:
        fprintf(state->output, ".if_%s_0x%zX_end:\n\n", state->func_name, state->control_flow_cnt);
        return true;
    default:
        LOG_ASSERT(0 && "Unreachable code", return false);
        return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(BRANCH)
{
    switch (stage)
    {
    case STAGE_COMPILING_LEFT:
        array_push(&state->control_flow_stack, state->control_flow_cnt);
        state->control_flow_cnt++;
        return true;
    case STAGE_COMPILED_LEFT:
        state->control_flow_cnt = *array_back(&state->control_flow_stack);
        array_pop(&state->control_flow_stack);
        if (node->right != NULL)    // skip over non-empty 'else' branch
            fprintf(state->output, "jmp .if_%s_0x%zX_end:\n\n", state->func_name, state->control_flow_cnt);
        return true;
    case STAGE_COMPILING_RIGHT:
        fprintf(state->output, ".if_%s_0x%zX_false:\n", state->func_name, state->control_flow_cnt);
        array_push(&state->control_flow_stack, state->control_flow_cnt);
        state->control_flow_cnt++;
        return true;
    case STAGE_COMPILED_RIGHT:
        state->control_flow_cnt = *array_back(&state->control_flow_stack);
        array_pop(&state->control_flow_stack);
        return true;
    default:
        LOG_ASSERT(0 && "Unreachable code", return false);
        return false;
    }
    LOG_ASSERT(0 && "Unreachable code", return false);
    return false;
}

define_compile(CALL)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here
    const function* func = func_array_find_func(&state->functions, node->value.name);
    AST_ASSERT(func != NULL, "Function '%s' was not defined.", node->value.name);
    size_t args = 0;
    ast_node* arg = node->right;
    while(arg)
    {
        args++;
        arg = arg->right;
    }
    AST_ASSERT(func->arg_cnt == args,
        "Function '%s' expects %zu arguments, but %zu were given.", node->value.name, func->arg_cnt, args);
    fprintf(state->output, "\t\tcall %s\n", node->value.name);

    return true;
}

define_compile(PAR)
{
    return true; // Nothing to do here
}

define_compile(RET)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    fprintf(state->output, "\t\tjmp .%s.end\n", state->func_name);
    if (state->block_depth == 1)
        state->has_return = true;
    return true;
}

define_compile(CONST)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    fprintf(state->output, "\t\tpush %d\n", (int)(1000 * node->value.num));

    return true;
}

define_compile(VAR)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    bool is_global = false;
    const char* def_label = table_stack_find_var(&state->name_scope, node->value.name, &is_global);

    AST_ASSERT(def_label != NULL, "Undefined reference to variable '%s'.", node->value.name);

    fprintf(state->output, "\t\tpush [%s%s]\n", is_global ? "" : "rbp+", def_label);
    return true;
}

define_compile(CMP) { return false; }
define_compile(LOGIC) { return false; }
