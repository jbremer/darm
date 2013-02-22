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

typedef enum _darm_cond_t {
    C_EQ, C_NE, C_CS, C_CC, C_MI, C_PL, C_VS,
    C_VC, C_HI, C_LS, C_GE, C_LT, C_GT, C_LE, C_AL,

    C_HS = C_CS, C_LO = C_CC,
} darm_cond_t;

typedef struct _darm_t {
    // the original encoded instruction
    uint32_t        w;

    // the instruction label
    armv7_instr_t   instr;
    armv7_enctype_t instr_type;

    // conditional flags, if any
    darm_cond_t     cond;

    // does this instruction update the conditional flags?
    uint32_t        S;

    // endian specifier for the SETEND instruction
    uint32_t        E;

    // option operand for the DMB, DSB and ISB instructions
    uint32_t        option;

    // to add or to subtract the immediate, this is used for instructions
    // which take a relative offset to a pointer or to the program counter
    uint32_t        U;

    // the bit for the unconditional BLX instruction which allows one to
    // branch with link to a 2-byte aligned thumb2 instruction
    uint32_t        H;

    // register operands
    darm_reg_t      Rd; // destination
    darm_reg_t      Rn; // first operand
    darm_reg_t      Rm; // second operand
    darm_reg_t      Ra; // accumulate operand

    // for instructions which produce a 64bit output we have to specify a
    // high and a low 32bits destination register
    darm_reg_t      RdHi; // high 32bits destination
    darm_reg_t      RdLo; // low 32bits destination

    // immediate operand
    uint32_t        imm;

    // register shift info
    uint32_t        type;
    uint32_t        shift_is_reg; // shift from register Rs or not?
    darm_reg_t      Rs;
    uint32_t        shift;
} darm_t;

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

int armv7_disassemble(darm_t *d, uint32_t w);

const char *armv7_mnemonic_by_index(armv7_instr_t instr);
const char *armv7_enctype_by_index(armv7_enctype_t enctype);
const char *armv7_register_by_index(darm_reg_t reg);
const char *armv7_condition_by_index(darm_cond_t cond);

#endif
