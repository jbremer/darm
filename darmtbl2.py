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


class Bitsize:
    def __init__(self, name, bitsize, comment):
        self.name = name
        self.bitsize = bitsize
        self.comment = comment

    def __repr__(self):
        return '<%s:%d>' % (self.name, self.bitsize)


cond          = Bitsize('cond', 4, 'Conditional Flags')
Rd            = Bitsize('Rd', 4, 'Destination Register')
Rd3           = Bitsize('Rd3', 3, 'Destination Register')
Rs            = Bitsize('Rs', 4, 'Shift Immediate')
Rn            = Bitsize('Rn', 4, 'N Register')
Rn3           = Bitsize('Rn3', 3, 'N Register')
Rm            = Bitsize('Rm', 4, 'Shift Register')
Rm3           = Bitsize('Rm3', 3, 'Shift Register')
Rt            = Bitsize('Rt', 4, 'Transferred Register')
Rt3           = Bitsize('Rt3', 3, 'Transferred Register')
Rt2           = Bitsize('Rt2', 4, 'Second Ternary Register')
Ra            = Bitsize('Ra', 4, 'Accumulate Register')
Rdm           = Bitsize('Rdm', 4, 'Destination & M Register')
Rdm3          = Bitsize('Rdm3', 3, 'Destination & M Register')
Rdn           = Bitsize('Rdn', 4, 'Destination & N Register')
Rdn3          = Bitsize('Rdn3', 3, 'Destination & N Register')
S             = Bitsize('S', 1, 'Update Conditional Flags')
type_         = Bitsize('type', 2, 'Shift Type')
msb           = Bitsize('msb', 5, 'Most Significant Bit')
lsb           = Bitsize('lsb', 5, 'Least Significant Bit')
register_list = Bitsize('register_list', 16, 'Register List')
register_list8 = Bitsize('register_list8', 8, 'Register List')
E             = Bitsize('E', 1, 'Endian Specifier')
msr           = Bitsize('msr', 2, 'Move to Special Register mask')
rotate        = Bitsize('rotate', 2, 'Rotation Type')
H             = Bitsize('H', 1, 'Sign Extension Bit for BL(X)')
option        = Bitsize('option', 4, 'Option for Debug Hint')
W             = Bitsize('W', 1, 'Some Bit for LDM')
widthm1       = Bitsize('widthm1', 5, 'Bit Width Minus One')
M             = Bitsize('M', 1, 'High 16bits for Rm')
N             = Bitsize('N', 1, 'High 16bits for Rn')
RdHi          = Bitsize('RdHi', 4, 'High 32bits for Rd')
RdLo          = Bitsize('RdLo', 4, 'Low 32bits for Rd')
R             = Bitsize('R', 1, 'Round Integer')
sat_imm4      = Bitsize('sat_imm4', 4, 'Saturate Immediate')
sat_imm5      = Bitsize('sat_imm5', 5, 'Saturate Immediate')
sh            = Bitsize('sh', 1, 'Immediate Shift Value')
opc1          = Bitsize('opc1', 4, 'Coprocessor Operation Code')
opc2          = Bitsize('opc2', 3, 'Coprocessor Information')
CRn           = Bitsize('CRn', 4, 'Coprocessor Operand Register')
CRd           = Bitsize('CRd', 4, 'Coprocessor Destination Register')
coproc        = Bitsize('coproc', 4, 'Coprocessor Number')
CPOpc         = Bitsize('CPOpc', 3, 'Coprocessor Operation Mode')
CRm           = Bitsize('CRm', 4, 'Coprocessor Operand Register')
U             = Bitsize('U', 1, 'Addition flag for PLD')
P             = Bitsize('P', 1, 'Protected Mode Flag?')
D             = Bitsize('D', 1, 'User-defined bit')
tb            = Bitsize('tb', 1, 'Is PKH in TB form or not?')
imm4H         = Bitsize('imm4H', 4, 'High Word Register')
imm4L         = Bitsize('imm4L', 4, 'Low Word Register')
op            = Bitsize('op', 1, 'Operation for CB{N}Z')
firstcond     = Bitsize('firstcond', 4, 'First Condition for IT')
mask          = Bitsize('mask', 4, 'Mask for IT')

i             = Bitsize('imm1', 1, 'Immediate')
J1            = Bitsize('J1', 1, 'Immediate')
J2            = Bitsize('J2', 1, 'Immediate')
imm2          = Bitsize('imm2', 2, 'Immediate')
imm3          = Bitsize('imm3', 3, 'Immediate')
imm4          = Bitsize('imm4', 4, 'Immediate')
imm5          = Bitsize('imm5', 5, 'Immediate')
imm6          = Bitsize('imm6', 6, 'Immediate')
imm7          = Bitsize('imm7', 7, 'Immediate')
imm8          = Bitsize('imm8', 8, 'Immediate')
imm10         = Bitsize('imm10', 10, 'Immediate')
imm10H        = Bitsize('imm10H', 10, 'Immediate')
imm10L        = Bitsize('imm10L', 10, 'Immediate')
imm11         = Bitsize('imm11', 11, 'Immediate')
imm12         = Bitsize('imm12', 12, 'Immediate')
imm24         = Bitsize('imm24', 24, 'Immediate')


thumbs = [
    ('ADC{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 0, 1, 0, S, Rn, 0, imm3, Rd, imm8),
    ('ADC{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, Rm3, Rdn3),
    ('ADC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ADD{S}<c> <Rd3>, <Rn3>, #<imm3>', 0, 0, 0, 1, 1, 1, 0, imm3, Rn3, Rd3),
    ('ADD{S}<c> <Rdn3>, #<imm8>', 0, 0, 1, 1, 0, Rdn3, imm8),
    ('ADD{S}<c>.W <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 0, 0, 0, S, Rn, 0, imm3, Rd, imm8),
    ('ADDW<c> <Rd>, <Rn>, #<imm12>', 1, 1, 1, 1, 0, i, 1, 0, 0, 0, 0, 0, Rn, 0, imm3, Rd, imm8),
    ('ADD{S}<c> <Rd>, <Rn>, <Rm>', 0, 0, 0, 1, 1, 0, 0, Rm3, Rn3, Rd3),
    # ('ADD<c> <Rdn>, <Rm>', 0, 1, 0, 0, 0, 1, 0, 0, Rm, Rdn, DN),
    ('ADD{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ADD{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ADD<c> <Rd3>, SP, #<imm>', 1, 0, 1, 0, 1, Rd3, imm8),
    ('ADD<c> SP, SP, #<imm>', 1, 0, 1, 1, 0, 0, 0, 0, 0, imm7),
    ('ADD{S}<c>.W <Rd>, SP, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 0, 0, 0, S, 1, 1, 0, 1, 0, imm3, Rd, imm8),
    ('ADDW<c> <Rd>, SP, #<imm12>', 1, 1, 1, 1, 0, i, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, imm3, Rd, imm8),
    # ('ADD<c> <Rdm>, SP, <Rdm>', 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, Rdm, DM),
    ('ADD<c> SP, <Rm>', 0, 1, 0, 0, 0, 1, 0, 0, 1, Rm, 1, 0, 1),
    ('ADD{S}<c>.W <Rd>, SP, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, 1, 1, 0, 1, (0), imm3, Rd, imm2, type_, Rm),
    ('ADD{S}<c>.W <Rd>, SP, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, S, 1, 1, 0, 1, (0), imm3, Rd, imm2, type_, Rm),
    ('ADR<c> <Rd3>, <label>', 1, 0, 1, 0, 0, Rd3, imm8),
    ('ADR<c>.W <Rd>, <label>', 1, 1, 1, 1, 0, i, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, imm3, Rd, imm8),
    ('ADR<c>.W <Rd>, <label>', 1, 1, 1, 1, 0, i, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, imm3, Rd, imm8),
    ('AND{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 0, 0, S, Rn, 0, imm3, Rd, imm8),
    ('AND{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, Rm3, Rdn3),
    ('AND{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ASR{S}<c> <Rd3>, <Rm3>, #<imm>', 0, 0, 0, 1, 0, imm5, Rm3, Rd3),
    ('ASR{S}<c>.W <Rd>, <Rm>, #<imm>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3, Rd, imm2, 1, 0, Rm),
    ('ASR{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, Rm3, Rdn3),
    ('ASR{S}<c>.W <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('B<c> <label>', 1, 1, 0, 1, cond, imm8),
    ('B<c> <label>', 1, 1, 1, 0, 0, imm11),
    ('B<c>.W <label>', 1, 1, 1, 1, 0, S, cond, imm6, 1, 0, J1, 0, J2, imm11),
    ('B<c>.W <label>', 1, 1, 1, 1, 0, S, imm10, 1, 0, J1, 1, J2, imm11),
    ('BFC<c> <Rd>, #<lsb>, #<width>', 1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, imm3, Rd, imm2, (0), msb),
    ('BFI<c> <Rd>, <Rn>, #<lsb>, #<width>', 1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 1, 0, Rn, 0, imm3, Rd, imm2, (0), msb),
    ('BIC{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 0, 1, S, Rn, 0, imm3, Rd, imm8),
    ('BIC{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, Rm3, Rdn3),
    ('BIC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('BKPT #<imm8>', 1, 0, 1, 1, 1, 1, 1, 0, imm8),
    ('BL<c> <label>', 1, 1, 1, 1, 0, S, imm10, 1, 1, J1, 1, J2, imm11),
    ('BLX<c> <label>', 1, 1, 1, 1, 0, S, imm10H, 1, 1, J1, 0, J2, imm10L, H),
    ('BLX<c> <Rm>', 0, 1, 0, 0, 0, 1, 1, 1, 1, Rm, (0), (0), (0)),
    ('BX<c> <Rm>', 0, 1, 0, 0, 0, 1, 1, 1, 0, Rm, (0), (0), (0)),
    ('BXJ<c> <Rm>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, Rm, 1, 0, (0), 0, (1), (1), (1), (1), (0), (0), (0), (0), (0), (0), (0), (0)),
    # CBZ or CBNZ
    ('CBZ <Rn>, <label>', 1, 0, 1, 1, op, 0, i, 1, imm5, Rn3),
    ('CLREX<c>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 0, 1, 0, (1), (1), (1), (1)),
    ('CLZ<c> <Rd>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm),
    ('CMN<c> <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 0, 0, 0, 1, Rn, 0, imm3, 1, 1, 1, 1, imm8),
    ('CMN<c> <Rn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, Rm3, Rn3),
    ('CMN<c>.W', 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, Rn, (0), imm3, 1, 1, 1, 1, imm2, type_, Rm),
    ('CMP<c> <Rn3>, #<imm8>', 0, 0, 1, 0, 1, Rn3, imm8),
    ('CMP<c>.W <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 1, 0, 1, 1, Rn, 0, imm3, 1, 1, 1, 1, imm8),
    ('CMP<c> <Rn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, Rm3, Rn3),
    ('CMP<c> <Rn>, <Rm>', 0, 1, 0, 0, 0, 1, 0, 1, N, Rm, Rn3),
    ('CMP<c>.W <Rn>, <Rm> {, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, Rn, (0), imm3, 1, 1, 1, 1, imm2, type_, Rm),
    ('DBG<c> #<option>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 1, 1, 1, 1, option),
    ('DMB<c> <option>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 0, 1, option),
    ('DSB<c> <option>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 0, 0, option),
    ('EOR{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 1, 0, 0, S, Rn, 0, imm3, Rd, imm8),
    ('EOR{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, Rm3, Rdn3),
    ('EOR{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ISB<c> <option>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, (1), (1), (1), (1), 1, 0, (0), 0, (1), (1), (1), (1), 0, 1, 1, 0, option),
    ('IT{<x>{<y>{<z>}}} <firstcond>', 1, 0, 1, 1, 1, 1, 1, 1, firstcond, mask),
    ('LDM<c> <Rn>{!}, <registers>', 1, 1, 0, 0, 1, Rn3, register_list8),
    ('LDM<c>.W <Rn>{!}, <registers>', 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list),
    ('LDM<c>.W <Rn>{!}, <registers>', 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list),
    ('LDMDB<c> <Rn>{!}, <registers>', 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, W, 1, Rn, register_list),
    ('LDR<c> <Rt3>, [<Rn3>{, #<imm>}]', 0, 1, 1, 0, 1, imm5, Rn3, Rt3),
    ('LDR<c> <Rt3>, [SP{, #<imm>}]', 1, 0, 0, 1, 1, Rt3, imm8),
    ('LDR<c>.W <Rt>, [<Rn>{, #<imm12>}]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, imm12),
    ('LDR<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDR<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDR<c> <Rt3>, <label>', 0, 1, 0, 0, 1, Rt3, imm8),
    ('LDR<c>.W <Rt>, <label>', 1, 1, 1, 1, 1, 0, 0, 0, U, 1, 0, 1, 1, 1, 1, 1, Rt, imm12),
    ('LDR<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 1, 0, 0, Rm3, Rn3, Rt3),
    ('LDR<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDR<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDRB<c> <Rt3>, [<Rn3>{, #<imm5>}]', 0, 1, 1, 1, 1, imm5, Rn3, Rt3),
    ('LDRB<c>.W <Rt>, [<Rn>{, #<imm12>}]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, Rn, Rt, imm12),
    ('LDRB<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRB<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRB<c> <Rt>, <label>', 1, 1, 1, 1, 1, 0, 0, 0, U, 0, 0, 1, 1, 1, 1, 1, Rt, imm12),
    ('LDRB<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 1, 1, 0, Rm3, Rn3, Rt3),
    ('LDRB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDRBT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8),
    ('LDRD<c> <Rt>, <Rt2>, [<Rn>{, #+/-<imm>}]', 1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 1, Rn, Rt, Rt2, imm8),
    ('LDRD<c> <Rt>, <Rt2>, <label>', 1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 1, 1, 1, 1, 1, Rt, Rt2, imm8),
    ('LDREX<c> <Rt>, [<Rn>{, #<imm>}]', 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), imm8),
    ('LDREXB<c> <Rt>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 0, (1), (1), (1), (1)),
    ('LDREXD<c> <Rt>, <Rt2>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, Rt2, 0, 1, 1, 1, (1), (1), (1), (1)),
    ('LDREXH<c> <Rt>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 1, (1), (1), (1), (1)),
    ('LDRH<c> <Rt3>, [<Rn3>{, #<imm>}]', 1, 0, 0, 0, 1, imm5, Rn3, Rt3),
    ('LDRH<c>.W <Rt>, [<Rn>{, #<imm12>}]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, Rn, Rt, imm12),
    ('LDRH<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRH<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRH<c> <Rt>, <label>', 1, 1, 1, 1, 1, 0, 0, 0, U, 0, 1, 1, 1, 1, 1, 1, Rt, imm12),
    ('LDRH<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 1, 0, 1, Rm3, Rn3, Rt3),
    ('LDRH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDRHT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, Rn, Rt, 1, 1, 1, 0, imm8),
    ('LDRSB<c> <Rt>, [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, imm12),
    ('LDRSB<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRSB<c> <Rt>, <label>', 1, 1, 1, 1, 1, 0, 0, 1, U, 0, 0, 1, 1, 1, 1, 1, Rt, imm12),
    ('LDRSB<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 0, 1, 1, Rm3, Rn3, Rt3),
    ('LDRSB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDRSBT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8),
    ('LDRSH<c> <Rt>, [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, imm12),
    ('LDRSH<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 1, P, U, W, imm8),
    ('LDRSH<c> <Rt>, <label>', 1, 1, 1, 1, 1, 0, 0, 1, U, 0, 1, 1, 1, 1, 1, 1, Rt, imm12),
    ('LDRSH<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 1, 1, 1, Rm3, Rn3, Rt3),
    ('LDRSH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('LDRSHT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, Rn, Rt, 1, 1, 1, 0, imm8),
    ('LDRT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, Rn, Rt, 1, 1, 1, 0, imm8),
    ('LSL{S}<c> <Rd3>, <Rm3>, #<imm5>', 0, 0, 0, 0, 0, imm5, Rm3, Rd3),
    ('LSL{S}<c>.W <Rd>, <Rm>, #<imm5>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3, Rd, imm2, 0, 0, Rm),
    ('LSL{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, Rm3, Rdn3),
    ('LSL{S}<c>.W <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('LSR{S}<c> <Rd3>, <Rm3>, #<imm>', 0, 0, 0, 0, 1, imm5, Rm3, Rd3),
    ('LSR{S}<c>.W <Rd>, <Rm>, #<imm>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3, Rd, imm2, 0, 1, Rm),
    ('LSR{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, Rm3, Rdn3),
    ('LSR{S}<c>.W <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('MLA<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, Ra, Rd, 0, 0, 0, 0, Rm),
    ('MLS<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, Ra, Rd, 0, 0, 0, 1, Rm),
    ('MOV{S}<c> <Rd3>, #<imm8>', 0, 0, 1, 0, 0, Rd3, imm8),
    ('MOV{S}<c>.W <Rd>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 1, 0, S, 1, 1, 1, 1, 0, imm3, Rd, imm8),
    ('MOVW<c> <Rd>, #<imm16>', 1, 1, 1, 1, 0, i, 1, 0, 0, 1, 0, 0, imm4, 0, imm3, Rd, imm8),
    ('MOV<c> <Rd>, <Rm>', 0, 1, 0, 0, 0, 1, 1, 0, D, Rm, Rd3),
    # ('MOV{S} <Rd3>, <Rm3>', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Rm3, Rd3),
    ('MOV{S}<c>.W <Rd>, <Rm>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), 0, 0, 0, Rd, 0, 0, 0, 0, Rm),
    ('MOVT<c> <Rd>, #<imm16>', 1, 1, 1, 1, 0, i, 1, 0, 1, 1, 0, 0, imm4, 0, imm3, Rd, imm8),
    ('MRS<c> <Rd>, <spec_reg>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, Rd, (0), (0), 0, (0), (0), (0), (0), (0)),
    #('MSR<c> <spec_reg>, <Rn>', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, Rn, 1, 0, (0), 0, mask, 0, 0, (0), (0), 0, (0), (0), (0), (0), (0)),
    ('MUL{S}<c> <Rdm3>, <Rn3>, <Rdm3>', 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, Rn3, Rdm3),
    ('MUL<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('MVN{S}<c> <Rd>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 1, 1, S, 1, 1, 1, 1, 0, imm3, Rd, imm8),
    ('MVN{S}<c> <Rd3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, Rm3, Rd3),
    ('MVN{S}<c>.W <Rd>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, S, 1, 1, 1, 1, (0), imm3, Rd, imm2, type_, Rm),
    ('NOP<c>', 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0),
    ('NOP<c>.W', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    ('ORN{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 1, 1, S, Rn, 0, imm3, Rd, imm8),
    ('ORN{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('ORR{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 1, 0, S, Rn, 0, imm3, Rd, imm8),
    ('ORR{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, Rm3, Rdn3),
    ('ORR{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    #('PKHBT<c> <Rd>, <Rn>, <Rm>{, LSL #<imm>}', 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, S, Rn, (0), imm3, Rd, imm2, tb, T, Rm),
    ('PLD{W}<c> [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, W, 1, Rn, 1, 1, 1, 1, imm12),
    ('PLD{W}<c> [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, W, 1, Rn, 1, 1, 1, 1, 1, 1, 0, 0, imm8),
    ('PLD<c> <label>', 1, 1, 1, 1, 1, 0, 0, 0, U, 0, (0), 1, 1, 1, 1, 1, 1, 1, 1, 1, imm12),
    ('PLD{W}<c> [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, W, 1, Rn, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('PLI<c> [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, Rn, 1, 1, 1, 1, imm12),
    ('PLI<c> [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, 1, 1, 0, 0, imm8),
    ('PLI<c> <label>', 1, 1, 1, 1, 1, 0, 0, 1, U, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, imm12),
    ('PLI<c> [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('POP<c> <registers>', 1, 0, 1, 1, 1, 1, 0, P, register_list8),
    ('POP<c>.W <registers>', 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, register_list),
    ('POP<c>.W <registers>', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, Rt, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0),
    ('PUSH<c> <registers>', 1, 0, 1, 1, 0, 1, 0, M, register_list8),
    ('PUSH<c>.W <registers>', 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, register_list),
    ('PUSH<c>.W <registers>', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, Rt, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0),
    ('QADD<c> <Rd>, <Rm>, <Rn>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm),
    ('QADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('QADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('QASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('QDADD<c> <Rd>, <Rm>, <Rn>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 1, Rm),
    ('QDSUB<c> <Rd>, <Rm>, <Rn>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 1, 1, Rm),
    ('QSAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('QSUB<c> <Rd>, <Rm>, <Rn>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 1, 0, Rm),
    ('QSUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('QSUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 1, Rm),
    ('RBIT<c> <Rd>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 1, 0, Rm),
    ('REV<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, Rm3, Rd3),
    ('REV<c>.W <Rd>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm),
    ('REV16<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, Rm3, Rd3),
    ('REV16<c>.W <Rd>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 0, 1, Rm),
    ('REVSH<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, Rm3, Rd3),
    ('REVSH<c>.W <Rd>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rm, 1, 1, 1, 1, Rd, 1, 0, 1, 1, Rm),
    ('ROR{S}<c> <Rd>, <Rm>, #<imm>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), imm3, Rd, imm2, 1, 1, Rm),
    ('ROR{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, Rm3, Rdn3),
    ('ROR{S}<c>.W <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, S, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('RRX{S}<c> <Rd>, <Rm>', 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, S, 1, 1, 1, 1, (0), 0, 0, 0, Rd, 0, 0, 1, 1, Rm),
    ('RSB{S} <Rd3>, <Rn3>, #0', 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, Rn3, Rd3),
    ('RSB{S}<c>.W <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 1, 1, 0, S, Rn, 0, imm3, Rd, imm8),
    ('RSB{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('SADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('SADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('SASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('SBC{S}<c> <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 0, 1, 1, S, Rn, 0, imm3, Rd, imm8),
    ('SBC{S}<c> <Rdn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, Rm3, Rdn3),
    ('SBC{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('SBFX<c> <Rd>, <Rn>, #<lsb>, #<width>', 1, 1, 1, 1, 0, (0), 1, 1, 0, 1, 0, 0, Rn, 0, imm3, Rd, imm2, (0), widthm1),
    ('SDIV<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, Rn, (1), (1), (1), (1), Rd, 1, 1, 1, 1, Rm),
    ('SEL<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 1, 0, 0, 0, Rm),
    ('SETEND <endian_specifier>', 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, (1), E, (0), (0), (0)),
    ('SEV<c>', 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0),
    ('SEV<c>.W', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
    ('SHADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SHADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SHASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SHSAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SHSUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SHSUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 1, 0, Rm),
    ('SMLA<x><y><c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, Rn, Ra, Rd, 0, 0, N, M, Rm),
    ('SMLAD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, Rn, Ra, Rd, 0, 0, 0, M, Rm),
    ('SMLAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm),
    ('SMLAL<x><y><c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 1, 0, N, M, Rm),
    ('SMLALD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, Rn, RdLo, RdHi, 1, 1, 0, M, Rm),
    ('SMLAW<y><c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, Rn, Ra, Rd, 0, 0, 0, M, Rm),
    ('SMLSD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, Rn, Ra, Rd, 0, 0, 0, M, Rm),
    ('SMLSLD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, Rn, RdLo, RdHi, 1, 1, 0, M, Rm),
    ('SMMLA{R}<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, Rn, Ra, Rd, 0, 0, 0, R, Rm),
    ('SMMLS{R}<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, Rn, Ra, Rd, 0, 0, 0, R, Rm),
    ('SMMUL{R}<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, R, Rm),
    ('SMUAD{X}<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm),
    ('SMUL<x><y><c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, N, M, Rm),
    ('SMULL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm),
    ('SMULW<y><c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm),
    ('SMUSD{X}<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, M, Rm),
    #('SSAT<c> <Rd>, #<imm>, <Rn>{, <shift>}', 1, 1, 1, 1, 0, (0), 1, 1, 0, 0, sh, 0, Rn, 0, imm3, Rd, imm2, (0), sat_imm),
    #('SSAT16<c> <Rd>, #<imm>, <Rn>', 1, 1, 1, 1, 0, (0), 1, 1, 0, 0, 1, 0, Rn, 0, 0, 0, 0, Rd, 0, 0, (0), (0), sat_imm),
    ('SSAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('SSUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('SSUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('STM<c> <Rn3>!, <registers>', 1, 1, 0, 0, 0, Rn3, register_list8),
    ('STM<c>.W <Rn>{!}, <registers>', 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, W, 0, Rn, register_list),
    ('STMDB<c> <Rn>{!}, <registers>', 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, W, 0, Rn, register_list),
    ('STR<c> <Rt3>, [<Rn3>{, #<imm>}]', 0, 1, 1, 0, 0, imm5, Rn3, Rt3),
    ('STR<c> <Rt3>, [SP, #<imm>]', 1, 0, 0, 1, 0, Rt3, imm8),
    ('STR<c>.W <Rt>, [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, imm12),
    ('STR<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STR<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STR<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 0, 0, 0, Rm3, Rn3, Rt3),
    ('STR<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('STRB<c> <Rt3>, [<Rn3>, #<imm5>]', 0, 1, 1, 1, 0, imm5, Rn3, Rt3),
    ('STRB<c>.W <Rt>, [<Rn>, #<imm12>]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, Rn, Rt, imm12),
    ('STRB<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STRB<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STRB<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 0, 1, 0, Rm3, Rn3, Rt3),
    ('STRB<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('STRBT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, Rn, Rt, 1, 1, 1, 0, imm8),
    ('STRD<c> <Rt>, <Rt2>, [<Rn>{, #+/-<imm>}]', 1, 1, 1, 0, 1, 0, 0, P, U, 1, W, 0, Rn, Rt, Rt2, imm8),
    ('STREX<c> <Rd>, <Rt>, [<Rn>{, #<imm>}]', 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, Rd, imm8),
    ('STREXB<c> <Rd>, <Rt>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 0, Rd),
    ('STREXD<c> <Rd>, <Rt>, <Rt2>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, Rt2, 0, 1, 1, 1, Rd),
    ('STREXH<c> <Rd>, <Rt>, [<Rn>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, Rn, Rt, (1), (1), (1), (1), 0, 1, 0, 1, Rd),
    ('STRH<c> <Rt3>, [<Rn3>{, #<imm>}]', 1, 0, 0, 0, 0, imm5, Rn3, Rt3),
    ('STRH<c>.W <Rt>, [<Rn>{, #<imm12>}]', 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, Rn, Rt, imm12),
    ('STRH<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STRH<c> <Rt>, [<Rn>, #-<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 1, P, U, W, imm8),
    ('STRH<c> <Rt3>, [<Rn3>, <Rm3>]', 0, 1, 0, 1, 0, 0, 1, Rm3, Rn3, Rt3),
    ('STRH<c>.W <Rt>, [<Rn>, <Rm>{, LSL #<imm2>}]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 0, 0, 0, 0, 0, 0, imm2, Rm),
    ('STRHT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, Rn, Rt, 1, 1, 1, 0, imm8),
    ('STRT<c> <Rt>, [<Rn>, #<imm8>]', 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, Rn, Rt, 1, 1, 1, 0, imm8),
    ('SUB{S}<c> <Rd3>, <Rn3>, #<imm3>', 0, 0, 0, 1, 1, 1, 1, imm3, Rn3, Rd3),
    ('SUB{S}<c> <Rdn3>, #<imm8>', 0, 0, 1, 1, 1, Rdn3, imm8),
    ('SUB{S}<c>.W <Rd>, <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 1, 0, 1, S, Rn, 0, imm3, Rd, imm8),
    ('SUBW<c> <Rd>, <Rn>, #<imm12>', 1, 1, 1, 1, 0, i, 1, 0, 1, 0, 1, 0, Rn, 0, imm3, Rd, imm8),
    ('SUB{S}<c> <Rd3>, <Rn3>, <Rm3>', 0, 0, 0, 1, 1, 0, 1, Rm3, Rn3, Rd3),
    ('SUB{S}<c>.W <Rd>, <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, S, Rn, (0), imm3, Rd, imm2, type_, Rm),
    ('SUB<c> SP, SP, #<imm>', 1, 0, 1, 1, 0, 0, 0, 0, 1, imm7),
    ('SUB{S}<c>.W <Rd>, SP, #<const>', 1, 1, 1, 1, 0, i, 0, 1, 1, 0, 1, S, 1, 1, 0, 1, 0, imm3, Rd, imm8),
    ('SUBW<c> <Rd>, SP, #<imm12>', 1, 1, 1, 1, 0, i, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, imm3, Rd, imm8),
    ('SUB{S}<c> <Rd>, SP, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, S, 1, 1, 0, 1, (0), imm3, Rd, imm2, type_, Rm),
    ('SVC<c> #<imm8>', 1, 1, 0, 1, 1, 1, 1, 1, imm8),
    ('SXTAB<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('SXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('SXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('SXTB<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, Rm3, Rd3),
    ('SXTB<c>.W <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('SXTB16<c> <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('SXTH<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, Rm3, Rd3),
    ('SXTH<c>.W <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('TBB<c> [<Rn>, <Rm>]', 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, Rn, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, H, Rm),
    ('TEQ<c> <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 1, 0, 0, 1, Rn, 0, imm3, 1, 1, 1, 1, imm8),
    ('TEQ<c> <Rn>, <Rm>{, <shift>}', 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, Rn, (0), imm3, 1, 1, 1, 1, imm2, type_, Rm),
    ('TST<c> <Rn>, #<const>', 1, 1, 1, 1, 0, i, 0, 0, 0, 0, 0, 1, Rn, 0, imm3, 1, 1, 1, 1, imm8),
    ('TST<c> <Rn3>, <Rm3>', 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, Rm3, Rn3),
    ('TST<c>.W', 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, Rn, (0), imm3, 1, 1, 1, 1, imm2, type_, Rm),
    ('UADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('UADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('UASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('UBFX<c> <Rd>, <Rn>, #<lsb>, #<width>', 1, 1, 1, 1, 0, (0), 1, 1, 1, 1, 0, 0, Rn, 0, imm3, Rd, imm2, (0), widthm1),
    ('UDF<c> #<imm8>', 1, 1, 0, 1, 1, 1, 1, 0, imm8),
    ('UDF<c>.W #<imm16>', 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, imm4, 1, 0, 1, 0, imm12),
    ('UDIV<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, Rn, (1), (1), (1), (1), Rd, 1, 1, 1, 1, Rm),
    ('UHADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UHADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UHASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UHSAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UHSUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UHSUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 1, 0, Rm),
    ('UMAAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, Rn, RdLo, RdHi, 0, 1, 1, 0, Rm),
    ('UMLAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm),
    ('UMULL<c> <RdLo>, <RdHi>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, Rn, RdLo, RdHi, 0, 0, 0, 0, Rm),
    ('UQADD16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('UQADD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('UQASX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('UQSAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('UQSUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('UQSUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 1, Rm),
    ('USAD8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, Rn, 1, 1, 1, 1, Rd, 0, 0, 0, 0, Rm),
    ('USADA8<c> <Rd>, <Rn>, <Rm>, <Ra>', 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, Rn, Ra, Rd, 0, 0, 0, 0, Rm),
    #('USAT<c> <Rd>, #<imm5>, <Rn>{, <shift>}', 1, 1, 1, 1, 0, (0), 1, 1, 1, 0, sh, 0, Rn, 0, imm3, Rd, imm2, (0), sat_imm),
    #('USAT16<c> <Rd>, #<imm4>, <Rn>', 1, 1, 1, 1, 0, (0), 1, 1, 1, 0, 1, 0, Rn, 0, 0, 0, 0, Rd, 0, 0, (0), (0), sat_imm),
    ('USAX<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('USUB16<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('USUB8<c> <Rd>, <Rn>, <Rm>', 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, Rn, 1, 1, 1, 1, Rd, 0, 1, 0, 0, Rm),
    ('UXTAB<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('UXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('UXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, Rn, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('UXTB<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, Rm3, Rd3),
    ('UXTB<c>.W <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('UXTB16<c> <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('UXTH<c> <Rd3>, <Rm3>', 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, Rm3, Rd3),
    ('UXTH<c>.W <Rd>, <Rm>{, <rotation>}', 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, Rd, 1, (0), rotate, Rm),
    ('WFE<c>', 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0),
    ('WFE<c>.W', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0),
    ('WFI<c>', 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0),
    ('WFI<c>.W', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1),
    ('YIELD<c>', 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0),
    ('YIELD<c>.W', 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, (1), (1), (1), (1), 1, 0, (0), 0, (0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
]

if __name__ == '__main__':
    for description in thumbs:
        instr = description[0]
        bits = description[1:]

        bits = [1 if type(x) == int else x.bitsize for x in bits]
        if not sum(bits) in (16, 32):
            print(instr, bits, sum(bits))
