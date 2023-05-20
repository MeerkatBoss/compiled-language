/**
 * @file ir_bin_cvt.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-19
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __COMPILER_IR_BIN_CVT_H
#define __COMPILER_IR_BIN_CVT_H

#include "data_structures/intermediate_repr/ir.h"

/**
 * @brief Fills IR list with compiled operation bytes
 *
 * @param[inout] ir_list_head	Head of IR list
 *
 */
void ir_to_binary(ir_node* ir_list_head, size_t base_offset);

#endif /* ir_bin_cvt.h */
