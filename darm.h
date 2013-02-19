#ifndef __DARM__
#define __DARM__

#include "armv7-tbl.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

typedef enum _darm_reg_t {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
    FP = 0b1011, IP = 0b1100, SP = 0b1101, LR = 0b1110, PC = 0b1111,
} darm_reg_t;

typedef struct _darm_t {
    // the original encoded instruction
    uint32_t        w;

    // the instruction label
    armv7_instr_t   instr;
    armv7_enctype_t instr_type;

    // conditional flags, if any
    uint32_t        cond;
    // does this instruction update the conditional flags?
    uint32_t        S;

    // register operands
    uint32_t        Rd; // destination
    uint32_t        Rn; // first operand
    uint32_t        Rm; // second operand

    // immediate operand
    uint32_t        op_imm;

    // if op-imm is a label, then this flag specifies whether the immediate
    // should be added or subtracted from the current instruction pointer
    uint32_t        add;

    // register shift info
    uint32_t        type;
    uint32_t        shift_is_reg; // shift from register Rs or not?
    uint32_t        Rs;
    uint32_t        shift;
} darm_t;

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

int armv7_disassemble(darm_t *d, uint32_t w);

#endif
