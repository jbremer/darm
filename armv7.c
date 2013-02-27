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
#include <string.h>
#include "darm.h"
#include "armv7-tbl.h"

#define BITMSK_12 ((1 << 12) - 1)
#define BITMSK_16 ((1 << 16) - 1)
#define BITMSK_24 ((1 << 24) - 1)

#define ROR(val, rotate) (((val) >> (rotate)) | ((val) << (32 - (rotate))))

// the upper four bits define the rotation value, but we have to multiply the
// rotation value by two, so instead of right shifting by eight, we do a
// right shift of seven, effectively avoiding the left shift of one
#define ARMExpandImm(imm12) ROR((imm12) & 0xff, ((imm12) >> 7) & 0b11110)

static struct {
    const char *mnemonic_extension;
    const char *meaning_integer;
    const char *meaning_fp;
} g_condition_codes[] = {
    {"EQ", "Equal", "Equal"},
    {"NE", "Not equal", "Not equal, or unordered"},
    {"CS", "Carry Set", "Greater than, equal, or unordered"},
    {"CC", "Carry Clear", "Less than"},
    {"MI", "Minus, negative", "Less than"},
    {"PL", "Plus, positive or zero", "Greater than, equal, or unordered"},
    {"VS", "Overflow", "Unordered"},
    {"VC", "No overflow", "Not unordered"},
    {"HI", "Unsigned higher", "Greater than, unordered"},
    {"LS", "Unsigned lower or same", "Greater than, or unordered"},
    {"GE", "Signed greater than or equal", "Greater than, or unordered"},
    {"LT", "Signed less than", "Less than, or unordered"},
    {"GT", "Signed greater than", "Greater than"},
    {"LE", "Signed less than or equal", "Less than, equal, or unordered"},
    {"AL", "Always (unconditional)", "Always (unconditional)"},

    // alias for CS
    {"HS", "Carry Set", "Greater than, equal, or unordered"},
    // alias for CC
    {"LO", "Carry Clear", "Less than"},
};

static const char *shift_types[] = {
    "LSL", "LSR", "ASR", "ROR",
};

int darm_immshift_decode(const darm_t *d, const char **type,
    uint32_t *immediate)
{
    if(d->type == 0 && d->shift == 0) {
        *type = NULL, *immediate = 0;
        return -1;
    }
    else if(d->type == 0b11 && d->Rs == 0) {
        *type = "RRX", *immediate = 0;
    }
    else {
        *type = darm_shift_type_name(d->type);
        *immediate = d->shift;

        // 32 is encoded as 0
        if((d->type == 0b01 || d->type == 0b10) && d->shift == 0) {
            *immediate = 32;
        }
    }
    return 0;
}

static int armv7_disas_uncond(darm_t *d, uint32_t w)
{
    d->instr_type = T_UNCOND;

    // there are not a lot of unconditional instructions, so the following
    // values are a bit hardcoded
    switch ((w >> 25) & 0b111) {
    case 0b000:
        d->instr = I_SETEND;
        d->E = (w >> 9) & 1;
        return 0;

    case 0b010:
        // if the 21th bit is set, then it's one of the CLREX, DMB, DSB or ISB
        // instructions
        if((w >> 21) & 1) {
            d->instr = type_uncond2_instr_lookup[(w >> 4) & 0b111];
            if(d->instr != I_INVLD) {
                // if the instruction is either DMB, DSB or ISB, then the last
                // four bits represent an "option"
                if(d->instr != I_CLREX) {
                    d->option = w & 0b1111;
                }
                return 0;
            }
        }
        // otherwise, if the 21th bit is not set, it's either the PLD or the
        // PLI instruction
        // we fall-through here, as 0b011 also handles the PLD and PLI
        // instructions

    case 0b011:
        // if the 24th bit is set, then this is a PLD instruction, otherwise
        // it's a PLI instruction
        d->instr = (w >> 24) & 1 ? I_PLD : I_PLI;

        d->Rn = (w >> 16) & 0b1111;
        d->U = (w >> 23) & 1;

        // if the 25th bit is set, then this instruction takes a shifted
        // register as offset, otherwise, it takes an immediate as offset
        if((w >> 25) & 1) {
            d->Rm = w & 0b1111;
            d->shift_is_reg = B_UNSET;
            d->type = (w >> 5) & 0b11;
            d->shift = (w >> 7) & 0b11111;
        }
        else {
            d->I = B_SET;
            d->imm = w & BITMSK_12;
        }
        return 0;

    case 0b101:
        d->instr = I_BLX;
        d->H = (w >> 24) & 1;
        d->imm = w & BITMSK_24;
        d->I = B_SET;

        // check if the highest bit of the imm24 is set, if so, we
        // manually sign-extend the integer
        if((d->imm >> 23) & 1) {
            d->imm = (d->imm | 0xff000000) << 2;
        }
        else {
            d->imm = d->imm << 2;
        }

        // add the H bit
        d->imm |= d->H << 1;
        return 0;
    }
    return -1;
}

static int armv7_disas_cond(darm_t *d, uint32_t w)
{
    // we first handle some exceptions for MUL, STR, and LDR-like
    // instructions, which don't fit in the regular table (as they interfere
    // with the other instructions)

    // we have to check two parts of the encoded instruction, namely bits
    // 25..27 which should be zero, and bits 4..7, of which bit 4 and bit 7
    // should be one
    const uint32_t mask = (0b111 << 25) | (0b1001 << 4);
    if((w & mask) == (0b1001 << 4)) {

        // all variants of the MUL instruction
        if(((w >> 24) & 1) == 0 && ((w >> 4) & 0b1111) == 0b1001) {

            d->instr = type_mul_instr_lookup[(w >> 21) & 0b111];
            d->instr_type = T_MUL;

            // except for UMAAL and MLS, every variant takes the S bit
            d->S = (w >> 20) & 1;

            // each variant takes Rm and Rn
            d->Rm = (w >> 8) & 0b1111;
            d->Rn = w & 0b1111;

            // if this is the UMAAL or MLS instruction *and* the S bit is set,
            // then this is an invalid instruction
            if((d->instr == I_UMAAL || d->instr == I_MLS) && d->S != 0) {
                return -1;
            }

            switch ((uint32_t) d->instr) {
            case I_MLA: case I_MLS:
                d->Ra = (w >> 12) & 0b1111;
                // fall-through

            case I_MUL:
                d->Rd = (w >> 16) & 0b1111;
                break;

            case I_UMAAL: case I_UMULL: case I_UMLAL: case I_SMULL:
            case I_SMLAL:
                d->RdHi = (w >> 16) & 0b1111;
                d->RdLo = (w >> 12) & 0b1111;
                break;
            }
            return 0;
        }
        else if(((w >> 24) & 1) == 0 && ((w >> 5) & 0b11) != 0 &&
                (w >> 21) & 1) {

            // the high 2 bits are represented by the 5th and 6th bit, the
            // lower bit is represented by the 20th bit
            uint32_t index = ((w >> 4) & 0b110) | ((w >> 20) & 1);
            d->instr = type_stack1_instr_lookup[index];
            if(d->instr == I_INVLD) return -1;

            d->instr_type = T_STACK1;
            d->Rn = (w >> 16) & 0b1111;
            d->Rt = (w >> 12) & 0b1111;
            d->P = (w >> 24) & 1;
            d->U = (w >> 23) & 1;
            d->F = ((w >> 22) & 1) == 0;

            // depending on the register form we either have to extract a
            // register or an immediate
            if(d->F != 0) {
                d->Rm = w & 0b1111;
            }
            else {
                // the four high bits start at bit 8, so we shift them right
                // to their destination
                d->imm = ((w >> 4) & 0b11110000) | (w & 0b1111);
                d->I = B_SET;
            }
            return 0;
        }
        else if(((w >> 5) & 0b11) != 0 && ((w >> 20) & 0b10010) != 0b00010) {

            // the high 2 bits are represented by the 5th and 6th bit, the
            // lower bit is represented by the 20th bit
            uint32_t index = ((w >> 4) & 0b110) | ((w >> 20) & 1);
            d->instr = type_stack2_instr_lookup[index];
            if(d->instr == I_INVLD) return -1;

            d->instr_type = T_STACK2;
            d->Rn = (w >> 16) & 0b1111;
            d->Rt = (w >> 12) & 0b1111;
            d->P = (w >> 24) & 1;
            d->U = (w >> 23) & 1;
            d->F = ((w >> 22) & 1) == 0;
            d->W = (w >> 21) & 1;

            // depending on the register form we either have to extract a
            // register or an immediate
            if(d->F != 0) {
                d->Rm = w & 0b1111;
            }
            else {
                // the four high bits start at bit 8, so we shift them right
                // to their destination
                d->imm = ((w >> 4) & 0b11110000) | (w & 0b1111);
                d->I = B_SET;
            }
            return 0;
        }
        // synchronization primitive instructions
        else if(((w >> 24) & 1) == 1 && ((w >> 4) & 0b1111) == 0b1001) {
            d->instr = type_sync_instr_lookup[(w >> 20) & 0b1111];
            d->instr_type = T_SYNC;
            d->Rn = (w >> 16) & 0b1111;
            switch ((uint32_t) d->instr) {
            case I_SWP: case I_SWPB:
                d->B = (w >> 22) & 1;
                d->Rt = (w >> 12) & 0b1111;
                d->Rt2 = w & 0b1111;
                return 0;

            case I_LDREX: case I_LDREXD: case I_LDREXB: case I_LDREXH:
                d->Rt = (w >> 12) & 0b1111;
                return 0;

            case I_STREX: case I_STREXD: case I_STREXB: case I_STREXH:
                d->Rd = (w >> 12) & 0b1111;
                d->Rt = w & 0b1111;
                return 0;
            }
        }
    }
    // handles the STR, STRT, LDR, LDRT, STRB, STRBT, LDRB, LDRBT stack
    // instructions, and the media instructions
    else if(((w >> 26) & 0b11) == 0b01) {

        // if both the 25th and the 4th bit are set, then this is a media
        // instruction, which is handled in the big switch-case statement
        const uint32_t media_mask = (1 << 25) | (1 << 4);
        if((w & media_mask) != media_mask) {
            d->instr = type_stack0_instr_lookup[(w >> 20) & 0b11111];
            d->instr_type = T_STACK0;

            d->Rn = (w >> 16) & 0b1111;
            d->Rt = (w >> 12) & 0b1111;

            // extract some flags
            d->P = (w >> 24) & 1;
            d->U = (w >> 23) & 1;
            d->W = (w >> 21) & 1;

            // if the 25th bit is not set, then this instruction takes an
            // immediate, otherwise, it takes a shifted register
            if(((w >> 25) & 1) == 0) {
                d->imm = w & BITMSK_12;
                d->I = B_SET;
            }
            else {
                d->shift_is_reg = B_UNSET;
                d->type = (w >> 5) & 0b11;
                d->shift = (w >> 7) & 0b11111;
                d->Rm = w & 0b1111;
            }

            // if Rn == SP and P = 1 and U = 0 and W = 1 and imm12 = 4 and
            // this is a STR instruction, then this is a PUSH instruction
            if(d->instr == I_STR && d->Rn == SP && d->P == 1 && d->U == 0 &&
                    d->W == 1 && d->imm == 4) {
                d->instr = I_PUSH;
            }
            // if Rn == SP and P = 0 and U = 1 and W = 0 and imm12 = 4 and
            // this is a LDR instruction, then this is a POP instruction
            else if(d->instr == I_LDR && d->Rn == SP && d->P == 0 &&
                    d->U == 1 && d->imm == 4) {
                d->instr = I_POP;
            }
            return 0;
        }
    }
    // handle saturating addition and subtraction instructions, these
    // instructions have various masks; of bits 20..27 bit 24 is set and bits
    // 21..22 specify which instruction this is, furthermore, bits 4..7
    // represent the value 0b0101
    const uint32_t mask2 = (0b11111001 << 20) | (0b1111 << 4);
    if((w & mask2) == ((1 << 24) | (0b0101 << 4))) {
        d->instr = type_sat_instr_lookup[(w >> 21) & 0b11];
        d->instr_type = T_SAT;
        d->Rn = (w >> 16) & 0b1111;
        d->Rd = (w >> 12) & 0b1111;
        d->Rm = w & 0b1111;
        return 0;
    }
    // handle packing, unpacking, saturation, and reversal instructions, these
    // instructions have the 4th bit set and bits 23..27 represent 0b01101
    const uint32_t mask3 = (0b11111 << 23) | (1 << 4);
    if((w & mask3) == ((0b01101 << 23) | (1 << 4))) {
        // some instructions are already handled elsewhere (namely, PKH, SEL,
        // REV, REV16, RBIT, and REVSH)
        uint32_t op1 = (w >> 20) & 0b111;
        uint32_t A = (w >> 16) & 0b1111;
        uint32_t op2 = (w >> 5) & 0b111;

        d->instr_type = T_PUSR;

        // the (SX|UX)T(A)(B|H)(16) instructions
        if(op2 == 0b011) {
            // op1 represents the upper three bits, and A = 0b1111 represents
            // the lower bit
            d->instr = type_pusr_instr_lookup[(op1 << 1) | (A == 0b1111)];
            if(d->instr != I_INVLD) {
                d->Rd = (w >> 12) & 0b1111;
                d->Rm = w & 0b1111;

                // rotation is shifted to the left by three, so we do this
                // directly in our shift as well
                d->rotate = (w >> 7) & 0b11000;

                // if A is not 0b1111, then A represents the Rn operand
                if(A != 0b1111) {
                    d->Rn = A;
                }
                return 0;
            }
        }

        // SSAT
        if((op1 & 0b010) == 0b010 && (op2 & 1) == 0) {
            // if the upper bit is set, then it's USAT, otherwise SSAT
            d->instr = (op1 >> 2) ? I_USAT : I_SSAT;
            d->imm = (w >> 16) & 0b11111;
            d->I = B_SET;
            // signed saturate adds one to the immediate
            if(d->instr == I_SSAT) {
                d->imm++;
            }
            d->Rd = (w >> 12) & 0b1111;
            d->shift_is_reg = B_UNSET;
            d->shift = (w >> 7) & 0b11111;
            d->type = (w >> 5) & 0b11;
            d->Rn = w & 0b1111;
            return 0;
        }

        // SSAT16 and USAT16
        if((op1 == 0b010 || op1 == 0b110) && op2 == 0b001) {
            d->instr = op1 == 0b010 ? I_SSAT16 : I_USAT16;
            d->imm = (w >> 16) & 0b1111;
            d->I = B_SET;
            // signed saturate 16 adds one to the immediate
            if(d->instr == I_SSAT16) {
                d->imm++;
            }
            d->Rd = (w >> 12) & 0b1111;
            d->Rn = w & 0b1111;
            return 0;
        }
    }

    // the instruction label
    d->instr = armv7_instr_labels[(w >> 20) & 0xff];
    d->instr_type = armv7_instr_types[(w >> 20) & 0xff];

    // do a lookup for the type of instruction
    switch (d->instr_type) {
    case T_INVLD: case T_UNCOND: case T_MUL: case T_STACK0: case T_STACK1:
    case T_STACK2: case T_SAT: case T_SYNC: case T_PUSR:
        return -1;

    case T_ARITH_SHIFT:
        d->S = (w >> 20) & 1;
        d->Rd = (w >> 12) & 0b1111;
        d->Rn = (w >> 16) & 0b1111;
        d->Rm = w & 0b1111;
        d->type = (w >> 5) & 0b11;

        // type == 1, shift with the value of the lower bits of Rs
        d->shift_is_reg = (w >> 4) & 1;
        if(d->shift_is_reg == B_SET) {
            d->Rs = (w >> 8) & 0b1111;
        }
        else {
            d->shift = (w >> 7) & 0b11111;
        }
        return 0;

    case T_ARITH_IMM:
        d->S = (w >> 20) & 1;
        d->Rd = (w >> 12) & 0b1111;
        d->Rn = (w >> 16) & 0b1111;
        d->imm = ARMExpandImm(w & BITMSK_12);
        d->I = B_SET;

        // check whether this instruction is in fact an ADR instruction
        if((d->instr == I_ADD || d->instr == I_SUB) &&
                d->S == 0 && d->Rn == PC) {
            d->instr = I_ADR, d->Rn = R_INVLD;
            d->U = (w >> 23) & 1;
        }
        return 0;

    case T_BITS:
        d->instr = type_bits_instr_lookup[(w >> 21) & 0b11];

        d->instr_type = T_BITS;
        d->Rd = (w >> 12) & 0b1111;
        d->Rn = w & 0b1111;
        d->lsb = (w >> 7) & 0b11111;

        // the bfi and bfc instructions specify the MSB, whereas the SBFX and
        // UBFX instructions specify the width minus one
        if(d->instr == I_BFI) {
            d->width = ((w >> 16) & 0b11111) - d->lsb;

            // if Rn is 0b1111, then this is in fact the BFC instruction
            if(d->Rn == 0b1111) {
                d->Rn = R_INVLD;
                d->instr = I_BFC;
            }
        }
        else {
            d->width = ((w >> 16) & 0b11111) + 1;
        }
        return 0;

    case T_BRNCHSC:
        d->imm = w & BITMSK_24;
        d->I = B_SET;

        // if the instruction is B or BL, then we have to sign-extend it and
        // multiply it with four
        if(d->instr != I_SVC) {
            // check if the highest bit of the imm24 is set, if so, we
            // manually sign-extend the integer
            if((d->imm >> 23) & 1) {
                d->imm = (d->imm | 0xff000000) << 2;
            }
            else {
                d->imm = d->imm << 2;
            }
        }
        return 0;

    case T_BRNCHMISC:
        // first get the real instruction label
        d->instr = type_brnchmisc_instr_lookup[(w >> 4) & 0b1111];

        // now we do a switch statement based on the instruction label,
        // rather than some magic values
        switch ((uint32_t) d->instr) {
        case I_BKPT:
            d->imm = (((w >> 8) & BITMSK_12) << 4) + (w & 0b1111);
            d->I = B_SET;
            return 0;

        case I_BX: case I_BXJ: case I_BLX:
            d->Rm = w & 0b1111;
            return 0;

        case I_MSR:
            d->Rn = w & 0b1111;
            d->imm = (w >> 18) & 0b11;
            d->I = B_SET;
            return 0;

        case I_QSUB: case I_SMLAW: case I_SMULW: default:
            // returns -1
            break;
        }
        break;

    case T_MOV_IMM:
        d->Rd = (w >> 12) & 0b1111;
        d->imm = w & BITMSK_12;
        d->I = B_SET;

        // the MOV and MVN instructions have an S bit
        if(d->instr == I_MOV || d->instr == I_MVN) {
            d->S = (w >> 20) & 1;

            // the immediate values of the MOV and MVN instructions have to
            // be decoded
            d->imm = ARMExpandImm(d->imm);
        }
        // the MOVW and the MOVT instructions take another 4 bits of immediate
        else {
            d->imm |= ((w >> 16) & 0b1111) << 12;
        }
        return 0;

    case T_CMP_OP:
        d->Rn = (w >> 16) & 0b1111;
        d->Rm = w & 0b1111;
        d->type = (w >> 5) & 0b11;

        // type == 1, shift with the value of the lower bits of Rs
        d->shift_is_reg = (w >> 4) & 1;
        if(d->shift_is_reg == B_SET) {
            d->Rs = (w >> 8) & 0b1111;
        }
        else {
            d->shift = (w >> 7) & 0b11111;
        }
        return 0;

    case T_CMP_IMM:
        d->Rn = (w >> 16) & 0b1111;
        d->imm = ARMExpandImm(w & BITMSK_12);
        d->I = B_SET;
        return 0;

    case T_OPLESS:
        d->instr = type_opless_instr_lookup[w & 0b111];
        return d->instr == I_INVLD ? -1 : 0;

    case T_DST_SRC:
        d->instr = type_shift_instr_lookup[(w >> 4) & 0b1111];
        if(d->instr == I_INVLD) return -1;

        d->S = (w >> 20) & 1;
        d->Rd = (w >> 12) & 0b1111;
        d->type = (w >> 5) & 0b11;
        if((w >> 4) & 1) {
            d->Rm = (w >> 8) & 0b1111;
            d->Rn = w & 0b1111;
        }
        else {
            d->shift_is_reg = B_UNSET;
            d->Rm = w & 0b1111;
            d->shift = (w >> 7) & 0b11111;

            // if this is a LSL instruction with a zero shift, then it's
            // actually a MOV instruction
            if(d->instr == I_LSL && d->type == 0 && d->shift == 0) {
                d->instr = I_MOV;

                // if Rd and Rm are zero, then this is a NOP instruction
                if(d->Rd == 0 && d->Rm == 0) {
                    d->instr = I_NOP;
                    d->Rd = d->Rm = R_INVLD;
                }
            }

            // if this is a ROR instruction with a zero shift, then it's
            // actually a RRX instruction
            else if(d->instr == I_ROR && d->type == 0b11 &&
                    d->shift == 0) {
                d->instr = I_RRX;
            }
        }

        return 0;

    case T_LDSTREGS:
        d->W = (w >> 21) & 1;
        d->Rn = (w >> 16) & 0b1111;
        d->reglist = w & BITMSK_16;

        // if this is the LDM instruction and W = 1 and Rn = SP then this is
        // a POP instruction
        if(d->instr == I_LDM && d->W == 1 && d->Rn == SP) {
            d->instr = I_POP;
        }
        // if this is the STMDB instruction and W = 1 and Rn = SP then this is
        // the PUSH instruction
        else if(d->instr == I_STMDB && d->W == 1 && d->Rn == SP) {
            d->instr = I_PUSH;
        }
        return 0;

    case T_BITREV:
        d->Rd = (w >> 12) & 0b1111;
        d->Rm = w & 0b1111;

        // if this is the REV16 instruction and bits 4..7 are 0b0011, then
        // this is in fact the REV instruction
        if(d->instr == I_REV16 && ((w >> 4) & 0b1111) == 0b0011) {
            d->instr = I_REV;
        }
        // if this is the REVSH instruction and bits 4..7 are 0b0011, then
        // this is in fact the RBIT instruction
        else if(d->instr == I_REVSH && ((w >> 4) & 0b1111) == 0b0011) {
            d->instr = I_RBIT;
        }
        return 0;

    case T_MISC:
        switch ((uint32_t) d->instr) {
        case I_MVN:
            d->S = (w >> 20) & 1;
            d->Rd = (w >> 12) & 0b1111;
            d->shift_is_reg = (w >> 4) & 1;
            d->type = (w >> 5) & 0b11;
            d->Rm = w & 0b1111;
            if(d->shift_is_reg == B_UNSET) {
                d->shift = (w >> 7) & 0b11111;
            }
            else {
                d->Rs = (w >> 8) & 0b1111;
            }
            return 0;

        case I_DBG:
            d->option = w & 0b1111;
            return 0;

        case I_SMC:
            // if the 7th bit is 1, then this is the SMUL instruction
            if((w >> 7) & 1) {
                d->instr = I_SMUL;
                d->instr_type = T_SM;
                d->Rd = (w >> 16) & 0b1111;
                d->Rm = (w >> 8) & 0b1111;
                d->M  = (w >> 6) & 1;
                d->N  = (w >> 5) & 1;
                d->Rn = w & 0b1111;
            }
            else {
                d->imm = w & 0b1111;
                d->I = B_SET;
            }
            return 0;

        case I_SEL:
            d->Rd = (w >> 12) & 0b1111;
            d->Rn = (w >> 16) & 0b1111;
            d->Rm = w & 0b1111;

            // the SEL and PKH instructions share the same 8-bit identifier,
            // if the 5th bit is set, then this is the SEL instruction,
            // otherwise it's the PKH instruction
            if(((w >> 5) & 1) == 0) {
                d->instr = I_PKH;
                d->shift_is_reg = B_UNSET;
                d->type = (w >> 5) & 0b10;
                d->shift = (w >> 7) & 0b11111;
                d->T = (w >> 6) & 1;
            }
            return 0;
        }

    case T_SM:
        switch ((uint32_t) d->instr) {
        case I_SMMUL:
            d->Rd = (w >> 16) & 0b1111;
            d->Ra = (w >> 12) & 0b1111;
            d->Rm = (w >> 8) & 0b1111;
            d->R  = (w >> 5) & 1;
            d->Rn = w & 0b1111;

            // this can be either the SMMUL, the SMMLA, or the SMMLS
            // instruction, depending on the 6th bit and Ra
            if((w >> 6) & 1) {
                d->instr = I_SMMLS;
            }
            // if it's SMMUL instruction, but Ra is not 0b1111, then this is
            // the SMMLA instruction
            else if(d->Ra != 0b1111) {
                d->instr = I_SMMLA;
            }
            return 0;

        case I_SMUSD:
            d->Rd = (w >> 16) & 0b1111;
            d->Ra = (w >> 12) & 0b1111;
            d->Rm = (w >> 8) & 0b1111;
            d->M  = (w >> 5) & 1;
            d->Rn = w & 0b1111;

            // this can be either the SMLAD, the SMLSD, the SMUAD, or the
            // SMUSD instruction, depending on the 6th bit and Ra
            if((w >> 6) & 1 && d->Rn != 0b1111) {
                d->instr = I_SMLSD;
            }
            else if(((w >> 6) & 1) == 0) {
                d->instr = d->Ra == 0b1111 ? I_SMUAD : I_SMLAD;
            }
            return 0;

        case I_SMLSLD:
            d->RdHi = (w >> 16) & 0b1111;
            d->RdLo = (w >> 12) & 0b1111;
            d->Rm = (w >> 8) & 0b1111;
            d->M = (w >> 5) & 1;
            d->Rn = w & 0b1111;

            // if the 6th bit is zero, then this is in fact the SMLALD
            // instruction
            if(((w >> 6) & 1) == 0) {
                d->instr = I_SMLALD;
            }
            return 0;

        case I_SMLA:
            d->Rd = (w >> 16) & 0b1111;
            d->Ra = (w >> 12) & 0b1111;
            d->Rm = (w >> 8) & 0b1111;
            d->M  = (w >> 6) & 1;
            d->N  = (w >> 5) & 1;
            d->Rn = w & 0b1111;
            return 0;

        case I_SMLAL:
            d->RdHi = (w >> 16) & 0b1111;
            d->RdLo = (w >> 12) & 0b1111;
            d->Rm = (w >> 8) & 0b1111;
            d->M  = (w >> 6) & 1;
            d->N  = (w >> 5) & 1;
            d->Rn = w & 0b1111;
            return 0;

        case I_SMUL:
            // SMUL overlaps with SMC, so we define SMUL in SMC..
            break;
        }

    case T_PAS:
        // we have a lookup table with size 64, for all parallel signed and
        // unsigned addition and subtraction instructions
        // the upper three bits are represented by bits 20..22, so we only
        // right-shift those 17 bytes, the lower three bits are represented
        // by bits 5..7
        d->instr = type_pas_instr_lookup[((w >> 17) & 0b111000) |
                                         ((w >> 5) & 0b111)];
        if(d->instr == I_INVLD) return -1;

        d->Rn = (w >> 16) & 0b1111;
        d->Rd = (w >> 12) & 0b1111;
        d->Rm = w & 0b1111;
        return 0;
    }
    return -1;
}

int darm_armv7_disasm(darm_t *d, uint32_t w)
{
    int ret = -1;

    // initialize the entire darm state, in order to make sure that no members
    // contain undefined data
    memset(d, 0, sizeof(darm_t));
    d->w = w;
    d->cond = (w >> 28) & 0b1111;
    d->instr = I_INVLD;
    d->instr_type = T_INVLD;
    d->shift_is_reg = d->I = B_INVLD;
    d->S = d->E = d->U = d->H = d->P = d->F = B_INVLD;
    d->R = d->T = d->W = d->M = d->N = d->B = B_INVLD;
    d->Rd = d->Rn = d->Rm = d->Ra = d->Rt = R_INVLD;
    d->Rt2 = d->RdHi = d->RdLo = d->Rs = R_INVLD;
    d->option = O_INVLD;

    if(d->cond == 0b1111) {
        ret = armv7_disas_uncond(d, w);
    }
    else {
        ret = armv7_disas_cond(d, w);
    }

    // return error
    if(ret < 0) return ret;

    // TODO

    return 0;
}

const char *darm_mnemonic_name(armv7_instr_t instr)
{
    return instr < ARRAYSIZE(armv7_mnemonics) ?
        armv7_mnemonics[instr] : NULL;
}

const char *darm_enctype_name(armv7_enctype_t enctype)
{
    return enctype < ARRAYSIZE(armv7_enctypes) ?
        armv7_enctypes[enctype] : NULL;
}

const char *darm_register_name(darm_reg_t reg)
{
    return reg != R_INVLD && reg < (int32_t) ARRAYSIZE(armv7_registers) ?
        armv7_registers[reg] : NULL;
}

const char *darm_shift_type_name(darm_shift_type_t shifttype)
{
    return shifttype < (int32_t) ARRAYSIZE(shift_types) ?
        shift_types[shifttype] : NULL;
}

const char *darm_condition_name(darm_cond_t cond, int omit_always_execute)
{
    // we don't give the AL postfix, as almost every instruction would need
    // one then
    if(omit_always_execute != 0 && cond == C_AL) return "";

    return cond != C_INVLD && cond < (int32_t) ARRAYSIZE(g_condition_codes) ?
        g_condition_codes[cond].mnemonic_extension : NULL;
}

const char *darm_condition_meaning_int(darm_cond_t cond)
{
    return cond != C_INVLD && cond < (int32_t) ARRAYSIZE(g_condition_codes) ?
        g_condition_codes[cond].meaning_integer : NULL;
}

const char *darm_condition_meaning_fp(darm_cond_t cond)
{
    return cond != C_INVLD && cond < (int32_t) ARRAYSIZE(g_condition_codes) ?
        g_condition_codes[cond].meaning_fp : NULL;
}

darm_cond_t darm_condition_index(const char *condition_code)
{
    if(condition_code == NULL) return -1;

    // the "AL" condition flag
    if(condition_code[0] == 0) return C_AL;

    for (uint32_t i = 0; i < ARRAYSIZE(g_condition_codes); i++) {
        if(!strcmp(condition_code, g_condition_codes[i].mnemonic_extension)) {
            return i;
        }
    }

    return C_INVLD;
}

int darm_reglist(uint16_t reglist, char *out)
{
    char *base = out;

    if(reglist == 0) return -1;

    *out++ = '{';

    while (reglist != 0) {
        // count trailing zero's
        int32_t reg, start = __builtin_ctz(reglist);

        // all registers have length two
        *(uint16_t *) out = *(uint16_t *) armv7_registers[start];
        out += 2;

        for (reg = start; reg == __builtin_ctz(reglist); reg++) {
            // unset this bit
            reglist &= ~(1 << reg);
        }

        // if reg is not start + 1, then this means that a series of
        // consecutive registers have been identified
        if(reg != start + 1) {
            // if reg is start + 2, then this means that two consecutive
            // registers have been found, but we prefer the notation
            // {r0,r1} over {r0-r1} in that case
            *out++ = reg == start + 2 ? ',' : '-';
            *(uint16_t *) out = *(uint16_t *) armv7_registers[reg-1];
            out += 2;
        }
        *out++ = ',';
    }

    out[-1] = '}';
    *out = 0;
    return out - base;
}

void darm_dump(const darm_t *d)
{
    printf(
        "encoded:       0x%08x\n"
        "instr:         I_%s\n"
        "instr-type:    T_%s\n",
        d->w, darm_mnemonic_name(d->instr),
        darm_enctype_name(d->instr_type));

    if(d->cond == C_UNCOND) {
        printf("cond:          unconditional\n");
    }
    else if(d->cond != C_INVLD) {
        printf("cond:          C_%s\n", darm_condition_name(d->cond, 0));
    }

#define PRINT_REG(reg) if(d->reg != R_INVLD) \
    printf("%s:            %s\n", #reg, darm_register_name(d->reg))

    PRINT_REG(Rd);
    PRINT_REG(Rn);
    PRINT_REG(Rm);
    PRINT_REG(Ra);
    PRINT_REG(Rt);
    PRINT_REG(Rt2);
    PRINT_REG(RdHi);
    PRINT_REG(RdLo);

    if(d->I == B_SET) {
        printf("imm:           0x%08x  %d\n", d->imm, d->imm);
    }

#define PRINT_FLAG(flag, comment) if(d->flag != B_INVLD) \
    printf("%s:             %d   (%s)\n", #flag, d->flag, comment)

    PRINT_FLAG(B, "swap one byte or four bytes?");
    PRINT_FLAG(S, "does this instruction update conditional flags?");
    PRINT_FLAG(E, "endian specifier for SETEND");
    PRINT_FLAG(U, "add or subtract the offset?");
    PRINT_FLAG(H, "branch to 2-byte aligned Thumb2 instruction");
    PRINT_FLAG(P, "pre- or post-indexed addressing");
    PRINT_FLAG(F, "register-form or not?");
    PRINT_FLAG(M, "swap halfwords before operation");
    PRINT_FLAG(N, "which source operand to take");
    PRINT_FLAG(T, "tbform for PKH");
    PRINT_FLAG(R, "rounded results");
    PRINT_FLAG(W, "write-back bit");

    if(d->option != O_INVLD) {
        printf("option:        %d\n", d->option);
    }

    if(d->rotate != 0) {
        printf("rotate:        %d\n", d->rotate);
    }

    if(d->shift_is_reg != B_INVLD) {
        if(d->shift_is_reg == 0) {
            printf(
                "shift-is-reg:  %d   (is the operand register-shifted?)\n"
                "type:          %s (shift type)\n"
                "shift:         %-2d  (shift constant)\n",
                d->shift_is_reg, shift_types[d->type], d->shift);
        }
        else {
            printf(
                "shift-is-reg:  %d   (is the operand register-shifted?)\n"
                "type:          %s (shift type)\n"
                "Rs:            %s  (register-shift)\n",
                d->shift_is_reg, shift_types[d->type],
                darm_register_name(d->Rs));
        }
    }

    if(d->lsb != 0 || d->width != 0) {
        printf(
            "lsb:           %d\n"
            "width:         %d\n",
            d->lsb, d->width);
    }

    if(d->reglist != 0) {
        char reglist[64];
        darm_reglist(d->reglist, reglist);
        printf("reglist:       %s\n", reglist);
    }

    printf("\n");
}
