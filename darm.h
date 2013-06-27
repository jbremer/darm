/*
Copyright (c) 2013, Jurriaan Bremer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the darm developer(s) nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __DARM__
#define __DARM__

#include "armv7-tbl.h"
#include "thumb-tbl.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define B_UNSET 0
#define B_SET   1
#define B_INVLD 2

typedef enum _darm_reg_t {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
    FP = b1011, IP = b1100, SP = b1101, LR = b1110, PC = b1111,

    R_INVLD = -1
} darm_reg_t;

typedef enum _darm_cond_t {
    C_EQ, C_NE, C_CS, C_CC, C_MI, C_PL, C_VS,
    C_VC, C_HI, C_LS, C_GE, C_LT, C_GT, C_LE, C_AL,

    C_HS = C_CS, C_LO = C_CC,
    C_UNCOND = b1111,

    C_INVLD = -1
} darm_cond_t;

typedef enum _darm_shift_type_t {
    S_LSL, S_LSR, S_ASR, S_ROR,

    S_INVLD = -1,
} darm_shift_type_t;

typedef enum _darm_option_t {
    O_SY    = b1111,
    O_ST    = b1110,
    O_ISH   = b1011,
    O_ISHST = b1010,
    O_NSH   = b0111,
    O_NSHST = b0110,
    O_OSH   = b0011,
    O_OSHST = b0010,

    O_INVLD = -1,
} darm_option_t;

typedef struct _darm_t {
    // the original encoded instruction
    uint32_t        w;

    // the instruction label
    darm_instr_t    instr;
    darm_enctype_t  instr_type;

    // conditional flags, if any
    darm_cond_t     cond;

    // if set, swap only one byte, otherwise swap four bytes
    uint32_t        B;

    // does this instruction update the conditional flags?
    uint32_t        S;

    // endian specifier for the SETEND instruction
    uint32_t        E;

    // whether halfwords should be swapped before various signed
    // multiplication operations
    uint32_t        M;

    // specifies, together with the M flag, which half of the source
    // operand is used to multiply
    uint32_t        N;

    // option operand for the DMB, DSB and ISB instructions
    darm_option_t   option;

    // to add or to subtract the immediate, this is used for instructions
    // which take a relative offset to a pointer or to the program counter
    uint32_t        U;

    // the bit for the unconditional BLX instruction which allows one to
    // branch with link to a 2-byte aligned thumb2 instruction
    uint32_t        H;

    // specifies whether this instruction uses pre-indexed addressing or
    // post-indexed addressing
    uint32_t        P;

    // specifies whether signed multiplication results should be rounded
    // or not
    uint32_t        R;

    // the PKH instruction has two variants, namely, PKHBT and PKHTB, the
    // tbform is represented by T, i.e., if T = 1 then the instruction is
    // PKHTB, otherwise it's PKHBT
    uint32_t        T;

    // write-back bit
    uint32_t        W;

    // flag which specifies whether an immediate has been set
    uint32_t        I;

    // rotation value
    uint32_t        rotate;

    // register operands
    darm_reg_t      Rd; // destination
    darm_reg_t      Rn; // first operand
    darm_reg_t      Rm; // second operand
    darm_reg_t      Ra; // accumulate operand
    darm_reg_t      Rt; // transferred operand
    darm_reg_t      Rt2; // second transferred operand

    // for instructions which produce a 64bit output we have to specify a
    // high and a low 32bits destination register
    darm_reg_t      RdHi; // high 32bits destination
    darm_reg_t      RdLo; // low 32bits destination

    // immediate operand
    uint32_t        imm;

    // register shift info
    darm_shift_type_t shift_type;
    darm_reg_t      Rs;
    uint32_t        shift;

    // certain instructions operate on bits, they specify the lowest
    // significant bit to be used, as well as the width, the amount of bits
    // that are affected
    uint32_t        lsb;
    uint32_t        width;

    // bitmask of registers affected by the STM/LDM/PUSH/POP instruction
    uint16_t        reglist;

    // indicates arm vs thumb
    uint8_t         isthumb;
} darm_t;

typedef struct _darm_str_t {
    // the full mnemonic, including extensions, flags, etc.
    char mnemonic[12];

    // a representation of each argument in a separate string
    char arg[4][32];

    // representation of shifting, if present
    char shift[12];

    // the entire instruction
    char instr[64];
} darm_str_t;

// disassemble an armv7 instruction
int darm_armv7_disasm(darm_t *d, uint32_t w);

// disassemble a thumb instruction
int darm_thumb_disasm(darm_t *d, uint16_t w);

// disassemble a thumb2 instruction
int darm_thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2);

int darm_immshift_decode(const darm_t *d, const char **type,
    uint32_t *immediate);

const char *darm_mnemonic_name(darm_instr_t instr);
const char *darm_enctype_name(darm_enctype_t enctype);
const char *darm_register_name(darm_reg_t reg);
const char *darm_shift_type_name(darm_shift_type_t shifttype);

// postfix for each condition, e.g., EQ, NE
const char *darm_condition_name(darm_cond_t cond, int omit_always_execute);

// meaning if this condition is used for regular instructions
const char *darm_condition_meaning_int(darm_cond_t cond);

// meaning if this condition is used for floating point instructions
const char *darm_condition_meaning_fp(darm_cond_t cond);

// look up a condition code, e.g., "EQ" => C_EQ
darm_cond_t darm_condition_index(const char *condition_code);

int darm_reglist(uint16_t reglist, char *out);
void darm_dump(const darm_t *d);

int darm_str(const darm_t *d, darm_str_t *str);
int darm_str2(const darm_t *d, darm_str_t *str, int lowercase);

#endif
