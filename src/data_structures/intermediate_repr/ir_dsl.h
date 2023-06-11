/**
 * @file ir_dsl.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-17
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __DATA_STRUCTURES_INTERMEDIATE_REPR_IR_DSL_H
#define __DATA_STRUCTURES_INTERMEDIATE_REPR_IR_DSL_H

#include "ir.h"

#define ir_operand_reg(reg_name) \
    { .flags = IR_OPERAND_REG, .reg = (reg_name), .immediate = 0 }

#define ir_operand_imm(imm) \
    { .flags = IR_OPERAND_IMM, .reg = IR_REG_NONE, .immediate = imm }

inline ir_node* ir_node_new_empty(void)
{
    return (ir_node*) calloc(1, sizeof(ir_node));
}

inline ir_node* ir_node_new_call(ir_node_ptr function)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_CALL;
    node->jump_target = function;
    return node;
}

inline ir_node* ir_node_new_ret(void)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_RET;
    return node;
}

inline ir_node* ir_node_new_binary(ir_op operation,
                                   ir_operand dest, ir_operand src)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = operation;
    node->operand1 = dest;
    node->operand2 = src;
    return node;
}

inline ir_node* ir_node_new_syscall(void)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_SYSCALL;
    return node;
}

inline ir_node* ir_node_new_push_reg(ir_reg reg)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_PUSH;
    node->operand1 = ir_operand_reg(reg);
    return node;
}

inline ir_node* ir_node_new_push_imm(long imm)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_PUSH;
    node->operand1 = ir_operand_imm(imm);
    return node;
}

inline ir_node* ir_node_new_pop_reg(ir_reg reg)
{
    ir_node* node = ir_node_new_empty();
    node->is_valid = true;
    node->operation = IR_POP;
    node->operand1 = ir_operand_reg(reg);
    return node;
}


#endif /* ir_dsl.h */
