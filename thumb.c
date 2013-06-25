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

#define BITMSK_8 ((1 << 8) - 1)
static int thumb_disasm(darm_t *d, uint16_t w)
{
    uint16_t tmp = 0;
    d->instr = thumb_instr_labels[w >> 8];
    d->instr_type = thumb_instr_types[w >> 8];
    d->isthumb = 1;

    d->cond = C_AL;
    switch ((uint32_t) d->instr_type) {
    case T_THUMB_ONLY_IMM8:
        d->I = B_SET;
        d->imm = w & 0xff;
        return 0;
    case T_THUMB_PUSHPOP:
        if((w>>11) & 1)
            d->instr = I_POP;
        else
            d->instr = I_PUSH;
        d->reglist = ((w) & BITMSK_8);
        d->reglist |= (((w>>8)&1)<<14); // or in the LR(9th bit)
        return 0;
    case T_THUMB_REG_IMM:
        tmp = (w>>11) & 0b11;
        if(tmp == 0b10)
            d->instr = I_ADDS;
        else if(tmp == 0b11)
            d->instr = I_SUBS;
        else if(tmp == 0b00)
            d->instr = I_MOVS;
        d->Rd = (w >> 8) & 0b111;
        d->imm = w & BITMSK_8;
        d->I = B_SET;
        return 0;
    case T_THUMB_ARITH_REG_REG:
        tmp = (w>>9) & 0b11;
        if(tmp == 0b1)
            d->instr = I_SUBS;
        else if (tmp == 0b0)
            d->instr = I_ADDS;
        d->Rd = (w) & 0b111;
        d->Rn = (w>>3) & 0b111;
        d->Rm = (w>>6) & 0b111;
        return 0;
    }
    return -1;
}

int darm_thumb_disasm(darm_t *d, uint16_t w)
{
    memset(d, 0, sizeof(darm_t));
    d->instr = I_INVLD;
    d->instr_type = T_INVLD;
    d->shift_type = S_INVLD;
    d->S = d->E = d->U = d->H = d->P = d->I = B_INVLD;
    d->R = d->T = d->W = d->M = d->N = d->B = B_INVLD;
    d->Rd = d->Rn = d->Rm = d->Ra = d->Rt = R_INVLD;
    d->Rt2 = d->RdHi = d->RdLo = d->Rs = R_INVLD;
    d->option = O_INVLD;

    switch (w >> 11) {
    case 0b11101: case 0b11110: case 0b11111:
        return -1;

    default:
        d->w = w;
        return thumb_disasm(d, w);
    }
}
