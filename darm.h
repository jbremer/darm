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

#include "darm-instr.h"

#define B_UNSET 0
#define B_SET   1
#define B_INVLD 2

typedef enum _darm_reg_t {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,

    cr0, cr1, cr2, cr3, cr4, cr5, cr6, cr7, cr8,
    cr9, cr10, cr11, cr12, cr13, cr14, cr15,

    R_REGCNT, R_INVLD = -1,

    FP = r11, IP = r12, SP = r13, LR = r14, PC = r15, R_BASE = r0,
    CR_BASE = cr0,
} darm_reg_t;

typedef enum _darm_cond_t {
    C_EQ = 0, C_NE = 1, C_CS = 2, C_CC = 3, C_MI = 4,
    C_PL = 5, C_VS = 6, C_VC = 7, C_HI = 8, C_LS = 9,
    C_GE = 10, C_LT = 11, C_GT = 12, C_LE = 13, C_AL = 14,

    C_HS = C_CS, C_LO = C_CC, C_UNCOND = 15,

    C_INVLD = -1, C_BASE = C_EQ,
} darm_cond_t;

typedef enum _darm_shift_type_t {
    S_INVLD,
} darm_shift_type_t;

typedef enum _darm_option_t {
    O_INVLD,
} darm_option_t;

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

    uint32_t        S, E, P, U, W;
    darm_option_t   option;
    uint32_t        type, lsb, msb;
    uint32_t        register_list;

    // Internal format for generation of human-readable strings.
    const uint8_t   *format;
} darm_t;

// Suggested size for a buffer when representing an instruction as string.
#define DARM_BUFLEN 64

int darm_armv7(darm_t *d, uint32_t insn);
int darm_string(const darm_t *d, char *out);

int darm_reglist(uint16_t reglist, char *out);

#endif
