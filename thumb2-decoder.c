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
