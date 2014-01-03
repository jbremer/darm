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

from tablegen import Instruction, Macro, Table, Node, ScatteredField
from tablegen import Register, CoprocessorRegister, FloatingPointRegister
from tablegen import Field, NopField, Immediate, ScatteredImmediate, FieldPlus


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

    def _create(self, sm, lut, fmt, bitsize):
        off = sm.alloc(5)
        off2 = lut.alloc(2)

        sm.update(off, 'SM_CMP4', bitsize-4, 0b1111,
                  'L(%d)' % off2, 'H(%d)' % off2)

        cond = self.cond.create(sm, lut, fmt, bitsize)
        uncond = self.uncond.create(sm, lut, fmt, bitsize)
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
typ = Field(2, 'shift_type')

cond = Field(4, 'cond')
S = Field(1, 'S')
W = Field(1, 'W')
P = Field(1, 'P')
U = Field(1, 'U')
sh = ScatteredField(1, 'shift_type', 1)

msb = Field(5, 'msb')
lsb = Field(5, 'msb')
option = Field(4, 'option')
register_list = Field(16, 'register_list')
widthm1 = FieldPlus(5, 'width', 1)
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
imm1_1 = ScatteredImmediate(1, 'imm1', 1)
imm1_4 = ScatteredImmediate(1, 'imm4', 4)
imm1_6 = ScatteredImmediate(1, 'imm1', 1)
imm1_7 = ScatteredImmediate(1, 'imm1', 7)
imm3 = Immediate(3, 'imm3')
imm3_4 = ScatteredImmediate(3, 'imm3', 4)
imm4 = Immediate(4, 'imm4')
imm4_4 = ScatteredImmediate(4, 'imm4', 4)
imm4_12 = ScatteredImmediate(4, 'imm4', 12)
imm5 = Immediate(5, 'imm5')
imm6 = Immediate(6, 'imm6')
imm8 = Immediate(8, 'imm8')
imm12 = Immediate(12, 'imm12')
imm12_4 = ScatteredImmediate(12, 'imm12', 4)
imm24 = Immediate(24, 'imm24')
imm24_2 = ScatteredImmediate(24, 'imm24', 2)

sat_imm5 = Field(5, 'sat_imm')
sat_imm4 = Field(4, 'sat_imm')
rotate = Field(2, 'rotate')

ARMExpandImm = Macro('ARMExpandImm')
NegateImm = Macro('NEG')
SignExtend = Macro('SIGN')

_table = [
    Instruction('ADC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 0, 1, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('ADC{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ADC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ADD{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 0, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('ADD{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ADD{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ADD{S}<c> <Rd>,SP,#<const>', (cond, 0, 0, 1, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm12), macro=ARMExpandImm),
    Instruction('ADD{S}<c> <Rd>,SP,<Rm>,<shift>', (cond, 0, 0, 0, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm5, typ, 0, Rm)),
    Instruction('ADR<c> <Rd>,<label>', (cond, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, imm12), macro=ARMExpandImm),
    Instruction('ADR<c> <Rd>,<label>', (cond, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, Rd, imm12), macros=[ARMExpandImm, NegateImm]),
    Instruction('AND{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 0, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('AND{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('AND{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ASR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 1, 0, 0, Rm)),
    Instruction('ASR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 1, 0, 1, Rn)),
    Instruction('B<c> <label>', (cond, 1, 0, 1, 0, imm24_2), macro=SignExtend(25)),
    Instruction('BFC<c> <Rd>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, 1, 1, 1, 1)),
    Instruction('BFI<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, Rn)),
    Instruction('BIC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 1, 1, 1, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('BIC{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('BIC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('BKPT #<imm16>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, imm12_4, 0, 1, 1, 1, imm4)),
    Instruction('BL<c> <label>', (cond, 1, 0, 1, 1, imm24_2), macro=SignExtend(25)),
    Instruction('BLX <label>', (1, 1, 1, 1, 1, 0, 1, imm1_1, imm24_2), macro=SignExtend(25)),
    Instruction('BLX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('BX<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('BXJ<c> <Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 0, Rm)),
    Instruction('CDP<c> <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>', (cond, 1, 1, 1, 0, opc1, CRn, CRd, coproc, opc2, 0, CRm)),
    Instruction('CDP2 <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>', (1, 1, 1, 1, 1, 1, 1, 0, opc1, CRn, CRd, coproc, opc2, 0, CRm)),
    Instruction('CLREX', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('CLZ<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('CMN<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm12), macro=ARMExpandImm),
    Instruction('CMN<c> <Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('CMN<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('CMP<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm12), macro=ARMExpandImm),
    Instruction('CMP<c> <Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('CMP<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('DBG<c> #<option>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 1, 1, 1, 1, option)),
    Instruction('DMB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 1, option)),
    Instruction('DSB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 0, option)),
    Instruction('EOR{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 0, 1, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('EOR{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('EOR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('ISB #<option>', (1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 1, 0, option)),
    Instruction('LDC<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, 0, W, 1, Rn, CRd, coproc, imm8), coproc=[_ for _ in xrange(2**4) if not _ in (0b1010, 0b1011)]),
    Instruction('LDCL<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, 1, W, 1, Rn, CRd, coproc, imm8), coproc=[_ for _ in xrange(2**4) if not _ in (0b1010, 0b1011)]),
    Instruction('LDC2 <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, 0, W, 1, Rn, CRd, coproc, imm8)),
    Instruction('LDC2L <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, 1, W, 1, Rn, CRd, coproc, imm8)),
    Instruction('LDM<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list)),
    Instruction('LDMDA<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 0, 0, W, 1, Rn, register_list)),
    Instruction('LDMDB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 0, 0, W, 1, Rn, register_list)),
    Instruction('LDMIB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 1, 0, W, 1, Rn, register_list)),
    Instruction('LDR<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 0, W, 1, Rn, Rt, imm12)),
    Instruction('LDR<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, P, U, 0, W, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRB<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 1, W, 1, Rn, Rt, imm12)),
    Instruction('LDRB<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, P, U, 1, W, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRBT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 1, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRBT<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, 0, U, 1, 1, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LDRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4_4, 1, 1, 0, 1, imm4)),
    Instruction('LDRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDREX<c> <Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXB<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXD<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDREXH<c> <Rt>, [<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1))),
    Instruction('LDRH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4_4, 1, 0, 1, 1, imm4)),
    Instruction('LDRH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('LDRHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4_4, 1, 0, 1, 1, imm4)),
    Instruction('LDRHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('LDRSB<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4_4, 1, 1, 0, 1, imm4)),
    Instruction('LDRSB<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4_4, 1, 1, 0, 1, imm4)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 0, 1, Rm)),
    Instruction('LDRSH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 1, Rn, Rt, imm4_4, 1, 1, 1, 1, imm4)),
    Instruction('LDRSH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 1, Rn, Rt, imm4_4, 1, 1, 1, 1, imm4)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>], +/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 1, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('LDRT<c> <Rt>, [<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 0, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRT<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, 0, U, 0, 1, 1, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 0, 0, Rm)),
    Instruction('LSL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 0, 1, Rn)),
    Instruction('LSR{S}<c> <Rd>,<Rm>,#<shift>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 1, 0, Rm)),
    Instruction('LSR{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 1, 1, Rn)),
    Instruction('MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (cond, 1, 1, 1, 0, opc1_3, 0, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MCR2 <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (1, 1, 1, 1, 1, 1, 1, 0, opc1_3, 0, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MCRR<c> <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>', (cond, 1, 1, 0, 0, 0, 1, 0, 0, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MCRR2 <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>', (1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MLA{S}<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 0, 1, S, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MLS<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 0, 0, 1, 1, 0, Rd, Ra, Rm, 1, 0, 0, 1, Rn)),
    Instruction('MOV{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm12), macro=ARMExpandImm),
    Instruction('MOVW<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 0, 0, 0, imm4_12, Rd, imm12)),
    Instruction('MOV{S}<c> <Rd>,<Rm>', (cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 0, 0, 0, Rm)),
    Instruction('MOVT<c> <Rd>,#<imm16>', (cond, 0, 0, 1, 1, 0, 1, 0, 0, imm4_12, Rd, imm12)),
    Instruction('MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (cond, 1, 1, 1, 0, opc1_3, 1, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MRC2<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}', (1, 1, 1, 1, 1, 1, 1, 0, opc1_3, 1, CRn, Rt, coproc, opc2, 1, CRm)),
    Instruction('MRRC<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>', (cond, 1, 1, 0, 0, 0, 1, 0, 1, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MRS<c> <Rd>,<spec_reg>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0))),
    Instruction('MRRC2<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>', (1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, Rt2, Rt, coproc, opc1, CRm)),
    Instruction('MSR<c> <spec_reg>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, mask, 0, 0, (1), (1), (1), (1), imm12), macro=ARMExpandImm),
    Instruction('MSR<c> <spec_reg>,<Rn>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, mask, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, Rn)),
    Instruction('MUL{S}<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 0, 0, 0, S, Rd, (0), (0), (0), (0), Rm, 1, 0, 0, 1, Rn)),
    Instruction('MVN{S}<c> <Rd>,#<const>', (cond, 0, 0, 1, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm12), macro=ARMExpandImm),
    Instruction('MVN{S}<c> <Rd>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm5, typ, 0, Rm)),
    Instruction('MVN{S}<c> <Rd>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, Rs, 0, typ, 1, Rm)),
    Instruction('NOP<c>', (cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 0)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 1, 1, 0, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('ORR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('PKHBT<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, imm5, 0, 0, 1, Rm)),
    Instruction('PKHTB<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, imm5, 1, 0, 1, Rm)),
    Instruction('PLD [<Rn>,#+/-<imm12>]', (1, 1, 1, 1, 0, 1, 0, 1, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm12)),
    Instruction('PLDW [<Rn>,#+/-<imm12>]', (1, 1, 1, 1, 0, 1, 0, 1, U, 0, 0, 1, Rn, (1), (1), (1), (1), imm12)),
    Instruction('PLD [<Rn>,+/-<Rm>,<shift>]', (1, 1, 1, 1, 0, 1, 1, 1, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm5, typ, 0, Rm)),
    Instruction('PLDW [<Rn>,+/-<Rm>,<shift>]', (1, 1, 1, 1, 0, 1, 1, 1, U, 0, 0, 1, Rn, (1), (1), (1), (1), imm5, typ, 0, Rm)),
    Instruction('PLI [<Rn>,#+/-<imm12>]', (1, 1, 1, 1, 0, 1, 0, 0, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm12)),
    Instruction('PLI [<Rn>,+/-<Rm>,<shift>]', (1, 1, 1, 1, 0, 1, 1, 0, U, 1, 0, 1, Rn, (1), (1), (1), (1), imm5, typ, 0, Rm)),
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
    Instruction('RSB{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 1, 1, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('RSB{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('RSB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('RSC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 1, 1, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('RSC{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('RSC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('SADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('SADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('SASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('SBC{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 1, 1, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('SBC{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 1, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
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
    Instruction('SMC<c> #<imm4>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), 0, 1, 1, 1, imm4)),
    Instruction('SMLABB<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, 0, 0, 0, Rn)),
    Instruction('SMLABT<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, 1, 0, 0, Rn)),
    Instruction('SMLATB<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, 0, 1, 0, Rn)),
    Instruction('SMLATT<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, 1, 1, 0, Rn)),
    Instruction('SMLAD<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 0, 0, 1, Rn)),
    Instruction('SMLADX<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 0, 1, 1, Rn)),
    Instruction('SMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SMLALBB<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 0, 0, 0, Rn)),
    Instruction('SMLALBT<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 1, 0, 0, Rn)),
    Instruction('SMLALTB<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 0, 1, 0, Rn)),
    Instruction('SMLALTT<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 1, 1, 0, Rn)),
    Instruction('SMLALD<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 0, 0, 1, Rn)),
    Instruction('SMLALDX<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 0, 1, 1, Rn)),
    Instruction('SMLAWB<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, Ra, Rm, 1, 0, 0, 0, Rn)),
    Instruction('SMLAWT<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, Ra, Rm, 1, 1, 0, 0, Rn)),
    Instruction('SMLSD<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 1, 0, 1, Rn)),
    Instruction('SMLSDX<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 1, 1, 1, Rn)),
    Instruction('SMLSLD<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 1, 0, 1, Rn)),
    Instruction('SMLSLDX<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 1, 1, 1, Rn)),
    Instruction('SMMLA<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 0, 0, 0, 1, Rn)),
    Instruction('SMMLAR<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 0, 0, 1, 1, Rn)),
    Instruction('SMMLS<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 1, 1, 0, 1, Rn)),
    Instruction('SMMLSR<c> <Rd>,<Rn>,<Rm>,<Ra>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 1, 1, 1, 1, Rn)),
    Instruction('SMMUL<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, 1, 1, 1, 1, Rm, 0, 0, 0, 1, Rn)),
    Instruction('SMMULR<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, 1, 1, 1, 1, Rm, 0, 0, 1, 1, Rn)),
    Instruction('SMUAD<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, 0, 1, Rn)),
    Instruction('SMUADX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, 1, 1, Rn)),
    Instruction('SMULBB<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 0, 0, 0, Rn)),
    Instruction('SMULBT<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 1, 0, 0, Rn)),
    Instruction('SMULTB<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 0, 1, 0, Rn)),
    Instruction('SMULTT<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 1, 1, 0, Rn)),
    Instruction('SMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', (cond, 0, 0, 0, 0, 1, 1, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn)),
    Instruction('SMULWB<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 0, 1, 0, Rn)),
    Instruction('SMULWT<c> <Rd>,<Rn>,<Rm>', (cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, 1, 1, 0, Rn)),
    Instruction('SMUSD<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 1, 0, 1, Rn)),
    Instruction('SMUSDX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 1, 1, 1, Rn)),
    Instruction('SSAT<c> <Rd>,#<imm>,<Rn>,<shift>', (cond, 0, 1, 1, 0, 1, 0, 1, sat_imm5, Rd, imm5, sh, 0, 1, Rn)),
    Instruction('SSAT16<c> <Rd>,#<imm>,<Rn>', (cond, 0, 1, 1, 0, 1, 0, 1, 0, sat_imm4, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rn)),
    Instruction('SSAX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm)),
    Instruction('SSUB16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm)),
    Instruction('SSUB8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm)),
    Instruction('STC<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, 0, W, 0, Rn, CRd, coproc, imm8), coproc=[_ for _ in xrange(2**4) if not _ in (0b1010, 0b1011)]),
    Instruction('STCL<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>', (cond, 1, 1, 0, P, U, 1, W, 0, Rn, CRd, coproc, imm8), coproc=[_ for _ in xrange(2**4) if not _ in (0b1010, 0b1011)]),
    Instruction('STC2 <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, 0, W, 0, Rn, CRd, coproc, imm8)),
    Instruction('STC2L <coproc>,<CRd>,[<Rn>],#+/-<imm>', (1, 1, 1, 1, 1, 1, 0, P, U, 1, W, 0, Rn, CRd, coproc, imm8)),
    Instruction('STM<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 1, 0, W, 0, Rn, register_list)),
    Instruction('STMDA<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 0, 0, 0, W, 0, Rn, register_list)),
    Instruction('STMDB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 0, 0, W, 0, Rn, register_list)),
    Instruction('STMIB<c> <Rn>{!},<registers>', (cond, 1, 0, 0, 1, 1, 0, W, 0, Rn, register_list)),
    Instruction('STR<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 0, W, 0, Rn, Rt, imm12)),
    Instruction('STR<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, P, U, 0, W, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRB<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, P, U, 1, W, 0, Rn, Rt, imm12)),
    Instruction('STRB<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, P, U, 1, W, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRBT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 1, 1, 0, Rn, Rt, imm12)),
    Instruction('STRBT<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, 0, U, 1, 1, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('STRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4_4, 1, 1, 1, 1, imm4)),
    Instruction('STRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 1, 1, 1, Rm)),
    Instruction('STREX<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXB<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXD<c> <Rd>,<Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STREXH<c> <Rd>,<Rt>,[<Rn>]', (cond, 0, 0, 0, 1, 1, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt)),
    Instruction('STRH<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, P, U, 1, W, 0, Rn, Rt, imm4_4, 1, 0, 1, 1, imm4)),
    Instruction('STRH<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, P, U, 0, W, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('STRHT<c> <Rt>,[<Rn>],#+/-<imm8>', (cond, 0, 0, 0, 0, U, 1, 1, 0, Rn, Rt, imm4_4, 1, 0, 1, 1, imm4)),
    Instruction('STRHT<c> <Rt>,[<Rn>],+/-<Rm>', (cond, 0, 0, 0, 0, U, 0, 1, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 1, 1, Rm)),
    Instruction('STRT<c> <Rt>,[<Rn>],#+/-<imm12>', (cond, 0, 1, 0, 0, U, 0, 1, 0, Rn, Rt, imm12)),
    Instruction('STRT<c> <Rt>,[<Rn>],+/-<Rm>,<shift>', (cond, 0, 1, 1, 0, U, 0, 1, 0, Rn, Rt, imm5, typ, 0, Rm)),
    Instruction('SUB{S}<c> <Rd>,<Rn>,#<const>', (cond, 0, 0, 1, 0, 0, 1, 0, S, Rn, Rd, imm12), macro=ARMExpandImm),
    Instruction('SUB{S}<c> <Rd>,<Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('SUB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
    Instruction('SUB{S}<c> <Rd>,SP,#<const>', (cond, 0, 0, 1, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm12), macro=ARMExpandImm),
    Instruction('SUB{S}<c> <Rd>,SP,<Rm>,<shift>', (cond, 0, 0, 0, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm5, typ, 0, Rm)),
    Instruction('SVC<c> #<imm24>', (cond, 1, 1, 1, 1, imm24)),
    Instruction('SWP<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 0, 0, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2)),
    Instruction('SWPB<c> <Rt>,<Rt2>,[<Rn>]', (cond, 0, 0, 0, 1, 0, 1, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2)),
    Instruction('SXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 1, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTB<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTB16<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('SXTH<c> <Rd>,<Rm>{,<rotation>}', (cond, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm)),
    Instruction('TEQ<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm12), macro=ARMExpandImm),
    Instruction('TEQ<c> <Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TEQ<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('TST<c> <Rn>,#<const>', (cond, 0, 0, 1, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm12), macro=ARMExpandImm),
    Instruction('TST<c> <Rn>,<Rm>,<shift>', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm5, typ, 0, Rm)),
    Instruction('TST<c> <Rn>,<Rm>,<type> <Rs>', (cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, typ, 1, Rm)),
    Instruction('UADD16<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm)),
    Instruction('UADD8<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm)),
    Instruction('UASX<c> <Rd>,<Rn>,<Rm>', (cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm)),
    Instruction('UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', (cond, 0, 1, 1, 1, 1, 1, 1, widthm1, Rd, lsb, 1, 0, 1, Rn)),
    Instruction('UDF<c> #<imm16>', (cond, 0, 1, 1, 1, 1, 1, 1, 1, imm12_4, 1, 1, 1, 1, imm4)),
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
    Instruction('USAT<c> <Rd>,#<imm5>,<Rn>,<shift>', (cond, 0, 1, 1, 0, 1, 1, 1, sat_imm5, Rd, imm5, sh, 0, 1, Rn)),
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
]

Vd = FloatingPointRegister(4, 'Vd', 22)
Vn = FloatingPointRegister(4, 'Vn', 7)
Vm = FloatingPointRegister(4, 'Vm', 5)

D = NopField(1, 'D')
M = NopField(1, 'M')
N = NopField(1, 'N')
Q = Field(1, 'Q')
F = Field(1, 'F')
T = Field(1, 'T')
B = Field(1, 'B')
U = Field(1, 'U')
E = Field(1, 'E')

size = Field(2, 'size')
sz = Field(1, 'sz')
sf = Field(1, 'sf')
sx = Field(1, 'sx')
cmode = Field(4, 'cmode')
align = Field(2, 'align')
index_align = Field(4, 'index_align')
a = Field(1, 'a')
op1 = Field(1, 'op1')
op2 = Field(2, 'op2')
type_ = Field(4, 'type_')
len_ = Field(2, 'len')
opc1 = Field(2, 'opc1')
opc2 = Field(3, 'opc2')
opc2_2 = Field(2, 'opc2')


_table2 = [
    Instruction('VABA.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 1, 1, N, Q, M, 1, Vm)),
    Instruction('VABAL.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 1, 0, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VABD.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VABDL.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 1, 1, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VABD.F32', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VABS.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 1, 1, 0, Q, M, 0, Vm)),
    Instruction('VABS<c>.F64 <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 0, 0, 0, Vd, 1, 0, 1, sz, 1, 1, M, 0, Vm)),
    Instruction('VACGE.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 0, N, Q, M, 1, Vm)),
    Instruction('VACGT.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, sz, Vn, Vd, 1, 1, 1, 0, N, Q, M, 1, Vm)),
    Instruction('VADD.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, size, Vn, Vd, 1, 0, 0, 0, N, Q, M, 0, Vm)),
    Instruction('VADD.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VADD<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 1, 1, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VADDHN.<dt> <Dd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 0, 1, 0, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VADDL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 0, 0, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VADDW.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 0, 0, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VAND <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, 0, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VBIC.<dt> <Qd>,#<imm>', (1, 1, 1, 1, 0, 0, 1, imm1_7, 1, D, 0, 0, 0, imm3_4, Vd, cmode, 0, Q, 1, 1, imm4), cmode=[0b0001, 0b0011, 0b0101, 0b0111, 0b1001, 0b1011]),
    Instruction('VBIC <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, 1, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VBSL <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, 1, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VBIT <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, 0, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VBIF <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, 1, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VCEQ.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, size, Vn, Vd, 1, 0, 0, 0, N, Q, M, 1, Vm)),
    Instruction('VCEQ.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VCEQ.<dt> <Qd>,<Qm>,#0', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 0, 1, 0, Q, M, 0, Vm)),
    Instruction('VCGE.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 1, 1, N, Q, M, 1, Vm)),
    Instruction('VCGE.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VCGE.<dt> <Qd>,<Qm>,#0', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 0, 0, 1, Q, M, 0, Vm)),
    Instruction('VCGT.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VCGT.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, sz, Vn, Vd, 1, 1, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VCGT.<dt> <Qd>,<Qm>,#0', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 0, 0, 0, Q, M, 0, Vm)),
    Instruction('VCLE.<dt> <Qd>,<Qm>,#0', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 0, 1, 1, Q, M, 0, Vm)),
    Instruction('VCLS.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 0, 0, 0, Q, M, 0, Vm)),
    Instruction('VCLT.<dt> <Qd>,<Qm>,#0', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 1, 0, 0, Q, M, 0, Vm)),
    Instruction('VCLZ.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 0, 0, 1, Q, M, 0, Vm)),
    Instruction('VCMP<c>.<f> <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 1, 0, 0, Vd, 1, 0, 1, sz, 0, 1, M, 0, Vm)),
    Instruction('VCMPE<c>.<f> <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 1, 0, 0, Vd, 1, 0, 1, sz, 1, 1, M, 0, Vm)),
    Instruction('VCMP<c>.<f> <Dd>,#0.0', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 1, 0, 1, Vd, 1, 0, 1, sz, 0, 1, (0), 0, (0), (0), (0), (0))),
    Instruction('VCMPE<c>.<f> <Dd>,#0.0', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 1, 0, 1, Vd, 1, 0, 1, sz, 1, 1, (0), 0, (0), (0), (0), (0))),
    Instruction('VCNT.8 <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 0, 1, 0, Q, M, 0, Vm)),
    Instruction('VCVT.<Td>.<Tm> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 1, Vd, 0, 1, 1, op2, Q, M, 0, Vm)),
    Instruction('VCVTR<c>.S32.F64 <Sd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 1, opc2, Vd, 1, 0, 1, sz, 0, 1, M, 0, Vm)),
    Instruction('VCVT<c>.S32.F64 <Sd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 1, opc2, Vd, 1, 0, 1, sz, 1, 1, M, 0, Vm)),
    Instruction('VCVT.<Td>.<Tm> <Qd>,<Qm>,#<fbits>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 1, 1, 1, op1, 0, Q, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VCVT<c>.<Td>.F64 <Dd>,<Dd>,#<fbits>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 1, op1, 1, U, Vd, 1, 0, 1, sf, sx, 1, imm1_4, 0, imm4)),
    Instruction('VCVT<c>.F64.F32 <Dd>,<Sm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 1, 1, 1, Vd, 1, 0, 1, sz, 1, 1, M, 0, Vm)),
    Instruction('VCVT.F32.F16 <Qd>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 1, 1, op1, 0, 0, M, 0, Vm)),
    Instruction('VCVT<y><c>.F32.F16 <Sd>,<Sm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 0, 1, op1, Vd, 1, 0, 1, (0), T, 1, M, 0, Vm)),
    Instruction('VDIV<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 0, 0, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VDUP.<size>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, imm4, Vd, 1, 1, 0, 0, 0, Q, M, 0, Vm)),
    Instruction('VDUP<c>.<size>', (cond, 1, 1, 1, 0, 1, B, Q, 0, Vd, Rt, 1, 0, 1, 1, D, 0, E, 1, (0), (0), (0), (0))),
    Instruction('VEOR <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, 0, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VEXT.8 <Qd>,<Qn>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, 1, 1, Vn, Vd, imm4, N, Q, M, 0, Vm)),
    Instruction('VFMA.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 0, 0, N, Q, M, 1, Vm)),
    Instruction('VFMS.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, sz, Vn, Vd, 1, 1, 0, 0, N, Q, M, 1, Vm)),
    Instruction('VFMA<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 0, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VFMS<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 0, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VFNMS<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 0, 1, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VFNMA<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 0, 1, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VHADD.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 0, 0, N, Q, M, 0, Vm)),
    Instruction('VHSUB.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VLD1.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 1, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0010, 0b0110, 0b0111, 0b1010]),
    Instruction('VLD1.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, size, 0, 0, index_align, Rm)),
    Instruction('VLD1.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, 1, 1, 0, 0, size, T, a, Rm)),
    Instruction('VLD2.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 1, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0011, 0b1000, 0b1001]),
    Instruction('VLD2.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, size, 0, 1, index_align, Rm)),
    Instruction('VLD2.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, 1, 1, 0, 1, size, T, a, Rm)),
    Instruction('VLD3.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 1, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0100, 0b0101]),
    Instruction('VLD3.<size> <list>,[<Rn>]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, size, 1, 0, index_align, Rm)),
    Instruction('VLD3.<size> <list>,[<Rn>]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, 1, 1, 1, 0, size, T, a, Rm)),
    Instruction('VLD4.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 1, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0000, 0b0001]),
    Instruction('VLD4.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, size, 1, 1, index_align, Rm)),
    Instruction('VLD4.<size> <list>,[<Rn>{ :<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 1, 0, Rn, Vd, 1, 1, 1, 1, size, T, a, Rm)),
    Instruction('VLDM{mode}<c> <Rn>{!},<list>', (cond, 1, 1, 0, P, U, D, W, 1, Rn, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VLDM{mode}<c> <Rn>{!},<list>', (cond, 1, 1, 0, P, U, D, W, 1, Rn, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VLDR<c> <Dd>,[<Rn>{,#+/-<imm>}]', (cond, 1, 1, 0, 1, U, D, 0, 1, Rn, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VLDR<c> <Sd>,[<Rn>{,#+/-<imm>}]', (cond, 1, 1, 0, 1, U, D, 0, 1, Rn, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VMAX.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VMIN.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 1, 0, N, Q, M, 1, Vm)),
    Instruction('VMAX.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VMIN.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VMLA.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, size, Vn, Vd, 1, 0, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VMLS.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, size, Vn, Vd, 1, 0, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VMLAL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 1, 0, 0, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMLSL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 1, 0, 1, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMLA.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VMLS.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VMLA<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 0, 0, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VMLS<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 0, 0, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VMLA.<dt> <Qd>,<Qn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, Q, 1, D, size, Vn, Vd, 0, 0, 0, F, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMLS.<dt> <Qd>,<Qn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, Q, 1, D, size, Vn, Vd, 0, 1, 0, F, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMLAL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 0, 1, 0, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMLSL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 1, 1, 0, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMOV.<dt> <Qd>,#<imm>', (1, 1, 1, 1, 0, 0, 1, imm1_7, 1, D, 0, 0, 0, imm3_4, Vd, cmode, 0, Q, 0, 1, imm4), cmode=[0b0000, 0b0010, 0b0100, 0b0110, 0b1000, 0b1010, 0b1100, 0b1101, 0b1110, 0b1111]),
    Instruction('VMOV.<dt> <Qd>,#<imm>', (1, 1, 1, 1, 0, 0, 1, imm1_7, 1, D, 0, 0, 0, imm3_4, Vd, cmode, 0, Q, 1, 1, imm4), cmode=[0b1110]),
    Instruction('VMOV<c>.F64 <Dd>,#<imm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, imm4_4, Vd, 1, 0, 1, sz, (0), 0, (0), 0, imm4)),
    # Instruction('VMOV <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, 0, Vm, Vd, 0, 0, 0, 1, M, Q, M, 1, Vm)),
    Instruction('VMOV<c>.F64 <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 0, 0, 0, Vd, 1, 0, 1, sz, 0, 1, M, 0, Vm)),
    # Instruction('VMOV<c>.<size> <Dd[x]>,<Rt>', (cond, 1, 1, 1, 0, 0, opc1, 0, Vd, Rt, 1, 0, 1, 1, D, opc2_2, 1, (0), (0), (0), (0))),
    # Instruction('VMOV<c>.<dt> <Rt>,<Dn[x]>', (cond, 1, 1, 1, 0, U, opc1, 1, Vn, Rt, 1, 0, 1, 1, N, opc2_2, 1, (0), (0), (0), (0))),
    # Instruction('VMOV<c> <Sn>,<Rt>', (cond, 1, 1, 1, 0, 0, 0, 0, op1, Vn, Rt, 1, 0, 1, 0, N, (0), (0), 1, (0), (0), (0), (0))),
    # Instruction('VMOV<c> <Sm>,<Sm1>,<Rt>,<Rt2>', (cond, 1, 1, 0, 0, 0, 1, 0, op1, Rt2, Rt, 1, 0, 1, 0, 0, 0, M, 1, Vm)),
    # Instruction('VMOV<c> <Dm>,<Rt>,<Rt2>', (cond, 1, 1, 0, 0, 0, 1, 0, op1, Rt2, Rt, 1, 0, 1, 1, 0, 0, M, 1, Vm)),
    Instruction('VMOVL.<dt> <Qd>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm3, 0, 0, 0, Vd, 1, 0, 1, 0, 0, 0, M, 1, Vm), imm3=[0b001, 0b010, 0b100]),
    Instruction('VMOVN.<dt> <Dd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 1, 0, 0, 0, M, 0, Vm)),
    Instruction('VMRS<c> <Rt>,FPSCR', (cond, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, Rt, 1, 0, 1, 0, (0), (0), (0), 1, (0), (0), (0), (0))),
    Instruction('VMSR<c> FPSCR,<Rt>', (cond, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, Rt, 1, 0, 1, 0, (0), (0), (0), 1, (0), (0), (0), (0))),
    Instruction('VMUL.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, op1, 0, D, size, Vn, Vd, 1, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VMULL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 1, 1, op1, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMUL.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VMUL<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 1, 0, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VMUL.<dt> <Qd>,<Qn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, Q, 1, D, size, Vn, Vd, 1, 0, 0, F, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMULL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 1, 0, 1, 0, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VMVN.<dt> <Qd>,#<imm>', (1, 1, 1, 1, 0, 0, 1, imm1_7, 1, D, 0, 0, 0, imm3_4, Vd, cmode, 0, Q, 1, 1, imm4), cmode=[0b0000, 0b0010, 0b0100, 0b0110, 0b1000, 0b1010, 0b1100, 0b1101]),
    Instruction('VMVN <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 0, 1, 1, Q, M, 0, Vm)),
    Instruction('VNEG.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 1, Vd, 0, F, 1, 1, 1, Q, M, 0, Vm)),
    Instruction('VNEG<c>.F64 <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 0, 0, 1, Vd, 1, 0, 1, sz, 0, 1, M, 0, Vm)),
    Instruction('VNMLA<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 0, 1, Vn, Vd, 1, 0, 1, sz, N, 0, M, 0, Vm)),
    Instruction('VNMLS<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 0, 1, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VNMUL<c>.<f> <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 1, 0, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VORN <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, 1, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    # Instruction('VORR.<dt> <Qd>,#<imm>', (1, 1, 1, 1, 0, 0, 1, imm1_7, 1, D, 0, 0, 0, imm3_4, Vd, cmode, 0, Q, 0, 1, imm4), cmode=[0b0001, 0b0011, 0b0101, 0b0111, 0b1001, 0b1011]),
    Instruction('VORR <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, 0, Vn, Vd, 0, 0, 0, 1, N, Q, M, 1, Vm)),
    Instruction('VPADAL.<dt>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 1, 0, op1, Q, M, 0, Vm)),
    Instruction('VPADD.<dt>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, size, Vn, Vd, 1, 0, 1, 1, N, Q, M, 1, Vm)),
    Instruction('VPADD.F32', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VPADDL.<dt>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 0, 1, 0, op1, Q, M, 0, Vm)),
    Instruction('VPMAX.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 1, 0, 1, 0, N, Q, M, 0, Vm)),
    Instruction('VPMIN.<dt>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 1, 0, 1, 0, N, Q, M, 1, Vm)),
    Instruction('VPMAX.F32', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VPMIN.F32', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, 1, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VPOP <list>', (cond, 1, 1, 0, 0, 1, D, 1, 1, 1, 1, 0, 1, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VPOP <list>', (cond, 1, 1, 0, 0, 1, D, 1, 1, 1, 1, 0, 1, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VPUSH<c> <list>', (cond, 1, 1, 0, 1, 0, D, 1, 0, 1, 1, 0, 1, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VPUSH<c> <list>', (cond, 1, 1, 0, 1, 0, D, 1, 0, 1, 1, 0, 1, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VQABS.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 1, 1, 0, Q, M, 0, Vm)),
    Instruction('VQADD.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 0, 0, N, Q, M, 1, Vm)),
    Instruction('VQDMLAL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 1, 0, 0, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMLSL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 1, 0, 1, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMLAL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 0, 0, 1, 1, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMLSL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 0, 1, 1, 1, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMULH.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, size, Vn, Vd, 1, 0, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VQDMULH.<dt> <Qd>,<Qn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, Q, 1, D, size, Vn, Vd, 1, 1, 0, 0, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMULL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 1, 1, 0, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQDMULL.<dt> <Qd>,<Dn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 1, 0, 1, 1, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQMOVUN.<type><size> <Dd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 1, 0, op2, M, 0, Vm), op2=[0b01]),
    Instruction('VQMOVN.<type><size> <Dd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 1, 0, op2, M, 0, Vm), op2=[0b10, 0b11]),
    Instruction('VQNEG.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 1, 1, 1, 1, Q, M, 0, Vm)),
    Instruction('VQRDMULH.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, size, Vn, Vd, 1, 0, 1, 1, N, Q, M, 0, Vm)),
    Instruction('VQRDMULH.<dt> <Qd>,<Qn>,<Dm[x]>', (1, 1, 1, 1, 0, 0, 1, Q, 1, D, size, Vn, Vd, 1, 1, 0, 1, N, 1, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VQRSHL.<type><size> <Qd>,<Qm>,<Qn>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 0, 1, N, Q, M, 1, Vm)),
    # TODO op correct?
    Instruction('VQRSHRUN.<type><size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, imm6, Vd, 1, 0, 0, op1, 0, 1, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VQRSHRN.<type><size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 1, 0, 0, op1, 0, 1, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VQSHL.<type><size> <Qd>,<Qm>,<Qn>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 0, 0, N, Q, M, 1, Vm)),
    # Instruction('VQSHL.<type><size> <Qd>,<Qm>,#<imm>',  (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 0, 1, 1, op1, imm1_6, Q, M, 1, Vm)),
    # Instruction('VQSHLU.<type><size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, imm6, Vd, 0, 1, 1, op1, imm1_6, Q, M, 1, Vm)),
    Instruction('VQSHRN.<type><size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 1, 0, 0, op1, 0, 0, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VQSHRUN.<type><size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, imm6, Vd, 1, 0, 0, op1, 0, 0, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VQSUB.<type><size> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 1, 0, N, Q, M, 1, Vm)),
    Instruction('VRADDHN.<dt> <Dd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, size, Vn, Vd, 0, 1, 0, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VRECPE.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 1, Vd, 0, 1, 0, F, 0, Q, M, 0, Vm)),
    Instruction('VRECPS.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 0, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 1, Vm)),
    Instruction('VREV16.<size> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 0, 0, 1, 0, Q, M, 0, Vm)),
    Instruction('VREV32.<size> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 0, 0, 0, 1, Q, M, 0, Vm)),
    Instruction('VREV64.<size> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 0, 0, Vd, 0, 0, 0, 0, 0, Q, M, 0, Vm)),
    Instruction('VRHADD <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 0, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VRSHL.<type><size> <Qd>,<Qm>,<Qn>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 0, 1, N, Q, M, 0, Vm)),
    # Instruction('VRSHR.<type><size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 0, 0, 1, 0, imm1_6, Q, M, 1, Vm)),
    Instruction('VRSHRN.I<size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 1, 0, 0, 0, 0, 1, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VRSQRTE.<dt> <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 1, Vd, 0, 1, 0, F, 1, Q, M, 0, Vm)),
    Instruction('VRSQRTS.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, sz, Vn, Vd, 1, 1, 1, 1, N, Q, M, 1, Vm)),
    Instruction('VRSRA.<type><size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 0, 0, 1, 1, imm1_6, Q, M, 1, Vm)),
    Instruction('VRSUBHN.<dt> <Dd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, size, Vn, Vd, 0, 1, 1, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VSHL.I<size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 0, 1, 0, 1, imm1_6, Q, M, 1, Vm)),
    Instruction('VSHL.<type><size> <Qd>,<Qm>,<Qn>', (1, 1, 1, 1, 0, 0, 1, U, 0, D, size, Vn, Vd, 0, 1, 0, 0, N, Q, M, 0, Vm)),
    Instruction('VSHLL.<type><size> <Qd>,<Dm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 1, 0, 1, 0, 0, 0, M, 1, Vm), imm6=[_ for _ in xrange(0b001000, 2**6) if not _ >> 3 in (0b001, 0b010, 0b100)]),
    Instruction('VSHLL.<type><size> <Qd>,<Dm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 1, 1, 0, 0, M, 0, Vm)),
    Instruction('VSHR.<type><size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 0, 0, 0, 0, imm1_6, Q, M, 1, Vm)),
    Instruction('VSHRN.I<size> <Dd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, imm6, Vd, 1, 0, 0, 0, 0, 0, M, 1, Vm), imm6=range(0b001000, 2**6)),
    Instruction('VSLI.<size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, imm6, Vd, 0, 1, 0, 1, imm1_6, Q, M, 1, Vm)),
    Instruction('VSQRT<c>.F64 <Dd>,<Dm>', (cond, 1, 1, 1, 0, 1, D, 1, 1, 0, 0, 0, 1, Vd, 1, 0, 1, sz, 1, 1, M, 0, Vm)),
    Instruction('VSRA.<type><size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, imm6, Vd, 0, 0, 0, 1, imm1_6, Q, M, 1, Vm)),
    Instruction('VSRI.<size> <Qd>,<Qm>,#<imm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, imm6, Vd, 0, 1, 0, 0, imm1_6, Q, M, 1, Vm)),
    Instruction('VST1.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 0, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0010, 0b0110, 0b0111, 0b1010]),
    Instruction('VST1.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 0, 0, Rn, Vd, size, 0, 0, index_align, Rm)),
    Instruction('VST2.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 0, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0011, 0b1000, 0b1001]),
    Instruction('VST2.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 0, 0, Rn, Vd, size, 0, 1, index_align, Rm)),
    Instruction('VST3.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 0, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0100, 0b0101]),
    Instruction('VST3.<size> <list>,[<Rn>]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 0, 0, Rn, Vd, size, 1, 0, index_align, Rm)),
    Instruction('VST4.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 0, D, 0, 0, Rn, Vd, type_, size, align, Rm), type_=[0b0000, 0b0001]),
    Instruction('VST4.<size> <list>,[<Rn>{:<align>}]{!}', (1, 1, 1, 1, 0, 1, 0, 0, 1, D, 0, 0, Rn, Vd, size, 1, 1, index_align, Rm)),
    Instruction('VSTM{mode}<c> <Rn>{!},<list>', (cond, 1, 1, 0, P, U, D, W, 0, Rn, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VSTM{mode}<c> <Rn>{!},<list>', (cond, 1, 1, 0, P, U, D, W, 0, Rn, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VSTR<c> <Dd>,[<Rn>{,#+/-<imm>}]', (cond, 1, 1, 0, 1, U, D, 0, 0, Rn, Vd, 1, 0, 1, 1, imm8)),
    Instruction('VSTR<c> <Sd>,[<Rn>{,#+/-<imm>}]', (cond, 1, 1, 0, 1, U, D, 0, 0, Rn, Vd, 1, 0, 1, 0, imm8)),
    Instruction('VSUB.<dt> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 0, D, size, Vn, Vd, 1, 0, 0, 0, N, Q, M, 0, Vm)),
    Instruction('VSUB.F32 <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, 1, sz, Vn, Vd, 1, 1, 0, 1, N, Q, M, 0, Vm)),
    Instruction('VSUB<c>.F64 <Dd>,<Dn>,<Dm>', (cond, 1, 1, 1, 0, 0, D, 1, 1, Vn, Vd, 1, 0, 1, sz, N, 1, M, 0, Vm)),
    Instruction('VSUBHN.<dt> <Dd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 1, D, size, Vn, Vd, 0, 1, 1, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VSUBL.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 0, 1, 0, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VSUBW.<dt> <Qd>,<Dn>,<Dm>', (1, 1, 1, 1, 0, 0, 1, U, 1, D, size, Vn, Vd, 0, 0, 1, 1, N, 0, M, 0, Vm), size=[0b00, 0b01, 0b10]),
    Instruction('VSWP <Qd>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 0, 0, 0, Q, M, 0, Vm)),
    Instruction('VTBL.8 <Dd>,<list>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, Vn, Vd, 1, 0, len_, N, 0, M, 0, Vm)),
    Instruction('VTBX.8 <Dd>,<list>,<Dm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, Vn, Vd, 1, 0, len_, N, 1, M, 0, Vm)),
    Instruction('VTRN.<size>', (1, 1, 1, 1, 0, 0, 1, 1, 1, D, 1, 1, size, 1, 0, Vd, 0, 0, 0, 0, 1, Q, M, 0, Vm)),
    Instruction('VTST.<size> <Qd>,<Qn>,<Qm>', (1, 1, 1, 1, 0, 0, 1, 0, 0, D, size, Vn, Vd, 1, 0, 0, 0, N, Q, M, 1, Vm)),
]

table = ARMv7Table(_table + _table2, 32)
