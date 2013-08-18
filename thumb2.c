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
#include "thumb2-tbl.h"

#define BITMSK_8 ((1 << 8) - 1)

darm_instr_t thumb2_lookup_instr(uint16_t w, uint16_t w2);

static int thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2)
{
    d->instr = thumb2_lookup_instr(w, w2);
    d->instr_type = thumb_instr_types[w >> 8];



/*
    switch ((uint32_t) d->instr_type) {
	case T_THUMB_ONLY_IMM8:
        	d->I = B_SET;
        	d->imm = w & BITMSK_8;
        	return 0;
	case T_THUMB_GPI:
		printf("GPI\n");
        	d->instr = type_gpi_instr_lookup[(w >> 6) & b1111];
        	switch ((uint32_t) d->instr) {
        		case I_AND: case I_EOR: case I_LSL: case I_LSR:
        		case I_ASR: case I_ADC: case I_SBC: case I_ROR:
            			d->Rn = w & b111;
				d->Rm = w2 & b111;
				d->Rd = (w2 >> 8) & b111;
            			return 0;
		}
	default:
		return 0;

    }
*/
	return 0;
}



darm_instr_t thumb2_lookup_instr(uint16_t w, uint16_t w2) {
// TODO: replace with binary search tree for speedup
	uint32_t dw;
	int i;
	dw = (w << 16) | w2;
	for (i = 0 ; i < THUMB2_INSTRUCTION_COUNT ; i++) {
		printf("%x %x\n", thumb2_instruction_ids[i], thumb2_instruction_masks[i]);
		if ((dw & thumb2_instruction_masks[i]) == thumb2_instruction_ids[i]) {
			return thumb2_instr_labels[i];
		}
	}
	return 0;
}

// Parse the register instruction type
void thumb2_parse_reg(darm_t *d, uint16_t w, uint16_t w2) {


}
// Parse the immediate instruction type
void thumb2_parse_imm(darm_t *d, uint16_t w, uint16_t w2) {


}
// Parse the flag instruction type
void thumb2_parse_flag(darm_t *d, uint16_t w, uint16_t w2) {


}




int darm_thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2)
{
    (void)d; (void) w; (void) w2;

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

	    // Thumb2 32-bit
	    case b11101: case b11110: case b11111:
        	d->w = w | (w2 << 16);
        	thumb2_disasm(d, w, w2);
		return 2;

    	    // Thumb 16-bit unconditional branch
    	    case b11100:
    	    // Thumb 16 bit
    	    default:
        	d->w = w;
        	darm_thumb_disasm(d, w);
		return 1;
    }
}
