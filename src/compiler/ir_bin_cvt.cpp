#include <string.h>

#include "ir_bin_cvt.h"

static void ir_fill_opcodes(ir_node* ir_list_head, size_t base_offset);
static void ir_update_jumps(ir_node* ir_list_head);

void ir_to_binary(ir_node* ir_list_head, size_t base_offset)
{
    ir_fill_opcodes(ir_list_head, base_offset);
    ir_update_jumps(ir_list_head);
}

enum rex_bytes
{
    REX   = 0x40,
    REX_W = 0x08,
    REX_R = 0x04,
    REX_X = 0x02,
    REX_B = 0x01
};

enum reg_bytes
{
    REG_EAX = 00,
    REG_ECX = 01,
    REG_EDX = 02,
    REG_EBX = 03,
    REG_ESP = 04,
    REG_EBP = 05,
    REG_ESI = 06,
    REG_EDI = 07
};

enum cond_bytes
{
    COND_E  = 0x04,
    COND_NE = 0x05,
    COND_L  = 0x0C,
    COND_NL = 0x0D,
    COND_LE = 0x0E,
    COND_NLE= 0x0F,

    COND_Z  = COND_E,
    COND_NZ = COND_NE,
    COND_G  = COND_NLE,
    COND_GE = COND_NL,
    COND_NG = COND_LE,
    COND_NGE= COND_L
};

static void ir_convert_nop(ir_node* node);

static void ir_convert_mov (ir_node* node);
static void ir_convert_cmov(ir_node* node);
static void ir_convert_push(ir_node* node);
static void ir_convert_pop (ir_node* node);

static void ir_convert_add(ir_node* node);
static void ir_convert_sub(ir_node* node);
static void ir_convert_mul(ir_node* node);
static void ir_convert_div(ir_node* node);
static void ir_convert_neg(ir_node* node);

static void ir_convert_and(ir_node* node);
static void ir_convert_or (ir_node* node);
static void ir_convert_xor(ir_node* node);
static void ir_convert_not(ir_node* node);

static void ir_convert_cmp (ir_node* node);
static void ir_convert_test(ir_node* node);
static void ir_convert_jmp (ir_node* node);

static void ir_convert_call   (ir_node* node);
static void ir_convert_ret    (ir_node* node);
static void ir_convert_syscall(ir_node* node);


static void ir_fill_opcodes(ir_node* ir_list_head, size_t base_offset)
{
    ir_node* current = ir_list_head;
    size_t cur_addr = base_offset;
    while (current)
    {
        switch (current->operation)
        {
        case IR_NOP: ir_convert_nop(current); break;

        case IR_MOV:  ir_convert_mov (current); break;
        case IR_CMOV: ir_convert_cmov(current); break;
        case IR_PUSH: ir_convert_push(current); break;
        case IR_POP:  ir_convert_pop (current); break;

        case IR_ADD: ir_convert_add(current); break;
        case IR_SUB: ir_convert_sub(current); break;
        case IR_MUL: ir_convert_mul(current); break;
        case IR_DIV: ir_convert_div(current); break;
        case IR_NEG: ir_convert_neg(current); break;

        case IR_AND: ir_convert_and(current); break;
        case IR_OR:  ir_convert_or (current); break;
        case IR_XOR: ir_convert_xor(current); break;
        case IR_NOT: ir_convert_not(current); break;

        case IR_CMP:  ir_convert_cmp (current); break;
        case IR_JMP:  ir_convert_jmp (current); break;
        case IR_TEST: ir_convert_test(current); break;

        case IR_CALL:    ir_convert_call   (current); break;
        case IR_RET:     ir_convert_ret    (current); break;
        case IR_SYSCALL: ir_convert_syscall(current); break;

        default:    // Unreachable
            break;
        }
        current->addr = cur_addr;
        cur_addr += current->encoded_length;
        current = current->next;
    }
}

static void ir_update_jumps(ir_node* ir_list_head)
{
    ir_node* current = ir_list_head;
    while (current)
    {
        if (current->operation == IR_JMP || current->operation == IR_CALL)
        {
            size_t pref_size = 0;
            if (current->operation == IR_JMP && current->flags != IR_COND_NONE)
                pref_size = 2;
            else
                pref_size = 1;
            
            int offset = 0;
            if (current->jump_target)
                offset = - (int) (current->addr + current->encoded_length
                            - current->jump_target->addr);
            memcpy(current->bytes + pref_size, &offset, 4);
        }
        current = current->next;
    }
}

static inline unsigned char encode_cond(ir_cond_flags cond)
{
    switch (cond)
    {
    case IR_COND_GREATER:     return COND_G;
    case IR_COND_NOT_GREATER: return COND_NG;
    case IR_COND_LESS:        return COND_L;
    case IR_COND_NOT_LESS:    return COND_NL;
    case IR_COND_EQUAL:       return COND_E;
    case IR_COND_NOT_EQUAL:   return COND_NE;

    case IR_COND_NONE:
    default:
        return 0;
        break;
    }
}

static inline unsigned char encode_reg_lo(ir_reg reg)
{
    switch (reg)
    {
    case IR_REG_RAX: return REG_EAX;
    case IR_REG_RCX: return REG_ECX;
    case IR_REG_RDX: return REG_EDX;
    case IR_REG_RBX: return REG_EBX;
    case IR_REG_RSP: return REG_ESP;
    case IR_REG_RBP: return REG_EBP;
    case IR_REG_RSI: return REG_ESI;
    case IR_REG_RDI: return REG_EDI;
    default:    // Unreachable
        return 0;
        break;
    }
}

static inline unsigned char encode_reg_hi(ir_reg reg)
{
    return (int) reg >= (int) IR_REG_R8;
}

static inline unsigned char encode_reg_pair_rex(ir_reg dst, ir_reg src)
{
    return encode_reg_hi(dst) * REX_B | encode_reg_hi(src) * REX_R;
}

static inline unsigned char encode_reg_pair_mod(ir_reg dst, ir_reg src)
{
    //     v--- mod = 11
    return 0xC0 | encode_reg_lo(dst) | (encode_reg_lo(src) << 3);
}

static inline unsigned char encode_mem_rex(ir_operand mem)
{
    if (! (mem.flags & IR_OPERAND_REG)) // Memory is absolute address
        return 0;

    return encode_reg_hi(mem.reg) * REX_B;// Register encoded as base
}

static inline unsigned char encode_mem_mod(ir_operand mem)
{
    if (mem.flags & IR_OPERAND_REG) // Memory is relative address
        return 0x84;
    return 0x04;
}

static inline unsigned char encode_mem_sib(ir_operand mem)
{
    if (! (mem.flags & IR_OPERAND_REG))
        return 0x25;    // Absolute addressing
    return 0x20 | encode_reg_lo(mem.reg);
}

static void ir_convert_nop(ir_node* node)
{
    node->bytes[0] = 0x90;  // NOP
    node->encoded_length = 1;
}

static void ir_convert_mov(ir_node* node)
{
    node->bytes[0] = REX | REX_W;   // All MOVs are 64-bit

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // Between registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x89;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    if (node->operand2.flags == IR_OPERAND_IMM) // MOV immediate
    {
        size_t imm_start = 0;
        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[1] |= 0xB8 | encode_reg_lo(node->operand2.reg);
            imm_start = 2;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[1] = 0xC7;
            node->bytes[2] = encode_mem_mod(node->operand1);
            node->bytes[3] = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 8);
        node->encoded_length = imm_start + 8;
        return;
    }

    // MOV memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // MOV to memory
    {
        node->bytes[1] = 0x89;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // MOV from memory
    {
        node->bytes[1] = 0x8B;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_cmov(ir_node* node)
{
    node->bytes[0] = REX | REX_W;   // All MOVs are 64-bit

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // Between registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x0F;
        node->bytes[2] = 0x40 | encode_cond(node->flags);
        node->bytes[3] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 4;
        return;
    }
    
    // MOV from memory
    ir_reg reg = node->operand1.reg;
    ir_operand mem = node->operand2;

    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[1]  = 0x0F;
    node->bytes[2]  = 0x40 | encode_cond(node->flags);
    node->bytes[3]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[4]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+5, &offset, 4);
    node->encoded_length = 9;
    return;
}

static void ir_convert_push(ir_node* node)
{
    if (node->operand1.flags == IR_OPERAND_REG) // Push register
    {
        node->bytes[0] = REX | encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[1] = 0x50 | encode_reg_lo(node->operand1.reg);
        node->encoded_length = 2;
        return;
    }
    node->bytes[0] = REX | encode_mem_rex(node->operand1);
    node->bytes[1] = 0xFF;
    node->bytes[2] = (06 << 3) | encode_mem_mod(node->operand1);
    node->bytes[3] = encode_mem_sib(node->operand1);

    int offset = (int) node->operand1.immediate;
    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_pop (ir_node* node)
{
    if (node->operand1.flags == IR_OPERAND_REG) // Push register
    {
        node->bytes[0] = REX | encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[1] = 0x58 | encode_reg_lo(node->operand1.reg);
        node->encoded_length = 2;
        return;
    }
    node->bytes[0] = REX | encode_mem_rex(node->operand1);
    node->bytes[1] = 0x8F;
    node->bytes[2] = encode_mem_mod(node->operand1);
    node->bytes[3] = encode_mem_sib(node->operand1);

    int offset = (int) node->operand1.immediate;
    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_add(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // ADD registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x01;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    if (node->operand2.flags == IR_OPERAND_IMM) // ADD immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] = encode_mem_mod(node->operand1);
            node->bytes[3] = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    // ADD memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // ADD to memory
    {
        node->bytes[1] = 0x01;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // ADD to register
    {
        node->bytes[1] = 0x03;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_sub(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // SUB registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x29;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    if (node->operand2.flags == IR_OPERAND_IMM) // SUB immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xE8 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] = encode_mem_mod(node->operand1);
            node->bytes[3] = 0x28 | encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    // SUB memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // SUB from memory
    {
        node->bytes[1] = 0x29;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // SUB from register
    {
        node->bytes[1] = 0x2B;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_mul(ir_node* node)
{
    if (node->operand1.flags & IR_OPERAND_MEM)  // Cannot MUL to memory
    {
        node->bytes[0] = 0x90;
        node->encoded_length = 1;
        node->is_valid = false;
        return;
    }

    node->bytes[0] = REX | REX_W;
    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // MUL registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x0F;
        node->bytes[2] = 0xAF;
        node->bytes[3] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 4;
        return;
    }

    if (node->operand2.flags == IR_OPERAND_IMM) // MUL immediate
    {
        node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[1] = 0x69;
        node->bytes[2] |= 0xE8 | encode_reg_lo(node->operand1.reg);
        memcpy(node->bytes + 3, &node->operand1.immediate, 8);
        node->encoded_length = 12;
        return;
    }

    ir_reg reg     = node->operand1.reg;
    ir_operand mem = node->operand2;

    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[1]  = 0x6B;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_div(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    node->bytes[1] = 0xF7;
    node->bytes[2] = 0x38;

    if (node->operand1.flags == IR_OPERAND_REG)
    {
        node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[2] |= 0xC | encode_reg_lo(node->operand1.reg);
        node->encoded_length = 3;
        return;
    }

    node->bytes[0] |= encode_mem_rex(node->operand1);
    node->bytes[2] |= encode_mem_mod(node->operand1);
    node->bytes[3]  = encode_mem_sib(node->operand1);
    int offset = (int) node->operand1.immediate;
    memcpy(node->bytes + 4, &offset, 4);
    node->encoded_length = 8;
}

static void ir_convert_neg(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    node->bytes[1] = 0xF7;
    node->bytes[2] = 0x18;

    if (node->operand1.flags == IR_OPERAND_REG)
    {
        node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[2] |= 0xC | encode_reg_lo(node->operand1.reg);
        node->encoded_length = 3;
        return;
    }

    node->bytes[0] |= encode_mem_rex(node->operand1);
    node->bytes[2] |= encode_mem_mod(node->operand1);
    node->bytes[3]  = encode_mem_sib(node->operand1);
    int offset = (int) node->operand1.immediate;
    memcpy(node->bytes + 4, &offset, 4);
    node->encoded_length = 8;
}

static void ir_convert_and(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand2.flags == IR_OPERAND_IMM) // AND immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        node->bytes[2] = 0x20;

        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] |= encode_mem_mod(node->operand1);
            node->bytes[3]  = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // AND registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x21;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    // AND memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // AND to memory
    {
        node->bytes[1] = 0x21;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // AND to register
    {
        node->bytes[1] = 0x23;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_or(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand2.flags == IR_OPERAND_IMM) // OR immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        node->bytes[2] = 0x08;

        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] |= encode_mem_mod(node->operand1);
            node->bytes[3]  = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // OR registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x21;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    // OR memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // OR to memory
    {
        node->bytes[1] = 0x09;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // OR to register
    {
        node->bytes[1] = 0x0B;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_xor(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand2.flags == IR_OPERAND_IMM) // XOR immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        node->bytes[2] = 0x30;

        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] |= encode_mem_mod(node->operand1);
            node->bytes[3]  = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // XOR registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x33;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    // XOR memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // XOR to memory
    {
        node->bytes[1] = 0x31;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // OR to register
    {
        node->bytes[1] = 0x33;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_not(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    node->bytes[1] = 0xF7;
    node->bytes[2] = 0x10;

    if (node->operand1.flags == IR_OPERAND_REG)
    {
        node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
        node->bytes[2] |= 0xC | encode_reg_lo(node->operand1.reg);
        node->encoded_length = 3;
        return;
    }

    node->bytes[0] |= encode_mem_rex(node->operand1);
    node->bytes[2] |= encode_mem_mod(node->operand1);
    node->bytes[3]  = encode_mem_sib(node->operand1);
    int offset = (int) node->operand1.immediate;
    memcpy(node->bytes + 4, &offset, 4);
    node->encoded_length = 8;
}

static void ir_convert_cmp(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand2.flags == IR_OPERAND_IMM) // CMP immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0x81;
        node->bytes[2] = 0x38;

        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] |= encode_mem_mod(node->operand1);
            node->bytes[3]  = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // CMP registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x3B;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    // CMP memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // CMP memory
    {
        node->bytes[1] = 0x39;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // CMP register
    {
        node->bytes[1] = 0x3B;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_test(ir_node* node)
{
    node->bytes[0] = REX | REX_W;
    if (node->operand2.flags == IR_OPERAND_IMM) // CMP immediate
    {
        size_t imm_start = 0;
        node->bytes[1] = 0xF7;
        node->bytes[2] = 0x00;

        if (node->operand1.flags == IR_OPERAND_REG)
        {
            node->bytes[0] |= encode_reg_hi(node->operand1.reg) * REX_B;
            node->bytes[2] |= 0xC0 | encode_reg_lo(node->operand1.reg);
            imm_start = 3;
        }
        else
        {
            node->bytes[0] |= encode_mem_rex(node->operand1);
            node->bytes[2] |= encode_mem_mod(node->operand1);
            node->bytes[3]  = encode_mem_sib(node->operand1);
            int offset = (int) node->operand1.immediate;
            memcpy(node->bytes + 4, &offset, 4);
            imm_start = 8;
        }
        memcpy(node->bytes + imm_start, &node->operand2.immediate, 4);
        node->encoded_length = imm_start + 4;
        return;
    }

    if (node->operand1.flags == IR_OPERAND_REG &&
            node->operand2.flags == IR_OPERAND_REG) // CMP registers
    {
        node->bytes[0] |= encode_reg_pair_rex(node->operand1.reg,
                                              node->operand2.reg);
        node->bytes[1] = 0x85;
        node->bytes[2] = encode_reg_pair_mod(node->operand1.reg,
                                             node->operand2.reg);
        node->encoded_length = 3;
        return;
    }

    // CMP memory/reg
    ir_reg reg = IR_REG_NONE;
    ir_operand mem = {};
    if (node->operand1.flags & IR_OPERAND_MEM)  // CMP memory
    {
        node->bytes[1] = 0x85;
        reg = node->operand2.reg;
        mem = node->operand1;
    }
    else    // CMP register
    {
        node->bytes[1] = 0x85;
        reg = node->operand1.reg;
        mem = node->operand2;
    }
    node->bytes[0] |= encode_mem_rex(mem) | encode_reg_hi(reg) * REX_R;
    node->bytes[2]  = encode_mem_mod(mem) | (encode_reg_lo(reg) << 3);
    node->bytes[3]  = encode_mem_sib(mem);

    int offset = (int) mem.immediate;

    memcpy(node->bytes+4, &offset, 4);
    node->encoded_length = 8;
    return;
}

static void ir_convert_jmp(ir_node* node)
{
    if (node->flags == IR_COND_NONE)
    {
        node->bytes[0] = 0xE9;
        node->encoded_length = 5;
        return;
    }

    node->bytes[0] = 0x0F;
    node->bytes[1] = 0x80 | encode_cond(node->flags);
    node->encoded_length = 6;
    return;
}

static void ir_convert_call(ir_node* node)
{
    node->bytes[0] = 0xE8;
    node->encoded_length = 5;
    return;
}

static void ir_convert_ret(ir_node* node)
{
    node->bytes[0] = 0xC3;
    node->encoded_length = 1;
    return;
}

static void ir_convert_syscall(ir_node* node)
{
    node->bytes[0] = 0x0F;
    node->bytes[1] = 0x05;
    node->encoded_length = 2;
    return;
}

