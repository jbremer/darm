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
            } else if (op2 == 1) {
		if ((w2 & 0x8000) == 0) {
                    // dataprocessing (plain binary immediate)
		} else if ((w2 & 0x8000) == 0x8000) {
                    // dranches and miscellaneous control
		}
            }
            break;

        case 3:
            break;
    }

} 

