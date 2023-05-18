#include "util/logger/logger.h"

#include "data_structures/name_scopes/table_stack.h"
#include "data_structures/name_scopes/func_array.h"
#include "data_structures/intermediate_repr/ir.h"
#include "data_structures/intermediate_repr/ir_dsl.h"

#include "compiler.h"

inline long max_long(long a, long b) { return a > b ? a : b; }

struct compilation_state
{
    func_array  functions;
    table_stack name_scope;

    const char* func_name;
    size_t      block_depth;
    bool        has_return;

    size_t global_var_cnt;
    size_t stack_frame_size;

    ir_node_stack   ir_stack;

    ir_node_ptr ir_head;
    ir_node_ptr ir_tail;
    ir_node_ptr func_return;
};

static void state_ctor(compilation_state* state, bool use_stdlib);
static void state_dtor(compilation_state* state);
static void state_add_ir_node(compilation_state* state, ir_node* node);

static bool extract_declarations(const ast_node* node, compilation_state* state);
static bool compile_node        (const ast_node* node, compilation_state* state);

#define STEP_WITH_CLEANUP(action, cleanup)\
    LOG_ASSERT(action, { cleanup; return false; })

#define STEP(action)\
    STEP_WITH_CLEANUP(action, {})

#define AST_ASSERT_WITH_CLEANUP(condition, cleanup, format, ...) \
    LOG_ASSERT_ERROR(condition, { cleanup; return false; },\
            "Malformed AST:" format, __VA_ARGS__)

#define AST_ASSERT(condition, format, ...)\
    AST_ASSERT_WITH_CLEANUP(condition, {}, format, __VA_ARGS__)


bool compiler_tree_to_asm(const abstract_syntax_tree *tree, FILE *output,
                          bool use_stdlib)
{
    compilation_state state = {};
    state_ctor(&state, use_stdlib);

    STEP_WITH_CLEANUP(extract_declarations(tree->root, &state),
                        state_dtor(&state));

    const function* main = func_array_find_func(&state.functions, "main");

    AST_ASSERT_WITH_CLEANUP(
        main != NULL,
        state_dtor(&state),
        "Function 'main' was not defined. Cannot create program entry point.",
        NULL
    );

    state_add_ir_node(&state, ir_node_new_call(main->ir_list_head));
    state_add_ir_node(&state, ir_node_new_binary(IR_MOV,
                                        ir_operand_reg(IR_REG_RDI),
                                        ir_operand_reg(IR_REG_RAX)));
    state_add_ir_node(&state, ir_node_new_binary(IR_MOV,
                                        ir_operand_reg(IR_REG_RAX),
                                        ir_operand_imm(0x3C)));  // call exit
    state_add_ir_node(&state, ir_node_new_syscall());

    
    for (size_t i = 0; i < state.functions.size; i++)
    {
        if (state.functions.data[i].node == NULL)   // stdlib function
            continue;
        STEP_WITH_CLEANUP(
            compile_node(state.functions.data[i].node, &state),
            state_dtor(&state)
        );
    }

    ir_list_dump(state.ir_head, output);
    state_dtor(&state);
    return true;
}

static void state_ctor(compilation_state* state, bool use_stdlib)
{
    func_array_ctor(&state->functions, use_stdlib);
    table_stack_ctor(&state->name_scope);
    ir_stack_ctor(&state->ir_stack);

    state->ir_head = ir_node_new_empty();
    state->ir_tail = state->ir_head;
}

static void state_dtor(compilation_state* state)
{
    func_array_dtor(&state->functions);
    table_stack_dtor(&state->name_scope);
    ir_stack_dtor(&state->ir_stack);
    ir_list_clear(state->ir_head);

    if (state->func_return) free(state->func_return);
    state = {};
}

static void state_add_ir_node(compilation_state* state, ir_node* node)
{
    state->ir_tail = ir_list_insert_after(state->ir_tail, node);
}


bool extract_declarations(const ast_node *node, compilation_state *state)
{
    AST_ASSERT(node != NULL, "Expected DEFS, got empty node.", NULL);

    while (node)
    {
        if (node->left->type == NODE_NVAR)
            STEP(compile_node(node->left, state));
        else if (node->left->type == NODE_NFUN)
            AST_ASSERT(
                func_array_add_func(&state->functions, node->left) == 0,
                "Function '%s' redefinition.", node->value.name);
        else    
            STEP(0 && "Unreachable code");

        node = node->right;
    }
    
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

/*
    TODO: USE THE GODDAMNED RECURSIVE DESCENT, STOP WITH THOSE EVENTS
    Seriously though, you're using custom stack to emulate stack frames. WHY??!?
*/

// ir_node* compile_node(const ast_node* node, compilation_state* state)
// v
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

#define NODE_TYPE(name, ...) define_compile(name);
#include "data_structures/types/node_types.h"
#undef NODE_TYPE


/* Wow, four almost identical functions :(
    Sadly, C preprocessor cannot fix this problem */
// On second thought, this is is not actually required in recursive solution
static bool on_compiling_left(const ast_node *node, compilation_state *state)
{
    switch (node->type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: return compile(name, COMPILING_LEFT);
    #include "data_structures/types/node_types.h"
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
    #include "data_structures/types/node_types.h"
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
    #include "data_structures/types/node_types.h"
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
    #include "data_structures/types/node_types.h"
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
            table_stack_add_var(&state->name_scope,
                                                        node->value.name);
            if (table_stack_is_at_global_scope(&state->name_scope))
                ++ state->global_var_cnt;

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
    const function* self = NULL;
    switch (stage)
    {
        case STAGE_COMPILING_LEFT:
            state->func_name = node->value.name;
            state->has_return = false;
            state->stack_frame_size = 0;

            // Function arguments in separate scope, starting at [rbp+16]
            table_stack_add_table(&state->name_scope, -16);

            self = func_array_find_func(&state->functions, state->func_name);
            
            // Add root node for others to reference
            state_add_ir_node(state, self->ir_list_head);
            // push rbp
            state_add_ir_node(state, ir_node_new_push_reg(IR_REG_RBP));
            // mov rbp, rsp
            state_add_ir_node(state, ir_node_new_binary(IR_MOV,
                                             ir_operand_reg(IR_REG_RBP),
                                             ir_operand_reg(IR_REG_RSP)));
            // Save function start to fill stack frame info
            ir_stack_push(&state->ir_stack, state->ir_tail);

            state->func_return = ir_node_new_empty();                                  
            return true;
        case STAGE_COMPILED_LEFT:
            table_stack_add_table(&state->name_scope, 8);
            return true;
        case STAGE_COMPILED_RIGHT:
            table_stack_pop_table(&state->name_scope);
            AST_ASSERT(state->has_return, "Control reaches end of function '%s'", node->value.name);
            state->func_name = NULL;
            state->has_return = false;
            table_stack_pop_table(&state->name_scope);
            state_add_ir_node(state, state->func_return);
            state->func_return = NULL;
            state->stack_frame_size -= 8;       // Discard initial offset

            if (state->stack_frame_size > 0)   // Create stack frame
            {
                ir_node* func_start = ir_stack_top(&state->ir_stack);
                const size_t frame_size = state->stack_frame_size;

                // sub rsp, FRAME_SIZE
                ir_node* frame_create = ir_node_new_binary(IR_SUB,
                                            ir_operand_reg(IR_REG_RSP),
                                            ir_operand_imm((long)frame_size));
                // add rsp, FRAME_SIZE
                ir_node* frame_destroy = ir_node_new_binary(IR_ADD,
                                            ir_operand_reg(IR_REG_RSP),
                                            ir_operand_imm((long)frame_size));
                
                ir_list_insert_after(func_start, frame_create);
                state_add_ir_node(state, frame_destroy);
            }

            ir_stack_pop(&state->ir_stack);
            state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RBP));
            state_add_ir_node(state, ir_node_new_ret());
            return true;

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
            table_stack_add_table(&state->name_scope,
                                  table_stack_get_next_offset(
                                            &state->name_scope));       
            return true;
        case STAGE_COMPILED_RIGHT:
            state->block_depth--;
            // Update frame size
            state->stack_frame_size = max_long(state->stack_frame_size,
                                               table_stack_get_next_offset(
                                                        &state->name_scope));       

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

static inline bool is_cmp(op_type op)
{
    return op == OP_EQ || op == OP_NEQ || op == OP_LT || op == OP_GT
        || op == OP_GEQ || op == OP_LEQ;
}

define_compile(OP)
{
    if (stage == STAGE_COMPILED_LEFT && node->value.op == OP_DIV)
    {
        // Multiply left operand by 1000
        state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RAX));
        state_add_ir_node(state, ir_node_new_binary(IR_MUL,
                                                    ir_operand_reg(IR_REG_RAX),
                                                    ir_operand_imm(1000)));
        state_add_ir_node(state, ir_node_new_push_reg(IR_REG_RAX));
        return true;
    }

    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    if (node->value.op == OP_NOT)
    {
        const ir_operand stack_top = {.flags = IR_OPERAND_REG | IR_OPERAND_MEM,
                                      .reg   = IR_REG_RSP };

        state_add_ir_node(state, ir_node_new_binary(IR_MOV,
                                                ir_operand_reg(IR_REG_RBX),
                                                ir_operand_imm(1)));
        state_add_ir_node(state, ir_node_new_binary(IR_XOR,
                                                ir_operand_reg(IR_REG_RAX),
                                                ir_operand_reg(IR_REG_RAX)));
        state_add_ir_node(state, ir_node_new_binary(IR_TEST,
                                                stack_top,
                                                ir_operand_imm(-1)));
        ir_node* cmov = ir_node_new_binary(IR_CMOV,
                                                ir_operand_reg(IR_REG_RAX),
                                            ir_operand_reg(IR_REG_RBX));
        cmov->flags = IR_COND_NOT_EQUAL;
        state_add_ir_node(state, cmov);
        state_add_ir_node(state, ir_node_new_binary(IR_MOV,
                                                stack_top,
                                                ir_operand_reg(IR_REG_RAX)));
        return true;
    }

    if (node->value.op == OP_NEG)
    {
        ir_node* neg = ir_node_new_empty();
        neg->is_valid = true;
        neg->operation = IR_NEG;
        neg->operand1 = {.flags = IR_OPERAND_REG | IR_OPERAND_MEM,
                         .reg   = IR_REG_RSP };
        state_add_ir_node(state, neg);
        return true;
    }

    state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RBX));
    state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RAX));

    ir_operand op1 = ir_operand_reg(IR_REG_RAX);
    ir_operand op2 = ir_operand_reg(IR_REG_RBX);
    ir_node* cmov = NULL;

    if (is_cmp(node->value.op))
    {
        state_add_ir_node(state, ir_node_new_binary(IR_XOR,
                                                ir_operand_reg(IR_REG_RDX),
                                                ir_operand_reg(IR_REG_RDX)));
        state_add_ir_node(state, ir_node_new_binary(IR_CMP, op1, op2));
        state_add_ir_node(state, ir_node_new_binary(IR_MOV, op1,
                                                    ir_operand_imm(1)));
        cmov = ir_node_new_binary(IR_CMOV, ir_operand_reg(IR_REG_RDX), op1);
        state_add_ir_node(state, cmov);
        state_add_ir_node(state, ir_node_new_binary(IR_MOV, op1,
                                                ir_operand_reg(IR_REG_RDX)));
    }

    switch (node->value.op)
    {
    case OP_ADD:
        state_add_ir_node(state, ir_node_new_binary(IR_ADD, op1, op2));
        break;
    case OP_SUB:
        state_add_ir_node(state, ir_node_new_binary(IR_SUB, op1, op2));
        break;
    case OP_AND:
        state_add_ir_node(state, ir_node_new_binary(IR_AND, op1, op2));
        break;
    case OP_OR:
        state_add_ir_node(state, ir_node_new_binary(IR_OR, op1, op2));
        break;

    case OP_MUL:
        state_add_ir_node(state, ir_node_new_binary(IR_MUL, op1, op2));
        state_add_ir_node(state, ir_node_new_binary(IR_MOV, op2,
                                                    ir_operand_imm(1000)));
    /* fallthrough */
    case OP_DIV:
        state_add_ir_node(state, ir_node_new_binary(IR_XOR,
                                                ir_operand_reg(IR_REG_RDX),
                                                ir_operand_reg(IR_REG_RDX)));
        state_add_ir_node(state, ir_node_new_binary(IR_DIV, op2, {}));
        break;

    case OP_LT:  cmov->flags = IR_COND_LESS;        break;
    case OP_GT:  cmov->flags = IR_COND_GREATER;     break;
    case OP_LEQ: cmov->flags = IR_COND_NOT_GREATER; break;
    case OP_GEQ: cmov->flags = IR_COND_NOT_LESS;    break;
    case OP_EQ:  cmov->flags = IR_COND_EQUAL;       break;
    case OP_NEQ: cmov->flags = IR_COND_NOT_EQUAL;   break;

    case OP_NOT:
    case OP_NEG:
    default:
        LOG_ASSERT(0 && "Unreachable code", return false);
        return false;
    }

    state_add_ir_node(state, ir_node_new_push_reg(IR_REG_RAX));
    return true;
}

define_compile(SEQ)
{
    return true;    // Nothing to do here
}

define_compile(ASS)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    long addr = 0;
    bool is_global = false;
    bool exists = table_stack_find_var(&state->name_scope, node->value.name,
                                        &is_global, &addr);

    AST_ASSERT(exists, "Undefined reference to variable '%s'.", node->value.name);

    ir_node* pop = ir_node_new_empty();
    pop->is_valid = true;
    pop->operation = IR_POP;
    pop->operand1 = {.flags = IR_OPERAND_MEM | IR_OPERAND_IMM,
                      .reg = IR_REG_NONE,
                      .immediate = addr };
    if (!is_global)
    {
        pop->operand1.flags |= IR_OPERAND_REG;
        pop->operand1.reg = IR_REG_RBP;
        pop->operand1.immediate *= -1; // Stack grows in opposite direction
    }
    state_add_ir_node(state, pop);

    return true;
}

define_compile(WHILE)
{
    ir_node* start_node = NULL;
    ir_node* jmp_node = NULL;
    ir_node* end_node = NULL;
    switch (stage)
    {
    case STAGE_COMPILING_LEFT:
        start_node = ir_node_new_empty();
        state_add_ir_node(state, start_node);
        ir_stack_push(&state->ir_stack, start_node);
        return true;
    case STAGE_COMPILED_LEFT:
        state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RAX));
        state_add_ir_node(state, ir_node_new_binary(IR_TEST,
                                                ir_operand_reg(IR_REG_RAX),
                                                ir_operand_reg(IR_REG_RAX)));
        jmp_node = ir_node_new_empty();
        jmp_node->is_valid = true;
        jmp_node->operation = IR_JMP;
        jmp_node->flags = IR_COND_EQUAL;        // Jump to end if 0
        state_add_ir_node(state, jmp_node);
        ir_stack_push(&state->ir_stack, jmp_node);
        return true;
    case STAGE_COMPILING_RIGHT:
        return true;
    case STAGE_COMPILED_RIGHT:
        end_node = ir_node_new_empty();
        jmp_node = ir_stack_top(&state->ir_stack);
        ir_stack_pop(&state->ir_stack);
        jmp_node->jump_target = end_node;       // Set target for start jump

        start_node = ir_stack_top(&state->ir_stack);
        jmp_node = ir_node_new_empty();
        jmp_node->is_valid = true;
        jmp_node->operation = IR_JMP;
        jmp_node->jump_target = start_node;     // Set target for main loop jump

        state_add_ir_node(state, jmp_node);
        state_add_ir_node(state, end_node);
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
    return true;        // Nothing to do here
}

define_compile(BRANCH)
{
    ir_node* jmp_node = NULL;
    ir_node* else_node = NULL;
    ir_node* end_node = NULL;
    switch (stage)
    {
    case STAGE_COMPILING_LEFT:
        state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RAX));
        state_add_ir_node(state, ir_node_new_binary(IR_TEST,
                                                ir_operand_reg(IR_REG_RAX),
                                                ir_operand_reg(IR_REG_RAX)));
        jmp_node = ir_node_new_empty();
        jmp_node->is_valid = true;
        jmp_node->operation = IR_JMP;
        jmp_node->flags = IR_COND_EQUAL;        // Jump to false if 0
        state_add_ir_node(state, jmp_node);
        ir_stack_push(&state->ir_stack, jmp_node);
        return true;
    case STAGE_COMPILED_LEFT:
        else_node = ir_node_new_empty();
        jmp_node = ir_stack_top(&state->ir_stack);
        ir_stack_pop(&state->ir_stack);
        jmp_node->jump_target = else_node;  // Set jump target for jump to else

        jmp_node = ir_node_new_empty();
        jmp_node->is_valid = true;
        jmp_node->operation = IR_JMP;       // Jump to end to skip 'else'
        ir_stack_push(&state->ir_stack, jmp_node);

        state_add_ir_node(state, jmp_node);
        state_add_ir_node(state, else_node);
        return true;
    case STAGE_COMPILING_RIGHT:
        return true;
    case STAGE_COMPILED_RIGHT:
        end_node = ir_node_new_empty();
        jmp_node = ir_stack_top(&state->ir_stack);
        jmp_node->jump_target = end_node;       // Set jump target to skip
                                                // 'else' branch
        state_add_ir_node(state, end_node);
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
    state_add_ir_node(state, ir_node_new_call(func->ir_list_head));
    if (args > 0)   // Free argument space
        state_add_ir_node(state, ir_node_new_binary(IR_ADD,
                                                ir_operand_reg(IR_REG_RSP),
                                                ir_operand_imm((long)(8*args))));
    state_add_ir_node(state, ir_node_new_push_reg(IR_REG_RAX));

    return true;
}

define_compile(PAR)
{
    return true; // Nothing to do here
}

define_compile(RET)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    if (state->block_depth == 1)
        state->has_return = true;

    ir_node* jmp_node = ir_node_new_empty();
    jmp_node->is_valid = true;
    jmp_node->operation = IR_JMP;
    jmp_node->jump_target = state->func_return;

    state_add_ir_node(state, ir_node_new_pop_reg(IR_REG_RAX));
    state_add_ir_node(state, jmp_node);

    return true;
}

define_compile(CONST)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    ir_node* push_node = ir_node_new_empty();
    push_node->is_valid = true;
    push_node->operation = IR_PUSH;
    push_node->operand1 = ir_operand_imm((long)(node->value.num*1000));
    state_add_ir_node(state, push_node);
    return true;
}

define_compile(VAR)
{
    if (stage != STAGE_COMPILED_RIGHT) return true; // Nothing to do here

    long addr = 0;
    bool is_global = false;
    bool exists = table_stack_find_var(&state->name_scope, node->value.name,
                                        &is_global, &addr);

    AST_ASSERT(exists, "Undefined reference to variable '%s'.", node->value.name);

    ir_node* push = ir_node_new_empty();
    push->is_valid = true;
    push->operation = IR_PUSH;
    push->operand1 = {.flags = IR_OPERAND_MEM | IR_OPERAND_IMM,
                      .reg = IR_REG_NONE,
                      .immediate = addr };
    if (!is_global)
    {
        push->operand1.flags |= IR_OPERAND_REG;
        push->operand1.reg = IR_REG_RBP;
        push->operand1.immediate *= -1; // Stack grows in opposite direction
    }

    state_add_ir_node(state, push);

    return true;
}

define_compile(CMP) { return false; }
define_compile(LOGIC) { return false; }
