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

#ifndef __DARM_INTERNAL__
#define __DARM_INTERNAL__

typedef enum _darm_sm_opcode_t {
    // Halt execution, invalid instruction.
    SM_HLT,

    // Follow either branch of a node, depending on the particular bit
    // in the instruction.
    SM_STEP,

    // Some instructions are a more specific variant of another instruction.
    // In these cases, the more specific instruction will have a couple of
    // bits which are hardcoded and have to be checked in order to determine
    // as which encoding we will disassemble this instruction.
    SM_CMP4,

    // Takes a 5-bit value and looks it up in a lookup table.
    SM_TBL5,

    // This instruction has been disassembled correctly, return success.
    SM_RETN,

    // Assign the instruction index.
    SM_INSTR,

    // Extracts a couple of bits from the instruction, optionally adds a
    // value to the extracted bits or left shifts the value, and stores them
    // or sets the bits in the given field in the darm_t object.
    SM_EXTR, SM_EXTR2, SM_EXTR3,

    // Extracts a Floating Point register, which consists of five bytes.
    SM_FPREG,

    // Extract an immediate. In addition to the bitsize, the extended version
    // also provides the bit index in the original encoding, and the bit index
    // in the resulting immediate.
    SM_IMM, SM_IMM2,

    // Sign extend the immediate.
    SM_SIGN,

    // Special not-xor for various bits of branch offsets.
    SM_BNXOR,

    // Extra information for generating a human-readable string.
    SM_STR,

    SM_ARMExpandImm,
} darm_sm_opcode_t;

typedef enum _darm_string_opcode_t {
    STR_RETN, STR_S, STR_cond, STR_REG, STR_REG_CONST, STR_SHIFT, STR_IMM,
    STR_INT, STR_REGLIST, STR_OPTION, STR_LABEL, STR_EXCL, STR_COPROC,
} darm_string_opcode_t;

#endif
