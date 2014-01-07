/*
Copyright (c) 2013-2014, Jurriaan Bremer
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

// If the debug level has not been set, which is the default, then we set it
// to release mode (i.e., no debug statements.) In case you're debugging
// something, you'll likely want to set it to 1 or 2.
// Either set it manually, or give -DDARM_DBGLVL=2 on the commandline.
#ifndef DARM_DBGLVL
#define DARM_DBGLVL 0
#endif

#include "darm-instr.h"

#define B_UNSET 0
#define B_SET   1
#define B_IT    2

typedef enum _darm_reg_t {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,

    cr0, cr1, cr2, cr3, cr4, cr5, cr6, cr7, cr8,
    cr9, cr10, cr11, cr12, cr13, cr14, cr15,

    d0, d1, d2, d3, d4, d5, d6, d8, d9, d10, d11, d12, d13, d14, d15, d16,
    d17, d18, d19, d20, d21, d22, d23, d24, d25, d26, d27, d28, d29, d30, d31,

    R_REGCNT, R_INVLD = -1, R_BASE = r0, CR_BASE = cr0, FP_BASE = d0,

    FP = r11, IP = r12, SP = r13, LR = r14, PC = r15,

    q0  =  d0, q1  =  d2, q2  =  d4, q3  =  d6, q4  =  d8, q5  = d10,
    q6  = d12, q7  = d14, q8  = d16, q9  = d18, q10 = d20, q11 = d22,
    q12 = d24, q13 = d26, q14 = d28, q15 = d30,
} darm_reg_t;

typedef enum _darm_cond_t {
    C_EQ = 0, C_NE = 1, C_CS = 2, C_CC = 3, C_MI = 4,
    C_PL = 5, C_VS = 6, C_VC = 7, C_HI = 8, C_LS = 9,
    C_GE = 10, C_LT = 11, C_GT = 12, C_LE = 13, C_AL = 14,

    C_HS = C_CS, C_LO = C_CC, C_UNCOND = 15,

    C_CONDCNT, C_BASE = C_EQ,
} darm_cond_t;

typedef enum _darm_shift_type_t {
    S_LSL, S_LSR, S_ASR, S_ROR, S_RRX,

    S_SHFTTYPCNT, S_BASE = S_LSL,
} darm_shift_type_t;

typedef enum _darm_option_t {
    O_OSHST = 2,  // b0010
    O_OSH   = 3,  // b0011
    O_NSHST = 6,  // b0110
    O_NSH   = 7,  // b0111
    O_ISHST = 10, // b1010
    O_ISH   = 11, // b1011
    O_ST    = 14, // b1110
    O_SY    = 15, // b1111

    O_OPTIONCNT, O_INVLD = -1, O_BASE = 0,
} darm_option_t;

typedef enum _darm_coproc_t {
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,

    P_PROCCNT, P_INVLD = -1, P_BASE = p0,
} darm_coproc_t;

typedef struct _darm_t {
    uint32_t        insn;

    darm_instr_t    instr;
    darm_cond_t     cond;

    darm_reg_t      Rd;
    darm_reg_t      Rn;
    darm_reg_t      Rm;
    darm_reg_t      Ra;
    darm_reg_t      Rt;
    darm_reg_t      Rt2;
    darm_reg_t      RdHi;
    darm_reg_t      RdLo;
    darm_reg_t      Rs;

    uint32_t        imm;

    darm_shift_type_t shift_type;
    uint32_t        S, E, P, U, W;
    darm_option_t   option;
    uint32_t        lsb, msb, width;
    uint32_t        register_list;

    darm_coproc_t   coproc;
    uint32_t        opc1, opc2;
    darm_reg_t      CRd, CRn, CRm;

    uint32_t        mask, rotate, sat_imm;
    uint32_t        msr_mask, it_mask, first_cond;

    uint32_t        Vd, cmode, Q, size, align, index_align, T, a;
    uint32_t        Vm, Vn, F, len, op1, sz, B, op2;

    // Internal format for generation of human-readable strings.
    const uint8_t   *format;
} darm_t;

typedef struct _darm_string_t {
    // Full mnemonic, including possible S flag, condition, and wide postfix.
    char mnemonic[12];

    // Each operand.
    uint32_t argcnt;
    char arg[6][32];

    // Shift operand.
    char shift[12];

    // Entire instruction.
    char total[64];
} darm_string_t;

// Suggested size for a buffer when representing an instruction as string.
#define DARM_BUFLEN 64

int darm_armv7(darm_t *d, uint32_t insn);
int darm_thumb(darm_t *d, uint16_t w, uint16_t w2);

int darm_string(const darm_t *d, char *out);
int darm_string2(const darm_t *d, darm_string_t *str);

int darm_reglist(uint16_t reglist, char *out);

#endif
