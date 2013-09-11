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
#include "thumb2.h"

#define BITMSK_8 ((1 << 8) - 1)
#define ROR(val, rotate) (((val) >> (rotate)) | ((val) << (32 - (rotate))))
#define SIGN_EXTEND32(v, len) ((v << (32 - len)) >> (32 - len))

void thumb2_parse_reg(darm_t *d, uint16_t w, uint16_t w2);
void thumb2_parse_imm(darm_t *d, uint16_t w, uint16_t w2);
void thumb2_parse_flag(darm_t *d, uint16_t w, uint16_t w2);
void thumb2_parse_misc(darm_t *d, uint16_t w, uint16_t w2);

// 12 -> 32 bit expansion function
// See manual for this
// We don't care about the carry for the moment (should we?)
uint32_t thumb_expand_imm(uint16_t imm12_r) {

	uint16_t imm12 = imm12_r & 0xFFF; // *snip*
	uint32_t imm32, unrotated;

	if ((imm12 & 0xC00) == 0) {
	    switch((imm12 & 0x300) >> 8) {
		case 0:
		    imm32 = (uint32_t) imm12 & 0xFF;
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
		unrotated = (0x80 | (imm12 & 0x7F));
		imm32 = ROR(unrotated, (imm12 & 0xF80) >> 7);
	}
	return imm32;
}


// Can replace this later on
void thumb2_decode_immshift(darm_t *d, uint8_t type, uint8_t imm5) 
{
	switch(type) {
	    case 0:
		d->shift_type = S_LSL;
		d->shift = imm5;
		break;
	    case 1:
		d->shift_type = S_LSR;
		d->shift = (imm5 == 0) ? 32 : imm5;
		break;
	    case 2:
		d->shift_type = S_ASR;
		d->shift = (imm5 == 0) ? 32 : imm5;
		break;
	    case 3:
		d->shift_type = S_ROR;
		d->shift = (imm5 == 0) ? 1 : imm5; // RRX! :)
		break;
	    default:
		d->shift_type = S_INVLD;
		break;
	}
}


// Parse the register instruction type
void thumb2_parse_reg(darm_t *d, uint16_t w, uint16_t w2) {

    switch(d->instr_type) {
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
void thumb2_parse_imm(darm_t *d, uint16_t w, uint16_t w2) {
    d->I = B_SET;

    switch(d->instr_imm_type) {
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
		break;
	case T_THUMB2_IMM2:
		// 2 bit immediate
		d->imm = (w2 >> 4) & b11;
		d->shift = d->imm;
		d->shift_type = S_LSL;
		break;
	case T_THUMB2_IMM2_IMM3:
		// 2 and 3 bit immediates
		// (imm3:imm2)
		d->imm = ((w2 >> 10) & b11100) | ((w2 >> 6) & b11);
		break;
	case T_THUMB2_IMM1_IMM3_IMM8:
		// 1, 3 and 8 bit immediates
		// i:imm3:imm8 -> imm12 -> imm32

		// if bits 9:8 == '10' then zero extend, otherwise thumb expand
		if ((w & 0x300) == 0x200)
		    d->imm = (uint32_t) ((w & 0x400) << 1) | ((w2 & 0x7000) >> 4) | (w2 & 0xFF);
		else
		    d->imm = thumb_expand_imm( ((w & 0x400) << 1) | ((w2 & 0x7000) >> 4) | (w2 & 0xFF));
		break;
	default:
		// Invalid.
		break;
    }

}
// Parse the flag instruction type
void thumb2_parse_flag(darm_t *d, uint16_t w, uint16_t w2) {

    switch(d->instr_flag_type) {
	case T_THUMB2_NO_FLAG:
	    // No flag
	    break;
	case T_THUMB2_ROTATE_FLAG:
	    // Rotate field
	    d->rotate = (w2 >> 1) & b11000;
	    break;
	case T_THUMB2_U_FLAG:
	    // U flag
	    d->U = (w >> 7) & 1 ? B_SET : B_UNSET;
	    break;
	case T_THUMB2_WUP_FLAG:
	    // W, U and P flags
	    d->W = (w2 >> 8) & 1 ? B_SET : B_UNSET;
	    d->U = (w2 >> 9) & 1 ? B_SET : B_UNSET;
	    d->P = (w2 >> 10) & 1 ? B_SET : B_UNSET;
	    break;
	case T_THUMB2_TYPE_FLAG:
	    // Type field
	    // This is always a T_THUMB2_IMM2_IMM3 type
	    thumb2_decode_immshift(d, (w2 >> 4) & 3, d->imm);
	    break;
	case T_THUMB2_REGLIST_FLAG:
	    // Reglist field
	    //d->reglist = w2 & 0x1FFF;
	    d->reglist = w2 & 0xFFFF;
	    break;
	case T_THUMB2_WP_REGLIST_FLAG:
	    // Reglist field and W, P flags
	    d->reglist = w2 & 0xFFFF;
	    d->W = (w >> 5) & 1 ? B_SET : B_UNSET;
	    //d->P = (w >> 15) & 1 ? B_SET : B_UNSET;
	    break;
	case T_THUMB2_S_FLAG:
	    // S flag
	    d->S = (w >> 4) & 1 ? B_SET : B_UNSET;
	    break;
	case T_THUMB2_S_TYPE_FLAG:
	    // S flag and type field
	    d->S = (w >> 4) & 1 ? B_SET : B_UNSET;
	    thumb2_decode_immshift(d, (w2 >> 4) & 3, d->imm);
	    break;
	default:
	    // Invalid.
	    break;
    }

}

// Parse misc instruction cases
void thumb2_parse_misc(darm_t *d, uint16_t w, uint16_t w2) {

    // Misc. cases
    switch(d->instr) {

	// Branch
        case I_B:
	        d->I = B_SET;
                d->S = (w >> 10) & 1 ? B_SET : B_UNSET;
                d->J1 = (w2 >> 13) & 1 ? B_SET : B_UNSET;
	        d->J2 = (w2 >> 11) & 1 ? B_SET : B_UNSET;
	        if ((w2 & 0x1000) == 0) {
		    // T3
		    // sign_extend(S:J2:J1:imm6:imm11:0, 32)
		    d->imm = SIGN_EXTEND32( ( ((w & 0x400) << 10) | ((w2 & 0x800) << 8) | ((w2 & 0x2000) << 5) | ((w & 0x3F) << 12) | ((w2 & 0x7FF) << 1)), 21 );
		    d->cond = (w >> 6) & b1111; // directly indexing the enum
	        } else {
		    // T4
		    // I1 = not(J1 xor S); I2 = not(J2 xor S); imm32 = sign_extend(S:I1:I2:imm10:imm11:0, 32)
		    d->imm = SIGN_EXTEND32( (((w & 0x400) << 14) | (((~(w2 >> 13) ^ (w >> 10)) & 1) << 23) | ((~((w2 >> 11) ^ (w >> 10)) & 1) << 22) | ((w & 0x3FF) << 12) | ((w2 & 0x7FF) << 1)), 25);
	        }
	    break;

	// Branch with Link
	case I_BL: case I_BLX:
	    d->I = B_SET;
            d->S = (w >> 10) & 1 ? B_SET : B_UNSET;
            d->J1 = (w2 >> 13) & 1 ? B_SET : B_UNSET;
	    d->J2 = (w2 >> 11) & 1 ? B_SET : B_UNSET;
	    if ((w2 & 0x1000) == 0) {
		// BLX
		// I1 = not(J1 xor S); I2 = not(J2 xor S); imm32 = sign_extend(S:I1:I2:imm10H:imm10L:00, 32)
		d->imm = SIGN_EXTEND32( (((w & 0x400) << 14) | ((~((w2 >> 13) ^ (w >> 10)) & 1) << 23) | ((~((w2 >> 11) ^ (w >> 10)) & 1) << 22) | ((w & 0x3FF) << 12) | ((w2 & 0x7FE) << 1)), 25);
		d->H = (w & 1) ? B_SET : B_UNSET;
	    } else {
		// BL
		// I1 = not(J1 xor S); I2 = not(J2 xor S); imm32 = sign_extend(S:I1:I2:imm10:imm11:0, 32)
		d->imm = SIGN_EXTEND32( (((w & 0x400) << 14) | (((~((w2 >> 13) ^ (w >> 10))) & 1) << 23) | ((~((w2 >> 11) ^ (w >> 10)) & 1) << 22) | ((w & 0x3FF) << 12) | ((w2 & 0x7FF) << 1)), 25);
	    }
	    break;

	// Bit field clear/insert
	case I_BFC: case I_BFI:
	    d->lsb = d->imm & 0x1F;
	    d->msb = w2 & 0x1F;
	    d->width = d->msb + 1 - d->lsb;
	    break;

	case I_LSL: case I_LSR: case I_ASR: case I_ROR:
	    if (d->I == B_SET) {
	        d->shift = d->imm;
	        d->shift_type = ((w2 >> 4) & b11);
	    }
	    break;

	// imm4 case
	case I_MOVW: case I_MOVT:
	    d->imm = (uint32_t) ((w & b1111) << 12) | ((w & 0x400) << 1) | ((w2 & 0x7000) >> 4) | (w2 & 0xFF);
	    break;

	// preserve PC in rd for further use(?)
	case I_CMP: case I_CMN: case I_TEQ: case I_TST:
	    d->Rd = PC;
	    break;

	// option field
	case I_DBG: case I_DMB: case I_DSB: case I_ISB:
	    d->option = w2 & b1111; // directly index enum
	    break;

	// co-proc data processing
	// TODO: not implemented
	//case I_CPD: case I_CPD2:
	//break;

	// co-proc load/store memory
	case I_LDC: case I_LDC2:
	case I_STC: case I_STC2:
	    d->P = (w >> 8) & 1 ? B_SET : B_UNSET;
	    d->U = (w >> 7) & 1 ? B_SET : B_UNSET;
	    d->D = (w >> 6) & 1 ? B_SET : B_UNSET;
	    d->W = (w >> 5) & 1 ? B_SET : B_UNSET;

	    // literal or immediate
	    d->Rn = ((w & b1111) == b1111) ? R_INVLD : (w & b1111);

	    d->I = B_SET;
	    d->imm = (uint32_t) ((w2 & 0xFF) << 2); // append '00' and zero extend
	    d->coproc = (w2 >> 8) & b1111;
	    d->CRd = (w2 >> 12) & b1111; // enum index
	    break;

	// Weird rd offset
	case I_STREX:
            d->Rd = ((w2 >> 8) & b1111);
	    d->imm = (uint32_t) ((w2 & 0xFF) << 2);
	    break;

	// zero-extend corner case with '00' appended
	case I_LDRD: case I_LDREX: case I_STRD:
	    d->imm = (uint32_t) ((w2 & 0xFF) << 2);
	    d->W = (w >> 5) & 1 ? B_SET : B_UNSET;
	    d->U = (w >> 7) & 1 ? B_SET : B_UNSET;
	    d->P = (w >> 8) & 1 ? B_SET : B_UNSET;
	    break;

	// Catch some pop/push inconsistencies
	case I_POP: case I_PUSH:
	    if (w == 0xF85D || w == 0xF84D) // no flags
		break;
	    if (w == 0xE8BD) // P flag
	    	d->P = (w2 >> 15) & 1 ? B_SET : B_UNSET; 
	    d->M = (w2 >> 14) & 1 ? B_SET : B_UNSET;
	    break;


	// co-processor move
	case I_MCR: case I_MCR2:
	case I_MRC: case I_MRC2:
	    d->CRm = (w2 & b1111);
	    d->CRn = (w & b1111);
	    d->coproc = (w2 >> 8) & b1111;
	    d->Rt = (w2 >> 12) & b1111;
	    d->opc1 = (w >> 5) & b111;
	    d->opc2 = (w2 >> 5) & b111;
	    break;

	// co-proc move 2 reg
	case I_MCRR: case I_MCRR2:
	case I_MRRC: case I_MRRC2:
	    d->coproc = (w2 >> 8) & b1111;
	    d->Rt = (w2 >> 12) & b1111;
	    d->opc1 = (w2 >> 4) & b1111;
	    d->CRm = (w2 & b1111);
	    d->Rt2 = w & b1111;
	    break;

	case I_MSR:
	    d->I = B_SET;
	    d->Rn = w & b1111;
	    d->mask = d->imm = (w2 >> 10) & b11;
	    break;

	case I_PKH:
	    // S flag and immediate already set
	    d->T = (w2 >> 4) & 1;
	    thumb2_decode_immshift(d, (w2 >> 4) & 2, d->imm);
	    break;

	case I_PLI:
	    d->Rt = R_INVLD;
	    d->P = B_INVLD;
	    d->W = B_INVLD;
	    if (d->Rn == b1111) {
		d->Rn = R_INVLD;
		d->imm = (w2 & 0xFFF);
		d->U = ((w >> 7) & 1) ? B_SET : B_UNSET;
	    }
	    break;

	case I_PLD:
    	    d->Rt = R_INVLD;
	    d->P = B_INVLD;
	    if (d->Rn == b1111) {
		d->Rn = R_INVLD;
		d->imm = (w2 & 0xFFF);
		d->shift_type = S_INVLD;
		d->shift = 0;
		d->Rm = R_INVLD;
		d->U = ((w >> 7) & 1) ? B_SET : B_UNSET;
	    }
            d->W = ((w & b1111) != b1111) ? ((w >> 5) & 1) : B_INVLD;
	    break;

	case I_SBFX: case I_UBFX:
	    d->lsb = d->imm;
	    d->width = (w2 & 0x1F) + 1;
	    break;

	// N, M flags
	case I_SMLABB: case I_SMLABT: case I_SMLATB: case I_SMLATT:
	case I_SMULBB: case I_SMULBT: case I_SMULTB: case I_SMULTT:
	    d->N = (w2 >> 5) & 1 ? B_SET : B_UNSET;
	case I_SMLAD: case I_SMLAW:
	case I_SMLSD: case I_SMUAD:
	case I_SMULW: case I_SMUSD:
	    d->M = (w2 >> 4) & 1 ? B_SET : B_UNSET;
	    if (d->Ra == b1111)
		d->Ra = R_INVLD;
	    break;

	// N, M, Rdhi, Rdlo flags
	case I_SMLALBB: case I_SMLALBT: case I_SMLALTB: case I_SMLALTT:
	    d->N = (w2 >> 5) & 1 ? B_SET : B_UNSET;
	case I_SMLSLD: case I_SMLALD:
	    d->M = (w2 >> 4) & 1 ? B_SET : B_UNSET;
	case I_SMLAL: case I_SMULL:
	case I_UMAAL: case I_UMLAL: case I_UMULL:
	    d->RdHi = (w2 >> 8) & b1111;
	    d->RdLo = (w2 >> 12) & b1111;
	    break;

	// R flag
	case I_SMMLA: case I_SMMLS: case I_SMMUL:
	    d->R = (w2 >> 4) & 1 ? B_SET : B_UNSET;
	    break;

	case I_SSAT: case I_USAT:
	    thumb2_decode_immshift(d, (w >> 4) & 2, d->imm);
	    d->sat_imm = w2 & 0x1F;
	    break;

	case I_SSAT16: case I_USAT16:
	    d->sat_imm = w2 & 0xF;
	    break;

	// WM flags
	case I_STM: case I_STMDB:
	    d->W = (w >> 5) & 1 ? B_SET : B_UNSET;
	    d->M = (w2 >> 14) & 1 ? B_SET : B_UNSET;
	    break;

	// H flag
	case I_TBB: case I_TBH:
	    d->H = (w2 >> 4) & 1 ? B_SET : B_UNSET;
	    break;

	default:
	    // Nothing happens
	    break;
    }


}


static int thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2)
{


    //printf("%i %i\n", T_THUMB2_RN_REG, d->instr_type);
    d->instr = thumb2_decode_instruction(d, w, w2);
    //printf("%i %i\n", T_THUMB2_RN_REG, d->instr_type);

    thumb2_parse_reg(d, w, w2);
    thumb2_parse_imm(d, w, w2);
    thumb2_parse_flag(d, w, w2);
    thumb2_parse_misc(d, w, w2);
    d->instr_type = I_INVLD;
    return 0;
}



// placeholder function for printing out thumb2 instructions
// This is here until the format string problem is resolved
// TODO: This lacks a lot of functionality, for debug only, replace with better function
char *darm_thumb2_str(darm_t *d) {

    int index=0, offset=0;
    static char stringbuf[512];

    for (int i = 0 ; i < THUMB2_INSTRUCTION_COUNT ; i++) {
	if (d->instr == thumb2_instr_labels[i]) {
	    index = i;
	    break;
	}
    }

    offset += sprintf(stringbuf+offset, "%s", thumb2_instruction_strings[index]);

    if (d->Rd != R_INVLD)
      offset += sprintf(stringbuf+offset, "rd%i,", d->Rd);

    if (d->Rt != R_INVLD)
      offset += sprintf(stringbuf+offset, "rt%i,", d->Rt2);
    if (d->Rt2 != R_INVLD)
      offset += sprintf(stringbuf+offset, "rt2%i,", d->Rt);

    if (d->Rn != R_INVLD) 
      offset += sprintf(stringbuf+offset, "rn%i,", d->Rn);

    if (d->Rm != R_INVLD)
      offset += sprintf(stringbuf+offset, "rm%i ", d->Rm);

    if (d->I == B_SET)
      offset += sprintf(stringbuf+offset, "#0x%x", d->imm);

    return stringbuf;
}


int darm_thumb2_disasm(darm_t *d, uint16_t w, uint16_t w2)
{
    (void) d; (void) w; (void) w2;

    memset(d, 0, sizeof(darm_t));
    // we set all conditional flags to "execute always" by default, as most
    // thumb instructions don't feature a conditional flag
    d->cond = C_AL;
    d->instr = I_INVLD;
    d->instr_type = T_INVLD;
    d->shift_type = S_INVLD;
    d->S = d->E = d->U = d->H = d->P = d->I = d->J1 = d->J2 = B_INVLD;
    d->R = d->T = d->W = d->M = d->N = d->B = d->D = B_INVLD;
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
