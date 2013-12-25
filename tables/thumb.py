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

from tablegen import Instruction, Table, Node, Immediate
from tablegen import ScatteredImmediate, ScatteredSignExtendImmediate
from tablegen import DoubleRegister, Register, Field, BranchNotXorImmediate


class ThumbTable(Table):
    def _init(self):
        self.thumb = Node()
        self.thumb2 = Node()
        return self.thumb, self.thumb2

    def _insert(self, ins):
        if ins.bitsize(len(ins.bits)) == 16:
            self.thumb.insert(ins)
        elif ins.bitsize(len(ins.bits)) == 32:
            self.thumb2.insert(ins)
        else:
            raise

    def _process(self):
        self.thumb.process()
        self.thumb2.process()

    def _create(self, sm, lut, fmt, bitsize):
        off = sm.alloc(4)
        off2 = lut.alloc(32)

        sm.update(off, 'SM_TBL5', bitsize-5, 'L(%d)' % off2, 'H(%d)' % off2)

        thumb = self.thumb.create(sm, lut, fmt, bitsize)
        thumb2 = self.thumb2.create(sm, lut, fmt, bitsize)

        is_thumb2 = lambda _: _ in (0b11101, 0b11110, 0b11111)
        tbl = [thumb2 if is_thumb2(_) else thumb for _ in xrange(32)]
        lut.update(off2, *tbl)


Rd = Register(4, 'Rd')
Rd3 = Register(3, 'Rd')
Rn = Register(4, 'Rn')
Rn3 = Register(3, 'Rn')
Rm = Register(4, 'Rm')
Rm3 = Register(3, 'Rm')
Ra = Register(4, 'Ra')
Rt = Register(4, 'Rt')
Rt3 = Register(3, 'Rt')
Rt2 = Register(4, 'Rt2')
RdHi = Register(4, 'RdHi')
RdLo = Register(4, 'RdLo')
Rdn3 = DoubleRegister(3, 'Rd', 'Rn')
DN = DoubleRegister(1, 'Rd', 'Rn')
Rdm3 = DoubleRegister(3, 'Rd', 'Rm')
DM = DoubleRegister(1, 'Rd', 'Rm')

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
sh = Field(1, 'sh')

msb = Field(5, 'msb')
option = Field(4, 'option')
register_list = Field(16, 'register_list')
register_list8 = Field(8, 'register_list')
widthm1 = Field(5, 'widthm1')
E = Field(1, 'E')

firstcond = Field(4, 'first_cond')
it_mask = Field(4, 'it_mask')

msr_mask = Field(2, 'msr_mask')

imm1_6 = ScatteredImmediate(1, 'imm1', 6)
imm1_11 = ScatteredImmediate(1, 'imm1', 11)
imm1_18 = ScatteredImmediate(1, 'imm1', 18)
imm1_19 = ScatteredImmediate(1, 'imm1', 19)
imm1_22_bnxor = BranchNotXorImmediate(1, 'imm1', 22)
imm1_23_bnxor = BranchNotXorImmediate(1, 'imm1', 23)
imm1_20_sign = ScatteredSignExtendImmediate(1, 'imm1', 20)
imm2 = Immediate(2, 'imm2')
imm3 = Immediate(3, 'imm3')
imm3_2 = ScatteredImmediate(3, 'imm3', 2)
imm3_8 = ScatteredImmediate(3, 'imm3', 8)
imm4_12 = ScatteredImmediate(4, 'imm4', 12)
imm5 = Immediate(5, 'imm5')
imm5_1 = ScatteredImmediate(5, 'imm5', 1)
imm6_12 = ScatteredImmediate(6, 'imm6', 12)
imm7 = Immediate(7, 'imm7')
imm8 = Immediate(8, 'imm8')
imm10_2 = ScatteredImmediate(10, 'imm10', 2)
imm10_12 = ScatteredImmediate(10, 'imm10', 12)
imm11 = Immediate(11, 'imm11')
imm11_1 = ScatteredImmediate(11, 'imm11', 1)
imm12 = Immediate(12, 'imm12')

sat_imm5 = Immediate(5, 'sat_imm')
sat_imm4 = Immediate(4, 'sat_imm')
rotate = Field(2, 'rotate')

_table = [
    Instruction('ADC{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 0, 1, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('ADC{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 1, 0, 1, Rm3, Rdn3)),
    Instruction('ADC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('ADD{S}<c> <Rd3>, <Rn3>, #<imm3>', (0, 0, 0, 1, 1, 1, 0, imm3, Rn3, Rd3)),
    Instruction('ADD{S}<c> <Rdn3>, #<imm8>', (0, 0, 1, 1, 0, Rdn3, imm8)),
    Instruction('ADD{S}<c>.W <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 0, 0, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('ADDW<c> <Rd>, <Rn>, #<imm12>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 0, 0, 0, 0, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('ADD{S}<c> <Rd>, <Rn>, <Rm>', (0, 0, 0, 1, 1, 0, 0, Rm3, Rn3, Rd3)),
    Instruction('ADD<c> <Rdn>, <Rm>', (0, 1, 0, 0, 0, 1, 0, 0, DN, Rm, Rdn3)),
    Instruction('ADD{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('ADD<c> <Rd3>, SP, #<imm>', (1, 0, 1, 0, 1, Rd3, imm8)),
    Instruction('ADD<c> SP, SP, #<imm>', (1, 0, 1, 1, 0, 0, 0, 0, 0, imm7)),
    # Instruction('ADD{S}<c>.W <Rd>, SP, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 0, 0, 0, S, 1, 1, 0, 1, 0, imm3_8, Rd, imm8)),
    Instruction('ADDW<c> <Rd>, SP, #<imm12>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, imm3_8, Rd, imm8)),
    Instruction('ADD<c> <Rdm>, SP, <Rdm>', (0, 1, 0, 0, 0, 1, 0, 0, DM, 1, 1, 0, 1, Rdm3)),
    # Instruction('ADD<c> SP, <Rm>', (0, 1, 0, 0, 0, 1, 0, 0, 1, Rm, 1, 0, 1)),
    # Instruction('ADD{S}<c>.W <Rd>, SP, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, 1, 1, 0, 1, (0), imm3, Rd, imm2, typ, Rm)),
    Instruction('ADR<c> <Rd3>, <label>', (1, 0, 1, 0, 0, Rd3, imm8)),
    Instruction('ADR<c>.W <Rd>, <label>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, imm3_8, Rd, imm8)),
    Instruction('ADR<c>.W <Rd>, <label>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, imm3_8, Rd, imm8)),
    Instruction('AND{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 0, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('AND{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 0, 0, 0, Rm3, Rdn3)),
    Instruction('AND{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('ASR{S}<c> <Rd3>, <Rm3>, #<imm>', (0, 0, 0, 1, 0, imm5, Rm3, Rd3)),
    Instruction('ASR{S}<c>.W <Rd>, <Rm>, #<imm>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3_2, Rd, imm2, 1, 0, Rm)),
    Instruction('ASR{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 1, 0, 0, Rm3, Rdn3)),
    Instruction('ASR{S}<c>.W <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('B<c> <label>', (1, 1, 0, 1, cond, imm8)),
    Instruction('B<c> <label>', (1, 1, 1, 0, 0, imm11)),
    Instruction('B<c>.W <label>', (1, 1, 1, 1, 0, imm1_20_sign, cond, imm6_12, 1, 0, imm1_18, 0, imm1_19, imm11_1)),
    Instruction('B<c>.W <label>', (1, 1, 1, 1, 0, S, imm10_12, 1, 0, imm1_23_bnxor, 1, imm1_22_bnxor, imm11_1)),
    Instruction('BFC<c> <Rd>, #<lsb>, #<width>', (1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, imm3_2, Rd, imm2, (0), msb)),
    Instruction('BFI<c> <Rd>, <Rn>, #<lsb>, #<width>', (1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 1, 0, Rn, 0, imm3_2, Rd, imm2, (0), msb)),
    Instruction('BIC{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 0, 1, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('BIC{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 1, 1, 0, Rm3, Rdn3)),
    Instruction('BIC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('BKPT #<imm8>', (1, 0, 1, 1, 1, 1, 1, 0, imm8)),
    Instruction('BL<c> <label>', (1, 1, 1, 1, 0, S, imm10_12, 1, 1, imm1_23_bnxor, 1, imm1_22_bnxor, imm11_1)),
    Instruction('BLX<c> <label>', (1, 1, 1, 1, 0, S, imm10_12, 1, 1, imm1_23_bnxor, 0, imm1_22_bnxor, imm10_2, 0)),
    Instruction('BLX<c> <Rm>', (0, 1, 0, 0, 0, 1, 1, 1, 1, Rm, (0), (0), (0))),
    Instruction('BX<c> <Rm>', (0, 1, 0, 0, 0, 1, 1, 1, 0, Rm, (0), (0), (0))),
    Instruction('BXJ<c> <Rm>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, Rm, 1, 0, (0), 0, (1), (1), (1), (1), (0), (0), (0), (0), (0), (0), (0), (0))),
    Instruction('CBZ <Rn>, <label>', (1, 0, 1, 1, 0, 0, imm1_6, 1, imm5_1, Rn3)),
    Instruction('CBNZ <Rn>, <label>', (1, 0, 1, 1, 1, 0, imm1_6, 1, imm5_1, Rn3)),
    Instruction('CLREX<c>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 0, 1, 0, (1), (1), (1), (1))),
    Instruction('CLZ<c> <Rd>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm)),
    Instruction('CMN<c> <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 0, 0, 0, 1, Rn, 0, imm3_8, 1, 1, 1, 1, imm8)),
    Instruction('CMN<c> <Rn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 0, 1, 1, Rm3, Rn3)),
    Instruction('CMN<c>.W', (1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, Rn, (0), imm3_2, 1, 1, 1, 1, imm2, typ, Rm)),
    Instruction('CMP<c> <Rn3>, #<imm8>', (0, 0, 1, 0, 1, Rn3, imm8)),
    Instruction('CMP<c>.W <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 1, 0, 1, 1, Rn, 0, imm3_8, 1, 1, 1, 1, imm8)),
    Instruction('CMP<c> <Rn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 0, 1, 0, Rm3, Rn3)),
    Instruction('CMP<c> <Rn>, <Rm>', (0, 1, 0, 0, 0, 1, 0, 1, N, Rm, Rn3)),
    Instruction('CMP<c>.W <Rn>, <Rm> {, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, Rn, (0), imm3_2, 1, 1, 1, 1, imm2, typ, Rm)),
    Instruction('DBG<c> #<option>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 1, 1, 1, 1, option)),
    Instruction('DMB<c> <option>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 0, 1, option)),
    Instruction('DSB<c> <option>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 0, 0, option)),
    Instruction('EOR{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 1, 0, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('EOR{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 0, 0, 1, Rm3, Rdn3)),
    Instruction('EOR{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('ISB<c> <option>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 1, 0, option)),
    Instruction('IT{<x>{<y>{<z>}}} <firstcond>', (1, 0, 1, 1, 1, 1, 1, 1, firstcond, it_mask)),
    Instruction('LDM<c> <Rn>{!}, <registers>', (1, 1, 0, 0, 1, Rn3, register_list8)),
    Instruction('LDM<c>.W <Rn>{!}, <registers>', (1, 1, 1, 0, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list)),
    Instruction('LDMDB<c> <Rn>{!}, <registers>', (1, 1, 1, 0, 1, 0, 0, 1, 0, 0, W, 1, Rn, register_list)),
    Instruction('LDR<c> <Rt3>, [<Rn3>{, #<imm>}]', (0, 1, 1, 0, 1, imm5, Rn3, Rt3)),
    Instruction('LDR<c> <Rt3>, [SP{, #<imm>}]', (1, 0, 0, 1, 1, Rt3, imm8)),
    Instruction('LDR<c>.W <Rt>, [<Rn>{, #<imm12>}]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, imm12)),
    Instruction('LDR<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 1, P, U, W, imm8)),
    Instruction('LDR<c> <Rt3>, <label>', (0, 1, 0, 0, 1, Rt3, imm8)),
    Instruction('LDR<c>.W <Rt>, <label>', (1, 1, 1, 1, 1, 0, 0, 0, U, 1, 0, 1, 1, 1, 1, 1, Rt, imm12)),
    Instruction('LDR<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 1, 0, 0, Rm3, Rn3, Rt3)),
    Instruction('LDR<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('LDRB<c> <Rt3>, [<Rn3>{, #<imm5>}]', (0, 1, 1, 1, 1, imm5, Rn3, Rt3)),
    Instruction('LDRB<c>.W <Rt>, [<Rn>{, #<imm12>}]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, Rn, Rt, imm12)),
    Instruction('LDRB<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 1, P, U, W, imm8)),
    # Instruction('LDRB<c> <Rt>, <label>', (1, 1, 1, 1, 1, 0, 0, 0, U, 0, 0, 1, 1, 1, 1, 1, Rt, imm12)),
    Instruction('LDRB<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 1, 1, 0, Rm3, Rn3, Rt3)),
    Instruction('LDRB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('LDRBT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('LDRD<c> <Rt>, <Rt2>, [<Rn>{, #+/-<imm>}]', (1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 1, Rn, Rt, Rt2, imm8)),
    # Instruction('LDRD<c> <Rt>, <Rt2>, <label>', (1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 1, 1, 1, 1, 1, Rt, Rt2, imm8)),
    Instruction('LDREX<c> <Rt>, [<Rn>{, #<imm>}]', (1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), imm8)),
    Instruction('LDREXB<c> <Rt>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 0, (1), (1), (1), (1))),
    Instruction('LDREXD<c> <Rt>, <Rt2>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, Rt2, 0, 1, 1, 1, (1), (1), (1), (1))),
    Instruction('LDREXH<c> <Rt>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 1, (1), (1), (1), (1))),
    Instruction('LDRH<c> <Rt3>, [<Rn3>{, #<imm>}]', (1, 0, 0, 0, 1, imm5, Rn3, Rt3)),
    Instruction('LDRH<c>.W <Rt>, [<Rn>{, #<imm12>}]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRH<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 1, P, U, W, imm8)),
    # Instruction('LDRH<c> <Rt>, <label>', (1, 1, 1, 1, 1, 0, 0, 0, U, 0, 1, 1, 1, 1, 1, 1, Rt, imm12)),
    Instruction('LDRH<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 1, 0, 1, Rm3, Rn3, Rt3)),
    Instruction('LDRH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('LDRHT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('LDRSB<c> <Rt>, [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, imm12)),
    Instruction('LDRSB<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 1, P, U, W, imm8)),
    # Instruction('LDRSB<c> <Rt>, <label>', (1, 1, 1, 1, 1, 0, 0, 1, U, 0, 0, 1, 1, 1, 1, 1, Rt, imm12)),
    Instruction('LDRSB<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 0, 1, 1, Rm3, Rn3, Rt3)),
    Instruction('LDRSB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('LDRSBT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('LDRSH<c> <Rt>, [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, imm12)),
    Instruction('LDRSH<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 1, P, U, W, imm8)),
    # Instruction('LDRSH<c> <Rt>, <label>', (1, 1, 1, 1, 1, 0, 0, 1, U, 0, 1, 1, 1, 1, 1, 1, Rt, imm12)),
    Instruction('LDRSH<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 1, 1, 1, Rm3, Rn3, Rt3)),
    Instruction('LDRSH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('LDRSHT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('LDRT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('LSL{S}<c> <Rd3>, <Rm3>, #<imm5>', (0, 0, 0, 0, 0, imm5, Rm3, Rd3)),
    Instruction('LSL{S}<c>.W <Rd>, <Rm>, #<imm5>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3_2, Rd, imm2, 0, 0, Rm)),
    Instruction('LSL{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 0, 1, 0, Rm3, Rdn3)),
    Instruction('LSL{S}<c>.W <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('LSR{S}<c> <Rd3>, <Rm3>, #<imm>', (0, 0, 0, 0, 1, imm5, Rm3, Rd3)),
    Instruction('LSR{S}<c>.W <Rd>, <Rm>, #<imm>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3_2, Rd, imm2, 0, 1, Rm)),
    Instruction('LSR{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 0, 1, 1, Rm3, Rdn3)),
    Instruction('LSR{S}<c>.W <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('MLA<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, Ra, Rd, 0, 0, 0, 0, Rm)),
    Instruction('MLS<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, Ra, Rd, 0, 0, 0, 1, Rm)),
    Instruction('MOV{S}<c> <Rd3>, #<imm8>', (0, 0, 1, 0, 0, Rd3, imm8)),
    Instruction('MOV{S}<c>.W <Rd>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 1, 0, S, 1, 1, 1, 1, 0, imm3_8, Rd, imm8)),
    Instruction('MOVW<c> <Rd>, #<imm16>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 0, 1, 0, 0, imm4_12, 0, imm3_8, Rd, imm8)),
    Instruction('MOV<c> <Rd>, <Rm>', (0, 1, 0, 0, 0, 1, 1, 0, D, Rm, Rd3)),
    Instruction('MOV{S} <Rd3>, <Rm3>', (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Rm3, Rd3)),
    Instruction('MOV{S}<c>.W <Rd>, <Rm>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), 0, 0, 0, Rd, 0, 0, 0, 0, Rm)),
    Instruction('MOVT<c> <Rd>, #<imm16>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 1, 1, 0, 0, imm4_12, 0, imm3_8, Rd, imm8)),
    Instruction('MRS<c> <Rd>, <spec_reg>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, Rd, (0), (0), 0, (0), (0), (0), (0), (0))),
    Instruction('MSR<c> <spec_reg>, <Rn>', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, Rn, 1, 0, (0), 0, msr_mask, 0, 0, (0), (0), 0, (0), (0), (0), (0), (0))),
    Instruction('MUL{S}<c> <Rdm3>, <Rn3>, <Rdm3>', (0, 1, 0, 0, 0, 0, 1, 1, 0, 1, Rn3, Rdm3)),
    Instruction('MUL<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('MVN{S}<c> <Rd>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 1, 1, S, 1, 1, 1, 1, 0, imm3_8, Rd, imm8)),
    Instruction('MVN{S}<c> <Rd3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 1, 1, 1, Rm3, Rd3)),
    Instruction('MVN{S}<c>.W <Rd>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, S, 1, 1, 1, 1, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('NOP<c>', (1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0)),
    Instruction('NOP<c>.W', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)),
    Instruction('ORN{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 1, 1, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('ORN{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('ORR{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 1, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('ORR{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 1, 0, 0, Rm3, Rdn3)),
    Instruction('ORR{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('PKHBT<c> <Rd>, <Rn>, <Rm>{, LSL #<imm>}', (1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, Rn, (0), imm3_2, Rd, imm2, 0, 0, Rm)),
    Instruction('PKHTB<c> <Rd>, <Rn>, <Rm>{, LSL #<imm>}', (1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, Rn, (0), imm3_2, Rd, imm2, 1, 0, Rm)),
    Instruction('PLD{W}<c> [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 0, W, 1, Rn, 1, 1, 1, 1, imm12)),
    Instruction('PLD{W}<c> [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, W, 1, Rn, 1, 1, 1, 1, 1, 1, 0, 0, imm8)),
    Instruction('PLD<c> <label>', (1, 1, 1, 1, 1, 0, 0, 0, U, 0, (0), 1, 1, 1, 1, 1, 1, 1, 1, 1, imm12)),
    Instruction('PLD{W}<c> [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, W, 1, Rn, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('PLI<c> [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, Rn, 1, 1, 1, 1, imm12)),
    Instruction('PLI<c> [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, 1, 1, 0, 0, imm8)),
    Instruction('PLI<c> <label>', (1, 1, 1, 1, 1, 0, 0, 1, U, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, imm12)),
    Instruction('PLI<c> [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('POP<c> <registers>', (1, 0, 1, 1, 1, 1, 0, P, register_list8)),
    Instruction('POP<c>.W <registers>', (1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, register_list)),
    Instruction('POP<c>.W <registers>', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, Rt, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('PUSH<c> <registers>', (1, 0, 1, 1, 0, 1, 0, M, register_list8)),
    Instruction('PUSH<c>.W <registers>', (1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, register_list)),
    Instruction('PUSH<c>.W <registers>', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, Rt, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('QADD<c> <Rd>, <Rm>, <Rn>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm)),
    Instruction('QADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('QADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('QASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('QDADD<c> <Rd>, <Rm>, <Rn>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 1, Rm)),
    Instruction('QDSUB<c> <Rd>, <Rm>, <Rn>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 1, 1, Rm)),
    Instruction('QSAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('QSUB<c> <Rd>, <Rm>, <Rn>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 1, 0, Rm)),
    Instruction('QSUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('QSUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm)),
    Instruction('RBIT<c> <Rd>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 1, 0, Rm)),
    Instruction('REV<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 1, 0, 1, 0, 0, 0, Rm3, Rd3)),
    Instruction('REV<c>.W <Rd>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm)),
    Instruction('REV16<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 1, 0, 1, 0, 0, 1, Rm3, Rd3)),
    Instruction('REV16<c>.W <Rd>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 1, Rm)),
    Instruction('REVSH<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 1, 0, 1, 0, 1, 1, Rm3, Rd3)),
    Instruction('REVSH<c>.W <Rd>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 1, 1, Rm)),
    Instruction('ROR{S}<c> <Rd>, <Rm>, #<imm>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3_2, Rd, imm2, 1, 1, Rm)),
    Instruction('ROR{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 1, 1, 1, Rm3, Rdn3)),
    Instruction('ROR{S}<c>.W <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('RRX{S}<c> <Rd>, <Rm>', (1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), 0, 0, 0, Rd, 0, 0, 1, 1, Rm)),
    Instruction('RSB{S} <Rd3>, <Rn3>, #0', (0, 1, 0, 0, 0, 0, 1, 0, 0, 1, Rn3, Rd3)),
    Instruction('RSB{S}<c>.W <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 1, 1, 0, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('RSB{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('SADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('SADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('SASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('SBC{S}<c> <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 0, 1, 1, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('SBC{S}<c> <Rdn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 0, 1, 1, 0, Rm3, Rdn3)),
    Instruction('SBC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('SBFX<c> <Rd>, <Rn>, #<lsb>, #<width>', (1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 0, 0, Rn, 0, imm3_2, Rd, imm2, (0), widthm1)),
    Instruction('SDIV<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, Rn, (1), (1), (1), (1), Rd, 1, 1, 1, 1, Rm)),
    Instruction('SEL<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm)),
    Instruction('SETEND <endian_specifier>', (1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, (1), E, (0), (0), (0))),
    Instruction('SEV<c>', (1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0)),
    Instruction('SEV<c>.W', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0)),
    Instruction('SHADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SHADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SHASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SHSAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SHSUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SHSUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm)),
    Instruction('SMLA<x><y><c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, Rn, Ra, Rd, 0, 0, N, M, Rm)),
    Instruction('SMLAD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, Rn, Ra, Rd, 0, 0, 0, M, Rm)),
    Instruction('SMLAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm)),
    Instruction('SMLAL<x><y><c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 1, 0, N, M, Rm)),
    Instruction('SMLALD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 1, 1, 0, M, Rm)),
    Instruction('SMLAW<y><c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, Rn, Ra, Rd, 0, 0, 0, M, Rm)),
    Instruction('SMLSD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, Rn, Ra, Rd, 0, 0, 0, M, Rm)),
    Instruction('SMLSLD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, Rn, RdLo, RdHi, 1, 1, 0, M, Rm)),
    Instruction('SMMLA{R}<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, Rn, Ra, Rd, 0, 0, 0, R, Rm)),
    Instruction('SMMLS{R}<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, Rn, Ra, Rd, 0, 0, 0, R, Rm)),
    Instruction('SMMUL{R}<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, R, Rm)),
    Instruction('SMUAD{X}<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm)),
    Instruction('SMUL<x><y><c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, N, M, Rm)),
    Instruction('SMULL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm)),
    Instruction('SMULW<y><c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm)),
    Instruction('SMUSD{X}<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm)),
    Instruction('SSAT<c> <Rd>, #<imm>, <Rn>{, <shift>}', (1, 1, 1, 1, 0, (0), 1, 1, 0, 0, sh, 0, Rn, 0, imm3_2, Rd, imm2, (0), sat_imm5)),
    Instruction('SSAT16<c> <Rd>, #<imm>, <Rn>', (1, 1, 1, 1, 0, (0), 1, 1, 0, 0, 1, 0, Rn, 0, 0, 0, 0, Rd, 0, 0, (0), (0), sat_imm4)),
    Instruction('SSAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('SSUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('SSUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('STM<c> <Rn3>!, <registers>', (1, 1, 0, 0, 0, Rn3, register_list8)),
    Instruction('STM<c>.W <Rn>{!}, <registers>', (1, 1, 1, 0, 1, 0, 0, 0, 1, 0, W, 0, Rn, register_list)),
    Instruction('STMDB<c> <Rn>{!}, <registers>', (1, 1, 1, 0, 1, 0, 0, 1, 0, 0, W, 0, Rn, register_list)),
    Instruction('STR<c> <Rt3>, [<Rn3>{, #<imm>}]', (0, 1, 1, 0, 0, imm5, Rn3, Rt3)),
    Instruction('STR<c> <Rt3>, [SP, #<imm>]', (1, 0, 0, 1, 0, Rt3, imm8)),
    Instruction('STR<c>.W <Rt>, [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, imm12)),
    Instruction('STR<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 1, P, U, W, imm8)),
    Instruction('STR<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 0, 0, 0, Rm3, Rn3, Rt3)),
    Instruction('STR<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('STRB<c> <Rt3>, [<Rn3>, #<imm5>]', (0, 1, 1, 1, 0, imm5, Rn3, Rt3)),
    Instruction('STRB<c>.W <Rt>, [<Rn>, #<imm12>]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, Rn, Rt, imm12)),
    Instruction('STRB<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 1, P, U, W, imm8)),
    Instruction('STRB<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 0, 1, 0, Rm3, Rn3, Rt3)),
    Instruction('STRB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('STRBT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('STRD<c> <Rt>, <Rt2>, [<Rn>{, #+/-<imm>}]', (1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 0, Rn, Rt, Rt2, imm8)),
    Instruction('STREX<c> <Rd>, <Rt>, [<Rn>{, #<imm>}]', (1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, Rd, imm8)),
    Instruction('STREXB<c> <Rd>, <Rt>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 0, Rd)),
    Instruction('STREXD<c> <Rd>, <Rt>, <Rt2>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, Rt2, 0, 1, 1, 1, Rd)),
    Instruction('STREXH<c> <Rd>, <Rt>, [<Rn>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 1, Rd)),
    Instruction('STRH<c> <Rt3>, [<Rn3>{, #<imm>}]', (1, 0, 0, 0, 0, imm5, Rn3, Rt3)),
    Instruction('STRH<c>.W <Rt>, [<Rn>{, #<imm12>}]', (1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, Rn, Rt, imm12)),
    Instruction('STRH<c> <Rt>, [<Rn>, #-<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 1, P, U, W, imm8)),
    Instruction('STRH<c> <Rt3>, [<Rn3>, <Rm3>]', (0, 1, 0, 1, 0, 0, 1, Rm3, Rn3, Rt3)),
    Instruction('STRH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm)),
    Instruction('STRHT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('STRT<c> <Rt>, [<Rn>, #<imm8>]', (1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 1, 1, 1, 0, imm8)),
    Instruction('SUB{S}<c> <Rd3>, <Rn3>, #<imm3>', (0, 0, 0, 1, 1, 1, 1, imm3, Rn3, Rd3)),
    Instruction('SUB{S}<c> <Rdn3>, #<imm8>', (0, 0, 1, 1, 1, Rdn3, imm8)),
    Instruction('SUB{S}<c>.W <Rd>, <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 1, 1, 0, 1, S, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('SUBW<c> <Rd>, <Rn>, #<imm12>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 1, 0, 1, 0, Rn, 0, imm3_8, Rd, imm8)),
    Instruction('SUB{S}<c> <Rd3>, <Rn3>, <Rm3>', (0, 0, 0, 1, 1, 0, 1, Rm3, Rn3, Rd3)),
    Instruction('SUB{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, S, Rn, (0), imm3_2, Rd, imm2, typ, Rm)),
    Instruction('SUB<c> SP, SP, #<imm>', (1, 0, 1, 1, 0, 0, 0, 0, 1, imm7)),
    # Instruction('SUB{S}<c>.W <Rd>, SP, #<const>', (1, 1, 1, 1, 0, imm1, 0, 1, 1, 0, 1, S, 1, 1, 0, 1, 0, imm3, Rd, imm8)),
    Instruction('SUBW<c> <Rd>, SP, #<imm12>', (1, 1, 1, 1, 0, imm1_11, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, imm3_8, Rd, imm8)),
    # Instruction('SUB{S}<c> <Rd>, SP, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, S, 1, 1, 0, 1, (0), imm3, Rd, imm2, typ, Rm)),
    Instruction('SVC<c> #<imm8>', (1, 1, 0, 1, 1, 1, 1, 1, imm8)),
    Instruction('SXTAB<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('SXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('SXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('SXTB<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 0, 0, 1, 0, 0, 1, Rm3, Rd3)),
    Instruction('SXTB<c>.W <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('SXTB16<c> <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('SXTH<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 0, 0, 1, 0, 0, 0, Rm3, Rd3)),
    Instruction('SXTH<c>.W <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    # Instruction('TBB<c> [<Rn>, <Rm>]', (1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, H, Rm)),
    Instruction('TEQ<c> <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 1, 0, 0, 1, Rn, 0, imm3_8, 1, 1, 1, 1, imm8)),
    Instruction('TEQ<c> <Rn>, <Rm>{, <shift>}', (1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, Rn, (0), imm3_2, 1, 1, 1, 1, imm2, typ, Rm)),
    Instruction('TST<c> <Rn>, #<const>', (1, 1, 1, 1, 0, imm1_11, 0, 0, 0, 0, 0, 1, Rn, 0, imm3_8, 1, 1, 1, 1, imm8)),
    Instruction('TST<c> <Rn3>, <Rm3>', (0, 1, 0, 0, 0, 0, 1, 0, 0, 0, Rm3, Rn3)),
    Instruction('TST<c>.W', (1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, Rn, (0), imm3_2, 1, 1, 1, 1, imm2, typ, Rm)),
    Instruction('UADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('UADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('UASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('UBFX<c> <Rd>, <Rn>, #<lsb>, #<width>', (1, 1, 1, 1, 0, (0), 1, 1, 1, 1, 0, 0, Rn, 0, imm3_2, Rd, imm2, (0), widthm1)),
    Instruction('UDF<c> #<imm8>', (1, 1, 0, 1, 1, 1, 1, 0, imm8)),
    Instruction('UDF<c>.W #<imm16>', (1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, imm4_12, 1, 0, 1, 0, imm12)),
    Instruction('UDIV<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, Rn, (1), (1), (1), (1), Rd, 1, 1, 1, 1, Rm)),
    Instruction('UHADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UHADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UHASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UHSAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UHSUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UHSUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm)),
    Instruction('UMAAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, Rn, RdLo, RdHi, 0, 1, 1, 0, Rm)),
    Instruction('UMLAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm)),
    Instruction('UMULL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm)),
    Instruction('UQADD16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('UQADD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('UQASX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('UQSAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('UQSUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('UQSUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm)),
    Instruction('USAD8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm)),
    Instruction('USADA8<c> <Rd>, <Rn>, <Rm>, <Ra>', (1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, Rn, Ra, Rd, 0, 0, 0, 0, Rm)),
    Instruction('USAT<c> <Rd>, #<imm5>, <Rn>{, <shift>}', (1, 1, 1, 1, 0, (0), 1, 1, 1, 0, sh, 0, Rn, 0, imm3_2, Rd, imm2, (0), sat_imm5)),
    Instruction('USAT16<c> <Rd>, #<imm4>, <Rn>', (1, 1, 1, 1, 0, (0), 1, 1, 1, 0, 1, 0, Rn, 0, 0, 0, 0, Rd, 0, 0, (0), (0), sat_imm4)),
    Instruction('USAX<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('USUB16<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('USUB8<c> <Rd>, <Rn>, <Rm>', (1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm)),
    Instruction('UXTAB<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('UXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('UXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('UXTB<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 0, 0, 1, 0, 1, 1, Rm3, Rd3)),
    Instruction('UXTB<c>.W <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('UXTB16<c> <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('UXTH<c> <Rd3>, <Rm3>', (1, 0, 1, 1, 0, 0, 1, 0, 1, 0, Rm3, Rd3)),
    Instruction('UXTH<c>.W <Rd>, <Rm>{, <rotation>}', (1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm)),
    Instruction('WFE<c>', (1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0)),
    Instruction('WFE<c>.W', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0)),
    Instruction('WFI<c>', (1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0)),
    Instruction('WFI<c>.W', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1)),
    Instruction('YIELD<c>', (1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0)),
    Instruction('YIELD<c>.W', (1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1)),
]

table = ThumbTable(_table, 32)
