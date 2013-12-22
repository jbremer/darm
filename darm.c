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

#include <stdio.h>
#include <stdint.h>
#include "darm.h"
#include "darm-instr.h"
#include "darm-internal.h"
#include "darm-tables.h"

typedef enum _darm_sm_opcode_t {
    // Halt execution, invalid instruction.
    SM_HLT,

    // Follow either branch of a node, depending on the particular bit
    // in the instruction.
    SM_STEP,

    // Some instructions are a more specific variant of another instruction.
    // In these cases, the more specific instruction will have a couple of
    // bits which are hardcoded and have to be checked in order to determine
    // as which encoding we will disassemble this instruction.
    SM_CMP4,

    // This instruction has been disassembled correctly, return success.
    SM_RETN,

    // Assign the instruction index.
    SM_INSTR,

    // Extract an immediate.
    SM_IMM,

    // Extract various general purpose registers.
    SM_Rd, SM_Rn, SM_Rm, SM_Ra, SM_Rt, SM_Rt2, SM_RdHi, SM_RdLo, SM_Rs,

    SM_ARMExpandImm,

    SM_S,
} darm_sm_opcode_t;

#define ROR(val, rotate) (((val) >> (rotate)) | ((val) << (32 - (rotate))))

// The upper four bits define the rotation value, but we have to multiply the
// rotation value by two, so instead of right shifting by eight, we do a
// right shift of seven, effectively avoiding the left shift of one.
#define ARMExpandImm(imm12) ROR((imm12) & 0xff, ((imm12) >> 7) & b11110)

static inline uint32_t _extract_field(uint32_t insn,
    uint32_t idx, uint32_t bits)
{
    return (insn >> idx) & ((1 << bits) - 1);
}

// Disassembles any instruction according to the state machine and lookup
// table. The input instruction can be either 16 or 32 bit, depending on
// the target state machine (which will have been created accordingly.)
static int darm_disassemble(darm_t *d, uint32_t insn,
    const uint8_t *sm, const uint16_t *lut)
{
    d->insn = insn;

    uint32_t off = 0, value;
    while (1) {
        switch ((darm_sm_opcode_t) sm[off++]) {
        case SM_HLT:
            return -1;

        case SM_STEP:
            value = (insn >> sm[off]) & 1;
            off = lut[sm[off+1] + sm[off+2]*256 + value];
            break;

        case SM_CMP4:
            value = _extract_field(insn, sm[off], 4) == sm[off+1];
            off = lut[sm[off+2] + sm[off+3]*256 + value];
            break;

        case SM_RETN:
            return 0;

        case SM_INSTR:
            d->instr = sm[off] + sm[off+1]*256;
            off += 2;
            break;

        case SM_IMM:
            d->imm = _extract_field(insn, 0, sm[off++]);
            break;

#define SM_REG(name) \
        case SM_##name: \
            d->name = _extract_field(insn, sm[off++], 4); \
            break;

        SM_REG(Rd); SM_REG(Rn); SM_REG(Rm); SM_REG(Ra); SM_REG(Rt);
        SM_REG(Rt2); SM_REG(RdHi); SM_REG(RdLo); SM_REG(Rs);

        case SM_ARMExpandImm:
            d->imm = ARMExpandImm(d->imm);
            break;

#define SM_FLAG(name, idx, bitsize) \
        case SM_##name: \
            d->name = _extract_field(insn, idx, bitsize); \
            break;

        SM_FLAG(S, 20, 1);

        }
    }
    return 0;
}

int darm_armv7(darm_t *d, uint32_t insn)
{
    return darm_disassemble(d, insn, g_armv7_sm, g_armv7_lut);
}
