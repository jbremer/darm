#ifndef __DARM__
#define __DARM__

#include "armv7-tbl.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define B_UNSET 0
#define B_SET   1
#define B_INVLD 2

typedef enum _darm_reg_t {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
    FP = 0b1011, IP = 0b1100, SP = 0b1101, LR = 0b1110, PC = 0b1111,

    R_INVLD = -1
} darm_reg_t;

typedef enum _darm_cond_t {
    C_EQ, C_NE, C_CS, C_CC, C_MI, C_PL, C_VS,
    C_VC, C_HI, C_LS, C_GE, C_LT, C_GT, C_LE, C_AL,

    C_HS = C_CS, C_LO = C_CC,
    C_UNCOND = 0b1111,

    C_INVLD = -1
} darm_cond_t;

typedef enum _armv7_option_t {
    O_SY    = 0b1111,
    O_ST    = 0b1110,
    O_ISH   = 0b1011,
    O_ISHST = 0b1010,
    O_NSH   = 0b0111,
    O_NSHST = 0b0110,
    O_OSH   = 0b0011,
    O_OSHST = 0b0010,

    O_INVLD = 0b10000,
} armv7_option_t;

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

    // specifies whether this instruction uses pre-indexed addressing or
    // post-indexed addressing
    uint32_t        P;

    // register-form specifies whether an immediate or a register should be
    // used as source operator, this value will then be added or subtracted
    // (see the U flag)
    uint32_t        R;

    // write-back bit
    uint32_t        W;

    // register operands
    darm_reg_t      Rd; // destination
    darm_reg_t      Rn; // first operand
    darm_reg_t      Rm; // second operand
    darm_reg_t      Ra; // accumulate operand
    darm_reg_t      Rt; // transferred operand

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

    // certain instructions operate on bits, they specify the lowest
    // significant bit to be used, as well as the width, the amount of bits
    // that are affected
    uint32_t        lsb;
    uint32_t        width;
} darm_t;

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

int armv7_disassemble(darm_t *d, uint32_t w);

const char *armv7_mnemonic_by_index(armv7_instr_t instr);
const char *armv7_enctype_by_index(armv7_enctype_t enctype);
const char *armv7_register_by_index(darm_reg_t reg);
const char *armv7_condition_by_index(darm_cond_t cond);
void darm_dump(const darm_t *d);

#endif
