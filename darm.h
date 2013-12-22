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

#ifndef __DARM__
#define __DARM__

#define B_UNSET 0
#define B_SET   1
#define B_INVLD 2

typedef enum _darm_reg_t {
    R_INVLD,
} darm_reg_t;

typedef enum _darm_cond_t {
    C_INVLD,
} darm_cond_t;

typedef enum _darm_shift_type_t {
    S_INVLD,
} darm_shift_type_t;

typedef enum _darm_option_t {
    O_INVLD,
} darm_option_t;

typedef struct _darm_t {
    uint32_t        insn;

    darm_reg_t      Rd;
    darm_reg_t      Rn;
    darm_reg_t      Rm;
    darm_reg_t      Ra;
    darm_reg_t      Rt;
    darm_reg_t      Rt2;
    darm_reg_t      RdHi;
    darm_reg_t      RdLo;
    darm_reg_t      Rs;

    uint32_t        S;
    uint32_t        imm;
} darm_t;

#endif
