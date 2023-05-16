/**
 * @file ir.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __DATA_STRUCTURES_INTERMEDIATE_REPR_IR_H
#define __DATA_STRUCTURES_INTERMEDIATE_REPR_IR_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

enum ir_op
{
    IR_NOP = 0,

    IR_MOV,  IR_CMOV,
    IR_PUSH, IR_POP,

    IR_ADD,  IR_SUB,
    IR_MUL,  IR_DIV,
    IR_NEG,

    IR_AND,  IR_OR,
    IR_XOR,  IR_NOT,

    IR_JMP,

    IR_CALL, IR_RET,
    IR_SYSCALL
};

enum ir_cond_flags
{
    IR_COND_NONE = 0,
    IR_COND_GREATER, IR_COND_NOT_GREATER,
    IR_COND_LESS,    IR_COND_NOT_LESS,
    IR_COND_EQUAL,   IR_COND_NOT_EQUAL
};

enum ir_reg
{
    IR_REG_NONE = 0,
    IR_REG_RAX, IR_REG_RBX, IR_REG_RCX, IR_REG_RDX,
    IR_REG_RSI, IR_REG_RDI, IR_REG_RSP, IR_REG_RBP,
    IR_REG_R8,  IR_REG_R9,  IR_REG_R10, IR_REG_R11,
    IR_REG_R12, IR_REG_R13, IR_REG_R14, IR_REG_R15
};

enum ir_operand_flags
{
    IR_OPERAND_NONE = 0,
    IR_OPERAND_IMM  = 1,
    IR_OPERAND_REG  = 2,
    IR_OPERAND_MEM  = 4
};

struct ir_operand
{
    ir_operand_flags    flags;
    ir_reg              reg;
    long                immediate;
};

#define ir_operand_reg(reg) \
    { .flags = IR_OPERAND_REG, .reg = (reg), .immediate = 0 };

#define ir_operand_imm(imm) \
    { .flags = IR_OPERAND_IMM, .reg = IR_REG_NONE, .immediate = imm };

struct ir_node;

typedef ir_node* ir_node_ptr;

struct ir_node
{
    size_t          node_id;
    bool is_valid;

    ir_op           operation;
    ir_operand      operand1,
                    operand2;
    ir_cond_flags   flags;
    ir_node_ptr     jump_target;
    size_t          addr;

    size_t          encoded_length;
    unsigned char   bytes[16];

    ir_node_ptr     next;
};

ir_node* ir_node_new_empty(void);
ir_node* ir_node_new_call(ir_node_ptr function);
ir_node* ir_node_new_binary(ir_op operation, ir_operand dest, ir_operand src);
ir_node* ir_node_new_syscall(void);

/**
 * @brief Print verbose information about IR to specified file
 *
 * @param[in] head      Start of IR linked list
 * @param[in] output    Output stream
 *
 */
void ir_list_dump(ir_node* head, FILE* output);

/**
 * @brief Write compiled IR to file
 *
 * @param[in] head      Start of IR linked list
 * @param[in] output    Output stream (in binary mode)
 *
 */
void ir_list_write(const ir_node* head, FILE* output);

/**
 * @brief Free all nodes in a linked list of IR nodes
 *
 * @param[in] head      Start of IR linked list
 *
 */
void ir_list_clear(ir_node* head);

#define ARRAY_ELEMENT ir_node_ptr
#include "array/dynamic_array.h"
#undef ARRAY_ELEMENT

typedef dynamic_array(ir_node_ptr) ir_node_stack;

__always_inline
static void ir_stack_ctor(ir_node_stack* stack)
{
    array_ctor(stack);
}

__always_inline
static void ir_stack_dtor(ir_node_stack* stack)
{
    array_dtor(stack);
}

__always_inline
static void ir_stack_push(ir_node_stack* stack, ir_node* node)
{
    array_push(stack, node);
}
__always_inline
static void ir_stack_pop(ir_node_stack* stack)
{
    array_pop(stack);
}
__always_inline
static ir_node* ir_stack_top(const ir_node_stack* stack)
{
    return *array_back(stack);
}


#endif /* ir.h */
