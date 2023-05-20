#include "ir.h"

#define ARRAY_ELEMENT ir_node_ptr

static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src)
{
    *dest = *src;
}

static inline void delete_element(ARRAY_ELEMENT* element)
{
    *element = NULL;
}

#include "array/dynamic_array_impl.h"

#undef ARRAY_ELEMENT

static void ir_node_dump(const ir_node* node, FILE* output);

void ir_list_dump(ir_node* head, FILE* output)
{
    ir_node* current = head;
    size_t cnt = 1;
    while (current)
    {
        current->node_id = cnt++;
        current = current->next;
    }

    fputs("[\n", output);
    current = head;
    while (current)
    {
        ir_node_dump(current, output);
        current = current->next;

        if (current) fputc(',', output);
        fputc('\n', output);
    }
    fputs("]\n", output);
}

void ir_list_write(const ir_node* head, FILE* output)
{
    const ir_node* current = head;
    while (current)
    {
        fwrite(current->bytes, sizeof(char), current->encoded_length, output);
        current = current->next;
    }
}

void ir_list_clear(ir_node* head)
{
    ir_node* current = head;
    while (current)
    {
        ir_node* tmp = current;
        current = current->next;
        free(tmp);
    }
}


static void ir_op_dump(ir_op operation, FILE* output);
static void ir_operand_dump(const ir_operand* operand, FILE* output);
static void ir_register_dump(ir_reg reg, FILE* output);
static void ir_cond_dump(ir_cond_flags cond, FILE* output);

static void ir_node_dump(const ir_node* node, FILE* output)
{
    fputs("{\n", output);

    fprintf(output, "  \"id\": %zu,\n", node->node_id);
    fprintf(output, "  \"is_valid\": \"%s\",\n",
                        node->is_valid ? "true" : "false");

    fputs("  \"type\": ", output);
    ir_op_dump(node->operation, output);
    fputs(",\n", output);

    fprintf(output, "  \"operands\": [\n");
    ir_operand_dump(&node->operand1, output);
    fputs(",\n", output);
    ir_operand_dump(&node->operand2, output);
    fputs("\n  ],\n", output);

    fprintf(output, "  \"cflags\": ");
    ir_cond_dump(node->flags, output);
    fputs(",\n", output);

    if (node->jump_target)
        fprintf(output, "  \"jump\": %zu,\n", node->jump_target->node_id);

    fprintf(output, "  \"address\": %zu,\n", node->addr);
    
    fprintf(output, "  \"length\": %zu", node->encoded_length);

    if (node->encoded_length)
    {
        fputs(",\n", output);
        fputs("  \"bytes\": \"", output);
        for (size_t i = 0; i < node->encoded_length; ++i)
            fprintf(output, "%02X ", node->bytes[i]);
        fputc('"', output);
    }

    fputs("\n}", output);
}

static void ir_op_dump(ir_op operation, FILE* output)
{
    switch (operation)
    {
    case IR_NOP:  fputs("\"NOP\"",  output); break;
    case IR_MOV:  fputs("\"MOV\"",  output); break;
    case IR_CMOV: fputs("\"CMOV\"", output); break;
    case IR_PUSH: fputs("\"PUSH\"", output); break;
    case IR_POP:  fputs("\"POP\"",  output); break;

    case IR_ADD: fputs("\"ADD\"", output); break;
    case IR_SUB: fputs("\"SUB\"", output); break;
    case IR_MUL: fputs("\"MUL\"", output); break;
    case IR_DIV: fputs("\"DIV\"", output); break;
    case IR_NEG: fputs("\"NEG\"", output); break;

    case IR_AND: fputs("\"AND\"", output); break;
    case IR_OR:  fputs("\"OR\"",  output); break;
    case IR_XOR: fputs("\"XOR\"", output); break;
    case IR_NOT: fputs("\"NOT\"", output); break;

    case IR_CMP:  fputs("\"CMP\"",  output); break;
    case IR_TEST: fputs("\"TEST\"", output); break;

    case IR_JMP:     fputs("\"JMP\"",     output); break;
    case IR_CALL:    fputs("\"CALL\"",    output); break;
    case IR_RET:     fputs("\"RET\"",     output); break;
    case IR_SYSCALL: fputs("\"SYSCALL\"", output); break;

    default:
        fputs("\"UNKNOWN\"", output);
        break;
    }
}

static void ir_operand_dump(const ir_operand* operand, FILE* output)
{
    fputs("    {\n", output);

    fputs("      \"flags\": \"", output);
    if (operand->flags & IR_OPERAND_IMM)
        fputc('i', output);
    if (operand->flags & IR_OPERAND_REG)
        fputc('r', output);
    if (operand->flags & IR_OPERAND_MEM)
        fputc('m', output);
    fputc('"', output);

    if (operand->flags & IR_OPERAND_REG)
    {
        fputs(",\n      \"register\": ", output);
        ir_register_dump(operand->reg, output);
    }

    if (operand->flags & IR_OPERAND_IMM)
        fprintf(output, ",\n      \"immediate\": %ld", operand->immediate);

    fputs("\n    }", output);
}

static void ir_register_dump(ir_reg reg, FILE* output)
{
    switch (reg)
    {
    case IR_REG_NONE: fputs("\"NONE\"", output); break;

    case IR_REG_RAX: fputs("\"RAX\"", output); break;
    case IR_REG_RBX: fputs("\"RBX\"", output); break;
    case IR_REG_RCX: fputs("\"RCX\"", output); break;
    case IR_REG_RDX: fputs("\"RDX\"", output); break;

    case IR_REG_RSI: fputs("\"RSI\"", output); break;
    case IR_REG_RDI: fputs("\"RDI\"", output); break;
    case IR_REG_RSP: fputs("\"RSP\"", output); break;
    case IR_REG_RBP: fputs("\"RBP\"", output); break;

    case IR_REG_R8:  fputs("\"R8\"",  output); break;
    case IR_REG_R9:  fputs("\"R9\"",  output); break;
    case IR_REG_R10: fputs("\"R10\"", output); break;
    case IR_REG_R11: fputs("\"R11\"", output); break;

    case IR_REG_R12: fputs("\"R12\"", output); break;
    case IR_REG_R13: fputs("\"R13\"", output); break;
    case IR_REG_R14: fputs("\"R14\"", output); break;
    case IR_REG_R15: fputs("\"R15\"", output); break;

    default:
        fputs("\"UNKNOWN\"", output);
        break;
    }
}

static void ir_cond_dump(ir_cond_flags cond, FILE* output)
{
    switch (cond)
    {
    case IR_COND_NONE:          fputs("\"NONE\"",           output); break;

    case IR_COND_GREATER:       fputs("\"GREATER\"",        output); break;
    case IR_COND_NOT_GREATER:   fputs("\"NOT_GREATER\"",    output); break;
                       
    case IR_COND_LESS:          fputs("\"LESS\"",           output); break;
    case IR_COND_NOT_LESS:      fputs("\"NOT_LESS\"",       output); break;

    case IR_COND_EQUAL:         fputs("\"EQUAL\"",          output); break;
    case IR_COND_NOT_EQUAL:     fputs("\"NOT_EQUAL\"",      output); break;

    default:
        fputs("\"UNKNOWN\"", output);
        break;
    }
}

