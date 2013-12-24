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

from tablegen import Instruction, Macro, Table, Node
from tablegen import Field, CoprocessorRegister, Register, Immediate


class ARMv7Table(Table):
    def _init(self):
        self.cond = Node()
        self.uncond = Node()
        return self.cond, self.uncond

    def _insert(self, ins):
        if ins.bits[0] == cond:
            self.cond.insert(ins)
        else:
            self.uncond.insert(ins)

    def _process(self):
        self.cond.process()
        self.uncond.process()

    def _create(self, sm, lut, bitsize):
        off = sm.alloc(5)
        off2 = lut.alloc(2)

        sm.update(off, 'SM_CMP4', bitsize-4, 0b1111,
                  'L(%d)' % off2, 'H(%d)' % off2)

        cond = self.cond.create(sm, lut, bitsize)
        uncond = self.uncond.create(sm, lut, bitsize)
        lut.update(off2, cond, uncond)


Rd = Register(4, 'Rd')
Rn = Register(4, 'Rn')
Rm = Register(4, 'Rm')
Ra = Register(4, 'Ra')
Rt = Register(4, 'Rt')
Rt2 = Register(4, 'Rt2')
RdHi = Register(4, 'RdHi')
RdLo = Register(4, 'RdLo')
Rs = Register(4, 'Rs')
typ = Field(2, 'type')

cond = Field(4, 'cond')
S = Field(1, 'S')
W = Field(1, 'W')
P = Field(1, 'P')
U = Field(1, 'U')
D = Field(1, 'D')
R = Field(1, 'R')
M = Field(1, 'M')
N = Field(1, 'N')
tb = Field(1, 'tb')
sh = Field(1, 'sh')

msb = Field(5, 'msb')
lsb = Field(5, 'msb')
option = Field(4, 'option')
register_list = Field(16, 'register_list')
widthm1 = Field(5, 'widthm1')
E = Field(1, 'E')

opc1 = Field(4, 'opc1')
opc1_3 = Field(3, 'opc1')
opc2 = Field(3, 'opc2')
CRd = CoprocessorRegister(4, 'CRd')
CRn = CoprocessorRegister(4, 'CRn')
CRm = CoprocessorRegister(4, 'CRm')
coproc = Field(4, 'coproc')
mask = Field(2, 'mask')

imm1 = Immediate(1, 'imm1')
imm4 = Immediate(4, 'imm4')
imm4L = Immediate(4, 'imm4L')
imm4H = Immediate(4, 'imm4H')
imm5 = Immediate(5, 'imm5')
imm8 = Immediate(8, 'imm8')
imm12 = Immediate(12, 'imm12')
imm24 = Immediate(24, 'imm24')

sat_imm5 = Immediate(5, 'sat_imm')
sat_imm4 = Immediate(4, 'sat_imm')
rotate = Field(2, 'rotate')

ARMExpandImm = Macro('ARMExpandImm')


_table = [
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
    Instruction('BLX <label>', (1, 1, 1, 1, 1, 0, 1, imm1, imm24)),
    Instruction('BLX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('BX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('BXJ<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 0, Rm)),
    Instruction('CDP<c> <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>', (cond, 1, 1, 1, 0, opc1, CRn, CRd, coproc, opc2, 0, CRm)),
    Instruction('CDP2<c> <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>', (1, 1, 1, 1, 1, 1, 1, 0, opc1, CRn, CRd, coproc, opc2, 0, CRm)),
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
    Instruction('LDC{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, D, W, 1, Rn, CRd, coproc, imm8)),
    Instruction('LDC2{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, D, W, 1, Rn, CRd, coproc, imm8)),
    Instruction('LDM<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list)),
    Instruction('LDMDA<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 0, 0, W, 1, Rn, register_list)),
    Instruction('LDMDB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 0, 0, W, 1, Rn, register_list)),
    Instruction('LDMIB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 1, 0, W, 1, Rn, register_list)),
    Instruction('LDR<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 0, W, 1, Rn, Rt, imm12)),
    Instruction('LDR<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, P, U, 0, W, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRB<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 1, W, 1, Rn, Rt, imm12)),
    Instruction('LDRB<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, P, U, 1, W, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRBT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 1, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRBT<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, 0, U, 1, 1, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4H, 1, 1, 0, 1, imm4L)),
    Instruction('LDRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDREX<c> <Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXB<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXD<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXH<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDRH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('LDRH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('LDRHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('LDRHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('LDRSB<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4H, 1, 1, 0, 1, imm4L)),
    Instruction('LDRSB<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 1, 0, 1, imm4L)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDRSH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4H, 1, 1, 1, 1, imm4L)),
    Instruction('LDRSH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4H, 1, 1, 1, 1, imm4L)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('LDRT<c> <Rt>, [<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 0, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRT<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, 0, U, 0, 1, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 0, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 0, 1, Rn)),
    Instruction('LSR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 1, 0, Rm)),
    Instruction('LSR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 1, 1, Rn)),
    Instruction('MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (cond, 1, 1, 1, 0, opc1_3, 0, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MCR2<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (1, 1, 1, 1, 1, 1, 1, 0, opc1_3, 0, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MCRR<c> <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>', (cond, 1, 1, 0, 0, 0, 1, 0, 0, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MCRR2<c> <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>', (1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MLA{S}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MLS<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 1, 1, 0, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MOV{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm12)),
    Instruction('MOVW<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 0, 0, 0, imm4, Rd, imm12)),
    Instruction('MOV{S}<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 0, 0, 0, Rm)),
    Instruction('MOVT<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 1, 0, 0, imm4, Rd, imm12)),
    Instruction('MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (cond, 1, 1, 1, 0, opc1_3, 1, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MRC2<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (1, 1, 1, 1, 1, 1, 1, 0, opc1_3, 1, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MRRC<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>', (cond, 1, 1, 0, 0, 0, 1, 0, 1, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MRS<c> <Rd>,<spec_reg>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('MRRC2<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>', (1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MSR<c> <spec_reg>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, mask, 0, 0, (1), (1), (1), (1), imm12)),
    Instruction('MSR<c> <spec_reg>,<Rn>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, mask, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, Rn)),
    Instruction('MUL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rd, (0), (0), (0), (0), Rm, 1, 0, 0, 1, Rn)),
    Instruction('MVN{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm12)),
    Instruction('MVN{S}<c> <Rd>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm5, typ, 0, Rm)),
    Instruction('MVN{S}<c> <Rd>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, Rs, 0, typ, 1, Rm)),
    Instruction('NOP<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 0)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 0, S, Rn, Rd, imm12)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('PKH<T><c> <Rd>,<Rn>,<Rm>{,<shift>}', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, imm5, tb, 0, 1, Rm)),
    Instruction('PLD{W}<c> [<Rn>,#+/-<imm12>]', (1, 1, 1, 1, 0, 1, 0, 1, U, R, 0, 1, Rn, (1), (1), (1), (1), imm12)),
    Instruction('PLD{W}<c> [<Rn>,+/-<Rm>{,<shift>}]', (1, 1, 1, 1, 0, 1, 1, 1, U, R, 0, 1, Rn, (1), (1), (1), (1), imm5, typ, 0, Rm)),
    Instruction('PLI [<Rn>,#+/-<imm12>]', (1, 1, 1, 1, 0, 1, 0, 0, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm12)),
    Instruction('PLI [<Rn>,+/-<Rm>{,<shift>}]', (1, 1, 1, 1, 0, 1, 1, 0, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm5, typ, 0, Rm)),
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
    Instruction('SBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 0, 1, widthm1, Rd, lsb, 1, 0, 1, Rn)),
    Instruction('SEL<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm)),
    Instruction('SETEND <endian_specifier>', (1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, (0), (0), (0), 1, (0), (0), (0), (0), (0), (0), E, (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('SEV<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('SHADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('SHADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('SHASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('SHSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('SHSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('SHSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('SMLA<x><y><c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, M, N, 0, Rn)),
    Instruction('SMLAD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 0, M, 1, Rn)),
    Instruction('SMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SMLAL<x><y><c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, M, N, 0, Rn)),
    Instruction('SMLALD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 0, M, 1, Rn)),
    Instruction('SMLAW<y><c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, Ra, Rm, 1, M, 0, 0, Rn)),
    Instruction('SMLSD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 1, M, 1, Rn)),
    Instruction('SMLSLD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 1, M, 1, Rn)),
    Instruction('SMMLA{R}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 0, 0, R, 1, Rn)),
    Instruction('SMMLS{R}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 1, 1, R, 1, Rn)),
    Instruction('SMMUL{R}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, 1, 1, 1, 1, Rm, 0, 0, R, 1, Rn)),
    Instruction('SMUAD{X}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, M, 1, Rn)),
    Instruction('SMUL<x><y><c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, M, N, 0, Rn)),
    Instruction('SMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SMULW<y><c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, M, 1, 0, Rn)),
    Instruction('SMUSD{X}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 1, M, 1, Rn)),
    Instruction('SSAT<c> <Rd>,#<imm>,<Rn>{,<shift>}', (cond, 0, 1, 1, 0, 1, 0, 1, sat_imm5, Rd, imm5, sh, 0, 1, Rn)),
    Instruction('SSAT16<c> <Rd>,#<imm>,<Rn>', (cond, 0, 1, 1, 0, 1, 0, 1, 0, sat_imm4, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rn)),
    Instruction('SSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('SSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('SSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('STC{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, D, W, 0, Rn, CRd, coproc, imm8)),
    Instruction('STC2{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, D, W, 0, Rn, CRd, coproc, imm8)),
    Instruction('STM<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 1, 0, W, 0, Rn, register_list)),
    Instruction('STMDA<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 0, 0, W, 0, Rn, register_list)),
    Instruction('STMDB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 0, 0, W, 0, Rn, register_list)),
    Instruction('STMIB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 1, 0, W, 0, Rn, register_list)),
    Instruction('STR<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 0, W, 0, Rn, Rt, imm12)),
    Instruction('STR<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, P, U, 0, W, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRB<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 1, W, 0, Rn, Rt, imm12)),
    Instruction('STRB<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, P, U, 1, W, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRBT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 1, 1, 0, Rn, Rt, imm12)),
    Instruction('STRBT<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}', (cond, 0, 1, 1, 0, U, 1, 1, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4H, 1, 1, 1, 1, imm4L)),
    Instruction('STRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('STREX<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXB<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXD<c> <Rd>,<Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXH<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STRH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('STRH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('STRHT<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 0, Rn, Rt, imm4H, 1, 0, 1, 1, imm4L)),
    Instruction('STRHT<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
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
    Instruction('SXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 1, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTB<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTB16<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTH<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('TEQ<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('TEQ<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TEQ<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('TST<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm12)),
    Instruction('TST<c> <Rn>,<Rm>{,<shift>}', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TST<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('UADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('UADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('UASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 1, widthm1, Rd, lsb, 1, 0, 1, Rn)),
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
    Instruction('USAT<c> <Rd>,#<imm5>,<Rn>{,<shift>}', (cond, 0, 1, 1, 0, 1, 1, 1, sat_imm5, Rd, imm5, sh, 0, 1, Rn)),
    Instruction('USAT16<c> <Rd>,#<imm4>,<Rn>', (cond, 0, 1, 1, 0, 1, 1, 1, 0, sat_imm4, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rn)),
    Instruction('USAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('USUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('USUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('UXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 1, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('UXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 0, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('UXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 1, 1, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('UXTB<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('UXTB16<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('UXTH<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('WFE<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 0)),
    Instruction('WFI<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 1)),
    Instruction('YIELD<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 1)),
    Instruction('MRS<c> <Rd>,<spec_reg>', (cond, 0, 0, 0, 1, 0, R, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('SMC<c> #<imm4>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), 0, 1, 1, 1, imm4)),
]

table = ARMv7Table(_table, 32)
