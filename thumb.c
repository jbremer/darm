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
#include "thumb-tbl.h"

static int thumb_disasm(darm_t *d, uint16_t w)
{
    d->instr = thumb_instr_labels[w >> 8];
    d->instr_type = thumb_instr_types[w >> 8];

    switch ((uint32_t) d->instr_type) {
    case T_THUMB_ONLY_IMM8:
        d->I = B_SET;
        d->imm = w & 0xff;
        return 0;

    case T_THUMB_COND_BRANCH:
        d->cond = (w >> 8) & b1111;
        d->I = B_SET;
        d->imm = (uint32_t)(int8_t)(w & 0xff) << 1;
        return 0;

    case T_THUMB_UNCOND_BRANCH:
        d->I = B_SET;
        d->imm = w & ((1 << 11) - 1);

        // manually sign-extend it
        if(((d->imm >> 10) & 1) != 0) {
            d->imm |= ~((1 << 11) - 1);
        }

        // finally, shift it one byte to the left
        d->imm <<= 1;
        return 0;

    case T_THUMB_SHIFT_IMM:
        d->Rd = (w >> 0) & b111;
        d->Rm = (w >> 3) & b111;
        d->imm = (w >> 6) & b11111;

        // if the immediate is zero, then this is actually a mov instruction
        if(d->imm != 0) {
            d->I = B_SET;
        }
        else {
            d->instr = I_MOV;
        }
        return 0;

    case T_THUMB_STACK:
        d->I = B_SET;
        d->imm = (w & 0xff) << 2;
        d->Rn = SP;
        d->Rt = (w >> 8) & b111;
        d->U = B_SET;
        d->W = B_UNSET;
        d->P = B_SET;
        return 0;

    case T_THUMB_LDR_PC:
        d->I = B_SET;
        d->imm = (w & 0xff) << 2;
        d->Rn = PC;
        d->Rt = (w >> 8) & b111;
        d->U = B_SET;
        d->W = B_UNSET;
        d->P = B_SET;
        return 0;

    case T_THUMB_GPI:
        d->instr = type_gpi_instr_lookup[(w >> 6) & b1111];
        switch ((uint32_t) d->instr) {
        case I_AND: case I_EOR: case I_LSL: case I_LSR:
        case I_ASR: case I_ADC: case I_SBC: case I_ROR:
            d->Rd = d->Rn = w & b111;
            d->Rm = (w >> 3) & b111;
            return 0;

        case I_TST: case I_CMP: case I_CMN:
            d->Rn = w & b111;
            d->Rm = (w >> 3) & b111;
            return 0;

        case I_RSB:
            d->I = B_SET;
            d->imm = 0;
            d->Rd = w & b111;
            d->Rn = (w >> 3) & b111;
            return 0;

        case I_ORR: case I_BIC:
            d->Rn = w & b111;
            // fall-through as the mvn handler is almost the same, except
            // for parsing Rn

        case I_MVN:
            d->Rd = w & b111;
            d->Rm = (w >> 3) & b111;
            return 0;

        case I_MUL:
            d->Rd = d->Rm = w & b111;
            d->Rn = (w >> 3) & b111;
            return 0;
        }

    case T_THUMB_BRANCH_REG:
        d->instr = (w >> 7) & 1 ? I_BLX : I_BL;
        d->Rm = (w >> 3) & b1111;
        return 0;

    case T_THUMB_NO_OPERANDS:
        d->instr = type_no_op_instr_lookup[(w >> 4) & b11];
        return 0;

    case T_THUMB_HAS_IMM8:
        d->I = B_SET;
        d->imm = w & 0xff;

        switch ((uint32_t) d->instr) {
        case I_ADD: case I_SUB:
            d->Rd = d->Rn = (w >> 8) & b111;
            return 0;

        case I_ADR:
            d->Rn = PC;
            d->U = B_SET;
            d->imm <<= 2;
            // fall-through as adr also has to set Rd

        case I_MOV:
            d->Rd = (w >> 8) & b111;
            return 0;

        case I_CMP:
            d->Rn = (w >> 8) & b111;
            return 0;
        }
    }
    return -1;
}

int darm_thumb_disasm(darm_t *d, uint16_t w)
{
    memset(d, 0, sizeof(darm_t));
    // we set all conditional flags to "execute always" by default, as most
    // thumb instructions don't feature a conditional flag
    d->cond = C_AL;
    d->instr = I_INVLD;
    d->instr_type = T_INVLD;
    d->shift_type = S_INVLD;
    d->S = d->E = d->U = d->H = d->P = d->I = B_INVLD;
    d->R = d->T = d->W = d->M = d->N = d->B = B_INVLD;
    d->Rd = d->Rn = d->Rm = d->Ra = d->Rt = R_INVLD;
    d->Rt2 = d->RdHi = d->RdLo = d->Rs = R_INVLD;
    d->option = O_INVLD;
    // TODO set opc and coproc? to what value?
    d->CRn = d->CRm = d->CRd = R_INVLD;

    switch (w >> 11) {
    case 0b11101: case 0b11110: case 0b11111:
        return -1;

    default:
        d->w = w;
        return thumb_disasm(d, w);
    }
}
