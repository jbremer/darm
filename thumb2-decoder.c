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


void thumb2_decode_instruction(darm_t *d, uint16_t w, uint16_t w2) {

    static int op2;
    op2 = (w >> 4) & 0x7F;

    switch((w>>11)&b11) {

        case 1:
            op2 &= 0x64;
            if (op2 == 0) {
                // load, store multiple
            } else if (op2 == b100) {
                // load/store dual, load/store exclusive, table branch
            } else if (((op2 >> 5) & b11) == b1) {
	        // dataprocessing (shifted register)
            } else if (((op2 >> 6) & b1) == b1) {
                // coproc, simd, fpu
            }
            break;

        case 2:
	    op2 = (op2 & 0x20) >> 5;
            if (op2 == 0 && (w2&0x8000) == 0) {
                // dataprocessing (modified immediate)
            } else if (op2 == 1 && (w2&0x8000) == 0) {
                // dataprocessing (plain binary immediate)
            } else if ((w2 & 0x8000) == 0x8000) {
                // branches and miscellaneous control
	    }
            break;

        case 3:

	    if ((op2 & 0x71) == 0) {
                // store single data item
	    } else if ((op2 & 0x71) == 0x10) {
		// Advanced SIMD element or structure load/store instructions
	    } else if ((op2 & 0x70) == 0x20) {
		// data-processing (register)
	    } else if ((op2 & 0x78) == 0x30) {
		// multiply, multiply accumulate, and absolute difference
            } else if ((op2 & 0x78) == 0x38) {
		// long multiply, long multiply accumulate, and divide
	    } else {

	        switch (op2 & 0x67) {
		    case 1:
			// load byte, memory hints
		    case 3:
			// load halfword, memory hints
                    case 5:
			// load word
                    case 7:
                    default:
                        // undefined
			break;
	        }
	    }
            break;
	default:
	    break;
    }

}


darm_instr_t thumb2_load_store_multiple(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op, L, W_Rn;
    op = (w >> 7) & b11;
    L = (w >> 4) & b1;
    W_Rn = ((w >> 1) & 0x10) | (w & b1111);
    switch (op) {
	case 0:
	case 3:
	    if (L == 0)
		return I_SRS;
	    else if (L == 1)
		return I_RFE;
	    break;
	case 1:
	    if (L == 0)
		return I_STM;
	    else if (L == 1) {
		if (W_Rn == 0x1D)
		    return I_POP;
		else
		    return I_LDM;
	    }
	    break;
	case 2:
	    if (L == 0) {
		if (W_Rn == 0x1D)
		    return I_PUSH;
		else
		    return I_STMDB;
	    } else if (L == 1)
		return I_LDMDB;
            break;
    }

    return I_INVLD;
}

darm_instr_t thumb2_load_store_dual(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op1, op2, op3, Rn;
    op1 = (w >> 7) & b11;
    op2 = (w >> 4) & b11;
    op3 = (w2 >> 4) & b1111;
    Rn = w & b1111;

    if (op1 == 0 && op2 == 0)
        return I_STREX;
    else if (op1 == 0 && op2 == 1)
        return I_LDREX;
    else if ((op1&2) == 0 && op2 == 2)
        return I_STRD;  // immediate
    else if ((op1&2) == 0 && op2 == 3)
        return I_LDRD;	// literal and immediate
    else if ((op1&2) == 2 && (op2&1) == 0)
	return I_STRD;	// immediate
    else if ((op1&2) == 2 && (op2&1) == 1)
	return I_LDRD; // literal and immediate
    else if (op1 == 1 && op2 == 0) {
	switch(op3) {
	    case 4:
		return I_STREXB;
	    case 5:
		return I_STREXH;
	    case 7:
		return I_STREXD;
        }
    } else if (op1 == 1 && op2 == 1) {
	switch(op3) {
	    case 0:
		return I_TBB;
	    case 1:
		return I_TBH;
	    case 4:
		return I_LDREXB;
	    case 5:
		return I_LDREXH;
	    case 7:
		return I_LDREXD;
	}
    }
}

darm_instr_t thumb2_move_shift(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t type, imm3_imm2;
    type = (w2>>4) & b11;
    imm3_imm2 = ((w2>>10) & 0x1C) | ((w2>>6) & b11);

    switch(type) {
	case 0:
	    if (imm3_imm2 == 0)
		return I_MOV;
	    else
		return I_LSL;
	case 1:
	    return I_LSR;
	case 2:
	    return I_ASR;
	case 3:
	    if (imm3_imm2 == 0)
		return I_RRX;
	    else
		return I_ROR;
    }

}

darm_instr_t thumb2_data_shifted_reg(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op, Rn, Rd_S;
    op = (w >> 5) & b1111;
    Rn = w & b1111;
    Rd_S = ((w2 >> 7) & 0x1E) | (w >> 4);

    // These all operate on registers

    switch(op) {
	case 0:
	    if (Rd_S == 0x1F)
		return I_TST;
	    else
		return I_AND;
	case 1:
	    return I_BIC;
	case 2:
	    if (Rn == 0x1F)
		return thumb2_move_shift(d, w, w2);
	    else
		return I_ORR;
	case 3:
	    if (Rn == 0x1F)
		return I_MVN;
	    else
		return I_ORN;
	case 4:
	    if (Rd_S == 0x1F)
		return I_TEQ;
	    else
		return I_EOR;
	case 6:
	    return I_PKH;
	case 8:
	    if (Rd_S == 0x1F)
		return I_CMN;
	    else
		return I_ADD;
	case 10:
	    return I_ADC;
	case 11:
	    return I_SBC;
	case 13:
	    if (Rd_S == 0x1F)
		return I_CMP;
	    else
		return I_SUB;
	case 14:
	    return I_RSB;
    }

}

darm_instr_t thumb2_modified_immediate(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op, Rn, Rd_S;
    op = (w >> 5) & b1111;
    Rn = w & b1111;
    Rd_S = ((w2 >> 7) & 0x1E) | (w >> 4);

    // These all operate on immediates
    switch(op) {
	case 0:
	    if (Rd_S == 0x1F)
		return I_TST;
	    else
		return I_AND;
	case 1:
	    return I_BIC;
	case 2:
	    if (Rn == 0x1F)
		return I_MOV;
	    else
		return I_ORR;
	case 3:
	    if (Rn == 0x1F)
		return I_MVN;
	    else
		return I_ORN;
	case 4:
	    if (Rd_S == 0x1F)
		return I_TEQ;
	    else
		return I_EOR;
	case 8:
	    if (Rd_S == 0x1F)
		return I_CMN;
	    else
		return I_ADD;
	case 10:
	    return I_ADC;
	case 11:
	    return I_SBC;
	case 13:
	    if (Rd_S == 0x1F)
		return I_CMP;
	    else
		return I_SUB;
	case 14:
	    return I_RSB;
    }
}

darm_instr_t thumb2_plain_immediate(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op, Rn;
    op = (w >> 4) & 0x1F;
    Rn = w & b1111;

    // Immediate stuff

    switch(op) {
	case 0:
	    if (Rn == b1111)
		return I_ADR;
	    else
		return I_ADDW;
	case 4:
	    return I_MOVW;
	case 10:
	    if (Rn = b1111)
		return I_ADR;
	    else
		return I_SUB;
	case 12:
	    return I_MOVT;
	case 16:
	    return I_SSAT;
	case 18:
	    if ((w2 & 0x70C0) == 0)
		return I_SSAT16;
	    else
		return I_SSAT;
	case 20:
	    return I_SBFX;
	case 22:
	    if (Rn == b1111)
		return I_BFC;
	    else
		return I_BFI;
	case 24:
	    return I_USAT;
	case 26:
	    if ((w2 & 0x70C0) == 0)
		return I_SSAT16;
	    else
		return I_SSAT;
	case 28:
	    return I_UBFX;

    }


}

darm_instr_t thumb2_proc_state(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op1, op2;
    op1 = (w2 >> 8) & b111;
    op2 = w2 & 0xFF;

    if (op1 == 0) {
        switch(op2) {
	    case 0:
		return I_NOP;
	    case 1:
		return I_YIELD;
	    case 2:
		return I_WFE;
	    case 3:
		return I_WFI;
	    case 4:
		return I_SEV;
	    default:
		if (((op2 >> 4) & b1111) == b1111)
		    return I_DBG;
        }

    } else
        return I_CPS;
}

darm_instr_t thumb2_misc_ctrl(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op;
    op = (w2 >> 4) & b111;
    switch(op) {
        case 0:
	case 1:
	    // TODO: ENTERX, LEAVEX
	    return I_NOP;
	case 2:
	    return I_CLREX;
	case 4:
	    return I_DSB;
	case 5:
	    return I_DMB;
	case 6:
	    return I_ISB;
    }
    return I_INVLD;
}


darm_instr_t thumb2_branch_misc_ctrl(darm_t *d, uint16_t w, uint16_t w2) {
    static uint8_t op, op1, op2, imm8;
    op = (w >> 4) & 0x7F;
    op1 = (w2 >> 12) & b111;
    op2 = (w2 >> 8) & b1111;
    imm8 = w2 & 0xFF;
    if (op1 == 0 && op == 0xFE)
	//TODO: return I_HVC;
	return I_NOP;
    else if (op1 == 0 && op == 0xFF)
	return I_SMC;
    else if ((op1&b101) == 1)
	return I_B;
    else if (op1 == 2 && op == 0xFF)
	return I_UDF;
    else if ((op1&b101) == 0) {
	if ((op & 0x38) != 0x38)
	    return I_B;
	else if ((op & 0x7E) == 0x38 && (imm8 & 0x10) == 0x10)
	    return I_MSR;	// banked register
	else if (op == 0x38 && (imm8 & 0x10) == 0)
	    return I_MSR;	// register 
	else if (op == 0x3A)
	    return thumb2_proc_state(d, w, w2);
	else if (op == 0x3B)
	    return thumb2_misc_ctrl(d, w, w2);
	else if (op == 0x3C)
	    return I_BXJ;
	else if (op == 0x3E && (imm8 & 0x10) == 0)
	    return I_MRS;

    }
}


darm_instr_t thumb2_coproc_simd(darm_t *d, uint16_t w, uint16_t w2) {

    /* TODO: implement */
    return -1;

}
