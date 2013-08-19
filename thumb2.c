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
#define ROR(val, rotate) (((val) >> (rotate)) | ((val) << (32 - (rotate))))


int thumb2_lookup_instr(uint16_t w, uint16_t w2);
void thumb2_parse_reg(int index, darm_t *d, uint16_t w, uint16_t w2);
void thumb2_parse_imm(int index, darm_t *d, uint16_t w, uint16_t w2);


// 12 -> 32 bit expansion function
// See manual for this, A6-233
// We don't care about the carry for the moment (should we?)
static uint32_t thumb_expand_imm(uint16_t imm12_r) {

	uint16_t imm12 = imm12_r & 0xFFF; // *snip*
	uint32_t imm32, unrotated;

	if ((imm12 & 0xC00) == 0) {
	    switch((imm12 & 0x300) >> 8) {
		case 0:
		    imm32 = ((imm12 & 0xFF) << 24);
		    break;
		case 1:
		    imm32 = ((imm12 & 0xFF) << 16) | (imm12 & 0xFF);
		    break;
		case 2:
		    imm32 = ((imm12 & 0xFF) << 24) | ((imm12 & 0xFF) << 8);
		    break;
		case 3:
		    imm32 = ((imm12 & 0xFF) << 24) | ((imm12 & 0xFF) << 16) | ((imm12 & 0xFF) << 8) | (imm12 & 0xFF); // Must be a more ninja way to do this
	    }
	} else {

		unrotated = (0x80 | (imm12 & 0x7F)) << 24;
		imm32 = ROR(unrotated, imm12 & 0xFC);
	}
	return imm32;
}


static int thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2)
{

    int index;
    index = thumb2_lookup_instr(w, w2);
    d->instr = thumb2_instr_labels[index];

    thumb2_parse_reg(index, d, w, w2);
    thumb2_parse_imm(index, d, w, w2);

    switch(thumb2_flags_instr_types[index]) {
	case T_THUMB2_NO_FLAG:
	    // No flag
	    break;
	case T_THUMB2_ROTATE_FLAG:
	    // Rotate field
	    d->rotate = (w2 & b110000) >> 4;
	    break;
	case T_THUMB2_U_FLAG:
	    // U flag
	    d->U = (w >> 7) & 1 ? B_SET : B_UNSET;
	    break;
	case T_THUMB2_WUP_FLAG:
	    // W, U and P flags
	    d->W = (w2 >> 8) & 1 ? B_SET : B_UNSET;
	    d->U = (w2 >> 9) & 1 ? B_SET : B_UNSET;
	    d->U = (w2 >> 10) & 1 ? B_SET : B_UNSET;
	case T_THUMB2_TYPE_FLAG:
	    // Type field
	    break;
	case T_THUMB2_REGLIST_FLAG:
	    // Reglist field
	    break;
	case T_THUMB2_WP_REGLIST_FLAG:
	    // Reglist field and W, P flags
	    break;
	case T_THUMB2_S_FLAG:
	    // S flag
	    break;
	case T_THUMB2_S_TYPE_FLAG:
	    // S flag and type field
	    break;
	default:
	    // Invalid.
	    break;
    }
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



int thumb2_lookup_instr(uint16_t w, uint16_t w2) {
// TODO: replace with binary search tree for speedup
	uint32_t dw;
	int i;
	dw = (w << 16) | w2;
	for (i = 0 ; i < THUMB2_INSTRUCTION_COUNT ; i++) {
		printf("%x %x\n", thumb2_instruction_ids[i], thumb2_instruction_masks[i]);
		if ((dw & thumb2_instruction_masks[i]) == thumb2_instruction_ids[i]) {
			return i;
		}
	}
	return 0;
}

// Parse the register instruction type
void thumb2_parse_reg(int index, darm_t *d, uint16_t w, uint16_t w2) {

    switch(thumb2_instr_types[index]) {
	case T_THUMB2_NO_REG:
		// No registers
		break;
	case T_THUMB2_RT_REG:
		// Rt register 
		d->Rt = (w2 >> 12) & b1111;
		break;
	case T_THUMB2_RT_RT2_REG:
		// Rt & Rt2 register
		d->Rt = (w2 >> 12) & b1111;
		d->Rt2 = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RM_REG:
		// Rm register
		d->Rm = (w & b1111);
		break;
	case T_THUMB2_RD_REG:
		// Rd register
		d->Rd = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RD_RM_REG:
		// Rd & Rm register
		d->Rd = (w2 >> 8) & b1111;
		d->Rm = w2 & b1111;
		break;
	case T_THUMB2_RN_REG:
		// Rn register
		d->Rn = w & b1111;
		break;
	case T_THUMB2_RN_RT_REG:
		// Rn & Rt register
		d->Rn = w & b1111;
		d->Rt = (w2 >> 12) & b1111;
		break;
	case T_THUMB2_RN_RT_RT2_REG:
		// Rn & Rt & Rt2 register
		d->Rn = w & b1111;
		d->Rt = (w2 >> 12) & b1111;
		d->Rt2 = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RN_RM_REG:
		// Rn & Rm register
		d->Rn = w & b1111;
		d->Rm = w2 & b1111;
		break;
	case T_THUMB2_RN_RM_RT_REG:
		// Rn & Rm & Rt register
		d->Rn = w & b1111;
		d->Rm = w2 & b1111;
		d->Rt = (w2 >> 12) & b1111;
		break;
	case T_THUMB2_RN_RD_REG:
		// Rn & Rd register
		d->Rn = w & b1111;
		d->Rd = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RN_RD_RT_REG:
		// Rn & Rd & Rt register
		d->Rn = (w & b1111);
		d->Rd = (w2 & b1111);
		d->Rt = (w2 >> 12) & b1111;
		break;
	case T_THUMB2_RN_RD_RT_RT2_REG:
		// Rn & Rd & Rt & Rt2 register
		d->Rn = (w & b1111);
		d->Rd = (w2 & b1111);
		d->Rt = (w2 >> 12) & b1111;
		d->Rt2 = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RN_RD_RM_REG:
		// Rn & Rd & Rm register
		d->Rn = w & b1111;
		d->Rm = w2 & b1111;
		d->Rd = (w2 >> 8) & b1111;
		break;
	case T_THUMB2_RN_RD_RM_RA_REG:
		// Rn & Rd & Rm & Ra register
		d->Rn = w & b1111;
		d->Rm = w2 & b1111;
		d->Rd = (w2 >> 8) & b1111;
		d->Ra = (w2 >> 12) & b1111;
		break;
	default:
		// Invalid yo.
		break;
    }

}
// Parse the immediate instruction type
void thumb2_parse_imm(int index, darm_t *d, uint16_t w, uint16_t w2) {
    d->I = B_SET;

    switch(thumb2_imm_instr_types[index]) {
	case T_THUMB2_NO_IMM:
		// No immediate
		d->I = B_UNSET;
		break;
	case T_THUMB2_IMM12:
		// 12 bit immediate
		d->imm = w2 & 0xFFF;
		break;
	case T_THUMB2_IMM8:
		// 8 bit immediate
		d->imm = w2 & 0xFF;
		// TODO: check which ones want zero extend to MSB!!
		break;
	case T_THUMB2_IMM2:
		// 2 bit immediate
		d->imm = (w2 >> 4) & b11;
		break;
	case T_THUMB2_IMM2_IMM3:
		// 2 and 3 bit immediates
		// (imm3:imm2)
		d->imm = ((w2 >> 10) & b11100) | ((w2 >> 6) & b11);
		break;
	case T_THUMB2_IMM1_IMM3_IMM8:
		// 1, 3 and 8 bit immediates
		// i:imm3:imm8 -> imm12 -> imm32
		d->imm = thumb_expand_imm( ((w & 0x400) << 1) | ((w2 & 0x7000) >> 4) | (w2 & 0xFF));
		break;
	default:
		// Invalid.
		break;
    }

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
