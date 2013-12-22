"""
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
"""

import sys
import textwrap
from tablegen import Instruction, BitPattern, Macro, Table
from tablegen import Field, Register, Immediate


Rd = Register(4, 'Rd')
Rn = Register(4, 'Rn')
Rm = Register(4, 'Rm')
Ra = Register(4, 'Ra')
Rt = Register(4, 'Rt')
Rt2 = Register(4, 'Rt2')
RdHi = Register(4, 'RdHi')
RdLo = Register(4, 'RdLo')
Rs = Register(4, 'Rs')
typ = BitPattern(2, 'type')

# TODO cond cannot be 0b1111
# cond = BitPattern(4, 'cond')
cond = Field(4, 'cond')
S = Field(1, 'S')
W = Field(1, 'W')
P = Field(1, 'P')
U = Field(1, 'U')

msb = Field(5, 'msb')
lsb = Field(5, 'msb')
option = Field(4, 'option')
register_list = Field(16, 'register_list')
widthm1 = Field(5, 'widthm1')
E = Field(1, 'E')

imm1 = Immediate(1, 'imm1')
imm4 = Immediate(4, 'imm4')
imm4L = Immediate(4, 'imm4L')
imm4H = Immediate(4, 'imm4H')
imm5 = Immediate(5, 'imm5')
imm8 = Immediate(8, 'imm8')
imm12 = Immediate(12, 'imm12')
imm24 = Immediate(24, 'imm24')

ARMExpandImm = Macro('ARMExpandImm')


table = [
    Instruction('ADC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 0, 1, S, Rn, Rd, imm12)),
    Instruction('ADC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ADC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ADD{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 0, 0, S, Rn, Rd, imm12)),
    Instruction('ADD{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ADD{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ADD{S}<c> <Rd>,SP,#<const>', (cond, 0, 0, 1, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm12)),
    Instruction('ADD{S}<c> <Rd>,SP,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm5, typ, 0, Rm)),
    Instruction('ADR<c> <Rd>,<label>', (cond, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, imm12)),
    Instruction('ADR<c> <Rd>,<label>', (cond, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, Rd, imm12)),
    Instruction('AND{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 0, 0, S, Rn, Rd, imm12)),
    Instruction('AND{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('AND{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ASR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 1, 0, 0, Rm)),
    Instruction('ASR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 1, 0, 1, Rn)),
    Instruction('B<c> <label>', (cond, 1, 0, 1, 0, imm24)),
    Instruction('BFC<c> <Rd>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, 1, 1, 1, 1)),
    Instruction('BFI<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, Rn)),
    Instruction('BIC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 1, 1, 1, 0, S, Rn, Rd, imm12)),
    Instruction('BIC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('BIC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('BKPT #<imm16>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, imm12, 0, 1, 1, 1, imm4)),
    Instruction('BL<c> <label>', (cond, 1, 0, 1, 1, imm24)),
    Instruction('BLX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('BX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('BXJ<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 0, Rm)),
    Instruction('CLREX', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('CLZ<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('CMN<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('CMN<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('CMN<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('CMP<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('CMP<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('CMP<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('DBG<c> #<option>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 1, 1, 1, 1, option)),
    Instruction('DMB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 1, option)),
    Instruction('DSB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 0, option)),
    Instruction('EOR{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 0, 1, S, Rn, Rd, imm12)),
    Instruction('EOR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('EOR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ISB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 1, 0, option)),
    Instruction('LDREX<c> <Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXB<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXD<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXH<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDRHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('LDRHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 1, 0, 1, imm4L)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 1, 1, 1, imm4L)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('LDRT<c> <Rt>, [<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 0, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRT<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, 0, U, 0, 1, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 0, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 0, 1, Rn)),
    Instruction('LSR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 1, 0, Rm)),
    Instruction('LSR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 1, 1, Rn)),
    Instruction('MLA{S}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MLS<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 1, 1, 0, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MOV{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm12)),
    Instruction('MOVW<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 0, 0, 0, imm4, Rd, imm12)),
    Instruction('MOV{S}<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 0, 0, 0, Rm)),
    Instruction('MOVT<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 1, 0, 0, imm4, Rd, imm12)),
    Instruction('MRS<c> <Rd>,<spec_reg>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('MUL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rd, (0), (0), (0), (0), Rm, 1, 0, 0, 1, Rn)),
    Instruction('MVN{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm12)),
    Instruction('MVN{S}<c> <Rd>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm5, typ, 0, Rm)),
    Instruction('MVN{S}<c> <Rd>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, Rs, 0, typ, 1, Rm)),
    Instruction('NOP<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 0)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 0, S, Rn, Rd, imm12)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('POP<c> <registers>', (cond, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, register_list)),
    Instruction('POP<c> <registers>', (cond, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, Rt, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('PUSH<c> <registers>', (cond, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, register_list)),
    Instruction('PUSH<c> <registers>', (cond, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, Rt, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('QADD<c> <Rd>,<Rm>,<Rn>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm)),
    Instruction('QADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('QADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('QASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('QDADD<c> <Rd>,<Rm>,<Rn>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm)),
    Instruction('QDSUB<c> <Rd>,<Rm>,<Rn>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm)),
    Instruction('QSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('QSUB<c> <Rd>,<Rm>,<Rn>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm)),
    Instruction('QSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('QSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('RBIT<c> <Rd>,<Rm>', (cond, 0, 1, 1, 0, 1, 1, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('REV<c> <Rd>,<Rm>', (cond, 0, 1, 1, 0, 1, 0, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('REV16<c> <Rd>,<Rm>', (cond, 0, 1, 1, 0, 1, 0, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm)),
    Instruction('REVSH<c> <Rd>,<Rm>', (cond, 0, 1, 1, 0, 1, 1, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm)),
    Instruction('ROR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 1, 1, 0, Rm)),
    Instruction('ROR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 1, 1, 1, Rn)),
    Instruction('RRX{S}<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 1, 1, 0, Rm)),
    Instruction('RSB{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 1, 1, S, Rn, Rd, imm12)),
    Instruction('RSB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('RSB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('RSC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 1, 1, S, Rn, Rd, imm12)),
    Instruction('RSC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('RSC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('SADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('SADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('SASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('SBC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 1, 0, S, Rn, Rd, imm12)),
    Instruction('SBC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 1, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('SBC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 1, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    # Instruction('SBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 0, 1, widthm1, Rd, lsb, 1, 0, 1, Rn)),
    Instruction('SEL<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm)),
    Instruction('SETEND <endian_specifier>', (1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, (0), (0), (0), 1, (0), (0), (0), (0), (0), (0), E, (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('SEV<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('SHADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('SHADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('SHASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('SHSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('SHSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('SHSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('SMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('SSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('SSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('STREX<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXB<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXD<c> <Rd>,<Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXH<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STRH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('STRH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('STRT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 0, 1, 0, Rn, Rt, imm12)),
    Instruction('STRT<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, 0, U, 0, 1, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('SUB{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 1, 0, S, Rn, Rd, imm12)),
    Instruction('SUB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('SUB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('SUB{S}<c> <Rd>,SP,#<const>', (cond, 0, 0, 1, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm12)),
    Instruction('SUB{S}<c> <Rd>,SP,<Rm>{,<shift>}', (cond, 0, 0, 0, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm5, typ, 0, Rm)),
    Instruction('SVC<c> #<imm24>', (cond, 1, 1, 1, 1, imm24)),
    Instruction('SWP<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2)),
    Instruction('SWPB<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 0, 1, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2)),
    Instruction('TEQ<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('TEQ<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TEQ<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('TST<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('TST<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TST<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('UADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('UADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('UASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    # Instruction('UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 1, widthm1, Rd, lsb, 1, 0, 1, Rn)),
    Instruction('UDF<c> #<imm12>', (cond, 0, 1, 1, 1, 1, 1, 1, 1, imm12, 1, 1, 1, 1, imm4)),
    Instruction('UHADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('UHADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('UHASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('UHSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('UHSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('UHSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('UMAAL<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('UMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 0, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('UMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 0, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('UQADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('UQADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('UQASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('UQSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('UQSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('UQSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('USAD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 1, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, 0, 1, Rn)),
    Instruction('USADA8<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 1, 0, 0, 0, Rd, Ra, Rm, 0, 0, 0, 1, Rn)),
    Instruction('USAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('USUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('USUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('WFE<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 0)),
    Instruction('WFI<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 1)),
    Instruction('YIELD<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 1)),
    Instruction('SMC<c> #<imm4>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), 0, 1, 1, 1, imm4)),
]

def generate_c_table(l, bitsize):
    return '\n    '.join(textwrap.wrap(', '.join('%s' % _ for _ in l), 74))


def magic_open(fname):
    sys.stdout = open(fname, 'w')
    print('/*')
    print(__doc__.strip())
    print('*/')


if __name__ == '__main__':
    t = Table(table, 32)
    sm, lut = t.create()

    magic_open('darm-tables.c')
    print('#include <stdint.h>')
    print('#include "darm.h"')
    print('#include "darm-instr.h"')
    print('#define offsetof(st, m) ((uint32_t)(&((st *)0)->m))')
    print('#define O(m) offsetof(darm_t, m)')
    print('const uint8_t g_armv7_sm[%d] = {' % len(sm.table))
    print('    ' + generate_c_table(sm.table, 8))
    print('};')
    print('const uint16_t g_armv7_lut[%d] = {' % len(lut.table))
    print('    ' + generate_c_table(lut.table, 16))
    print('};')

    magic_open('darm-tables.h')
    print('#ifndef __DARM_TABLES__')
    print('#define __DARM_TABLES__')
    print('#include <stdint.h>')
    print('extern const uint8_t g_armv7_sm[%d];' % len(sm.table))
    print('extern const uint16_t g_armv7_lut[%d];' % len(lut.table))
    print('#endif')

    magic_open('darm-instr.h')
    print('#ifndef __DARM_INSTR__')
    print('#define __DARM_INSTR__')
    print('typedef enum _darm_instr_t {')
    l = ['INVLD'] + sorted(set(_.name.upper() for _ in table)) + ['INSTRCNT']
    lines = textwrap.wrap(', '.join('I_%s' % _ for _ in l), 74)
    print('    ' + '\n    '.join(lines))
    print('} darm_instr_t;')
    print('#endif')
