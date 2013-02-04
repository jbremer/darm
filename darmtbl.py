"""Disassembler for the ARM instructions set(s)."""


class Operand:
    def __init__(self, name, bitsize, comment):
        self.name = name
        self.bitsize = bitsize
        self.comment = comment


class Immediate(Operand):
    pass


class Register(Operand):
    pass


cond = Immediate('cond', 4, 'Conditional Flags')
Rd = Register('Rd', 4, 'Destination Register')
Rs = Register('Rs', 4, 'Source Register')
Rn = Register('Rn', 4, 'N Register')
Rm = Register('Rm', 4, 'Shift Register')
Rt = Register('Rt', 4, 'Ternary Register')
Rt2 = Immediate('Rt2', 4, 'Second Ternary Register')
Ra = Register('Ra', 4, 'Accumulate Register')
S = Register('S', 1, 'Update Conditional Flags')
type_ = Immediate('type', 2, 'Shift Type')
msb = Immediate('msb', 5, 'Most Significant Bit')
lsb = Immediate('lsb', 5, 'Least Significant Bit')
register_list = Immediate('register_list', 16, 'Register List')
E = Immediate('E', 1, 'Endian Specifier')
msr = Immediate('msr', 2, 'Move to Special Register mask')
rotate = Immediate('rotate', 2, 'Rotation Type')
H = Immediate('H', 1, 'Sign Extension Bit for BL(X)')
option = Immediate('option', 4, 'Option for Debug Hint')
W = Immediate('W', 1, 'Some Bit for LDM')
widthm1 = Immediate('widthm1', 5, 'Bit Width Minus One')
M = Immediate('M', 1, 'High 16bits for Rm')
N = Immediate('N', 1, 'High 16bits for Rn')
RdHi = Immediate('RdHi', 4, 'High 32bits for Rd')
RdLo = Immediate('RdLo', 4, 'Low 32bits for Rd')
R = Immediate('R', 1, 'Round Integer')
sat_imm = Immediate('sat_imm', 5, 'Saturate Immediate')
sh = Immediate('sh', 2, 'Immediate Shift')

imm4 = Immediate('imm4', 4, 'Immediate')
imm5 = Immediate('imm5', 5, 'Immediate')
imm12 = Immediate('imm12', 12, 'Immediate')
imm24 = Immediate('imm24', 24, 'Immediate')

ARMv7 = [
    ('ADC{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 1, 0, 1, S, Rn, Rd, imm12),
    ('ADC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, imm5, type_, 0, Rm),
    ('ADC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('ADD{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 1, 0, 0, S, Rn, Rd, imm12),
    ('ADD{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('ADD{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 1, 0, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('ADD{S}<c> <Rd>,SP,#<const>', cond, 0, 0, 1, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm12),
    ('ADD{S}<c> <Rd>,SP,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 1, 0, 0, S, 1, 1, 0, 1, Rd, imm5, type_, 0, Rm),
    ('ADR<c> <Rd>,<label>', cond, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, imm12),
    ('ADR<c> <Rd>,<label>', cond, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, Rd, imm12),
    ('AND{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 0, 0, 0, S, Rn, Rd, imm12),
    ('AND{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('AND{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 0, 0, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('ASR{S}<c> <Rd>,<Rm>,#<imm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 1, 0, 0, Rm),
    ('ASR{S}<c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 1, 0, 1, Rn),
    ('B<c> <label>', cond, 1, 0, 1, 0, imm24),
    ('BFC<c> <Rd>,#<lsb>,#<width>', cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, 1, 1, 1, 1),
    ('BFI<c> <Rd>,<Rn>,#<lsb>,#<width>', cond, 0, 1, 1, 1, 1, 1, 0, msb, Rd, lsb, 0, 0, 1, Rn),
    ('BIC{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 1, 1, 1, 0, S, Rn, Rd, imm12),
    ('BIC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('BIC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 1, 1, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('BKPT #<imm16>', cond, 0, 0, 0, 1, 0, 0, 1, 0, imm12, 0, 1, 1, 1, imm4),
    ('BL<c> <label>', cond, 1, 0, 1, 1, imm24),
    ('BLX <label>', 1, 1, 1, 1, 1, 0, 1, H, imm24),
    ('BLX<c> <Rm>', cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('BX<c> Rm', cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('BXJ<c> <Rm>', cond, 0, 0, 0, 1, 0, 0, 1, 0, (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), (1), 0, 0, 1, 0, Rm),
    ('CLREX', 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 1, (1), (1), (1), (1)),
    ('CLZ<c> <Rd>,<Rm>', cond, 0, 0, 0, 1, 0, 1, 1, 0, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('CMN<c> <Rn>,#<const>', cond, 0, 0, 1, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm12),
    ('CMN<c> <Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), imm5, type_, 0, Rm),
    ('CMN<c> <Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 0, 1, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, type_, 1, Rm),
    ('CMP<c> <Rn>,#<const>', cond, 0, 0, 1, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm12),
    ('CMP<c> <Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), imm5, type_, 0, Rm),
    ('CMP<c> <Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 0, 1, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, type_, 1, Rm),
    ('DBG<c> #<option>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 1, 1, 1, 1, option),
    ('DMB #<option>', 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 1),
    ('DSB #<option>', 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 0, 0),
    ('EOR{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 0, 0, 1, S, Rn, Rd, imm12),
    ('EOR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, imm5, type_, 0, Rm),
    ('EOR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 0, 0, 1, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('ISB #<option>', 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, (1), (1), (1), (1), (1), (1), (1), (1), (0), (0), (0), (0), 0, 1, 1, 0),
    ('LDM<c> <Rn>{!},<registers>', cond, 1, 0, 0, 0, 1, 0, W, 1, Rn, register_list),
    ('LDMDA<c> <Rn>{!},<registers>', cond, 1, 0, 0, 0, 0, 0, W, 1, Rn, register_list),
    ('LDMDB<c> <Rn>{!},<registers>', cond, 1, 0, 0, 1, 0, 0, W, 1, Rn, register_list),
    ('LDMIB<c> <Rn>{!},<registers>', cond, 1, 0, 0, 1, 1, 0, W, 1, Rn, register_list),
    ('LDREX<c> <Rt>,[<Rn>]', cond, 0, 0, 0, 1, 1, 0, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1)),
    ('LDREXB<c> <Rt>, [<Rn>]', cond, 0, 0, 0, 1, 1, 1, 0, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1)),
    ('LDREXD<c> <Rt>,<Rt2>,[<Rn>]', cond, 0, 0, 0, 1, 1, 0, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1)),
    ('LDREXH<c> <Rt>, [<Rn>]', cond, 0, 0, 0, 1, 1, 1, 1, 1, Rn, Rt, (1), (1), (1), (1), 1, 0, 0, 1, (1), (1), (1), (1)),
    ('LSL{S}<c> <Rd>,<Rm>,#<imm5>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 0, 0, Rm),
    ('LSL{S}<c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 0, 1, Rn),
    ('LSR{S}<c> <Rd>,<Rm>,#<imm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 0, 1, 0, Rm),
    ('LSR{S}<c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 0, 1, 1, Rn),
    ('MLA{S}<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 0, 0, 0, 0, 0, 1, S, Rd, Ra, Rm, 1, 0, 0, 1, Rn),
    ('MLS<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 0, 0, 0, 0, 1, 1, 0, Rd, Ra, Rm, 1, 0, 0, 1, Rn),
    ('MOV{S}<c> <Rd>,#<const>', cond, 0, 0, 1, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm12),
    ('MOVW<c> <Rd>,#<imm16>', cond, 0, 0, 1, 1, 0, 0, 0, 0, imm4, Rd, imm12),
    ('MOV{S}<c> <Rd>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 0, 0, 0, Rm),
    ('MOVT<c> <Rd>,#<imm16>', cond, 0, 0, 1, 1, 0, 1, 0, 0, imm4, Rd, imm12),
    ('MRS<c> <Rd>,<spec_reg>', cond, 0, 0, 0, 1, 0, 0, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0)),
    ('MSR<c> <spec_reg>,#<const>', cond, 0, 0, 1, 1, 0, 0, 1, 0, msr, 0, 0, (1), (1), (1), (1), imm12),
    ('MSR<c> <spec_reg>,<Rn>', cond, 0, 0, 0, 1, 0, 0, 1, 0, msr, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, Rn),
    ('MUL{S}<c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 0, 0, 0, S, Rd, (0), (0), (0), (0), Rm, 1, 0, 0, 1, Rn),
    ('MVN{S}<c> <Rd>,#<const>', cond, 0, 0, 1, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm12),
    ('MVN{S}<c> <Rd>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, imm5, type_, 0, Rm),
    ('MVN{S}<c> <Rd>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 1, 1, 1, S, (0), (0), (0), (0), Rd, Rs, 0, type_, 1, Rm),
    ('NOP<c>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 0),
    ('ORR{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 1, 1, 0, 0, S, Rn, Rd, imm12),
    ('ORR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('ORR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 1, 0, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('POP<c> <registers>', cond, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, register_list),
    ('POP<c> <registers>', cond, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, Rt, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
    ('PUSH<c> <registers>', cond, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, register_list),
    ('PUSH<c> <registers>', cond, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, Rt, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
    ('QADD<c> <Rd>,<Rm>,<Rn>', cond, 0, 0, 0, 1, 0, 0, 0, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm),
    ('QADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('QADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('QASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('QDADD<c> <Rd>,<Rm>,<Rn>', cond, 0, 0, 0, 1, 0, 1, 0, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm),
    ('QDSUB<c> <Rd>,<Rm>,<Rn>', cond, 0, 0, 0, 1, 0, 1, 1, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm),
    ('QSAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('QSUB<c> <Rd>,<Rm>,<Rn>', cond, 0, 0, 0, 1, 0, 0, 1, 0, Rn, Rd, (0), (0), (0), (0), 0, 1, 0, 1, Rm),
    ('QSUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('QSUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('RBIT<c> <Rd>,<Rm>', cond, 0, 1, 1, 0, 1, 1, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('REV<c> <Rd>,<Rm>', cond, 0, 1, 1, 0, 1, 0, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('REV16<c> <Rd>,<Rm>', cond, 0, 1, 1, 0, 1, 0, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm),
    ('REVSH<c> <Rd>,<Rm>', cond, 0, 1, 1, 0, 1, 1, 1, 1, (1), (1), (1), (1), Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm),
    ('ROR{S}<c> <Rd>,<Rm>,#<imm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, imm5, 1, 1, 0, Rm),
    ('ROR{S}<c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, Rm, 0, 1, 1, 1, Rn),
    ('RRX{S}<c> <Rd>,<Rm>', cond, 0, 0, 0, 1, 1, 0, 1, S, (0), (0), (0), (0), Rd, 0, 0, 0, 0, 0, 1, 1, 0, Rm),
    ('RSB{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 0, 1, 1, S, Rn, Rd, imm12),
    ('RSB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, imm5, type_, 0, Rm),
    ('RSB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 0, 1, 1, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('RSC{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 1, 1, 1, S, Rn, Rd, imm12),
    ('RSC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, imm5, type_, 0, Rm),
    ('RSC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 1, 1, 1, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('SADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('SADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('SASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('SBC{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 1, 1, 0, S, Rn, Rd, imm12),
    ('SBC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 1, 1, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('SBC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 1, 1, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('SBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', cond, 0, 1, 1, 1, 1, 0, 1, widthm1, Rd, lsb, 1, 0, 1, Rn),
    ('SEL<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 1, 1, Rm),
    ('SETEND <endian_specifier>', 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, (0), (0), (0), 1, (0), (0), (0), (0), (0), (0), E, (0), 0, 0, 0, 0, (0), (0), (0), (0)),
    ('SEV<c>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 1, 0, 0),
    ('SHADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('SHADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('SHASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('SHSAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('SHSUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('SHSUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('SMLA<x><y><c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 0, 0, 1, 0, 0, 0, 0, Rd, Ra, Rm, 1, M, N, 0, Rn),
    ('SMLAD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 0, M, 1, Rn),
    ('SMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 1, 1, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn),
    ('SMLAL<x><y><c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, M, N, 0, Rn),
    ('SMLALD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 0, M, 1, Rn),
    ('SMLAW<y><c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, Ra, Rm, 1, M, 0, 0, Rn),
    ('SMLSD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, Ra, Rm, 0, 1, M, 1, Rn),
    ('SMLSLD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 0, 1, 0, 0, RdHi, RdLo, Rm, 0, 1, M, 1, Rn),
    ('SMMLA{R}<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 0, 0, R, 1, Rn),
    ('SMMLS{R}<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, Ra, Rm, 1, 1, R, 1, Rn),
    ('SMMUL{R}<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 0, 1, 0, 1, Rd, 1, 1, 1, 1, Rm, 0, 0, R, 1, Rn),
    ('SMUAD{X}<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, M, 1, Rn),
    ('SMUL<x><y><c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 0, 1, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, M, N, 0, Rn),
    ('SMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 1, 1, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn),
    ('SMULW<y><c> <Rd>,<Rn>,<Rm>', cond, 0, 0, 0, 1, 0, 0, 1, 0, Rd, (0), (0), (0), (0), Rm, 1, M, 1, 0, Rn),
    ('SMUSD{X}<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 0, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 1, M, 1, Rn),
    ('SSAT<c> <Rd>,#<imm>,<Rn>{,<shift>}', cond, 0, 1, 1, 0, 1, 0, 1, sat_imm, Rd, imm5, sh, 0, 1, Rn),
    ('SSAT16<c> <Rd>,#<imm>,<Rn>', cond, 0, 1, 1, 0, 1, 0, 1, 0, sat_imm, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rn),
    ('SSAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('SSUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('SSUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 0, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('STM<c> <Rn>{!},<registers>', cond, 1, 0, 0, 0, 1, 0, W, 0, Rn, register_list),
    ('STMDA<c> <Rn>{!},<registers>', cond, 1, 0, 0, 0, 0, 0, W, 0, Rn, register_list),
    ('STMDB<c> <Rn>{!},<registers>', cond, 1, 0, 0, 1, 0, 0, W, 0, Rn, register_list),
    ('STMIB<c> <Rn>{!},<registers>', cond, 1, 0, 0, 1, 1, 0, W, 0, Rn, register_list),
    ('STREX<c> <Rd>,<Rt>,[<Rn>]', cond, 0, 0, 0, 1, 1, 0, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt),
    ('STREXB<c> <Rd>,<Rt>,[<Rn>]', cond, 0, 0, 0, 1, 1, 1, 0, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt),
    ('STREXD<c> <Rd>,<Rt>,<Rt2>,[<Rn>]', cond, 0, 0, 0, 1, 1, 0, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt),
    ('STREXH<c> <Rd>,<Rt>,[<Rn>]', cond, 0, 0, 0, 1, 1, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rt),
    ('SUB{S}<c> <Rd>,<Rn>,#<const>', cond, 0, 0, 1, 0, 0, 1, 0, S, Rn, Rd, imm12),
    ('SUB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, imm5, type_, 0, Rm),
    ('SUB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 0, 0, 1, 0, S, Rn, Rd, Rs, 0, type_, 1, Rm),
    ('SUB{S}<c> <Rd>,SP,#<const>', cond, 0, 0, 1, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm12),
    ('SUB{S}<c> <Rd>,SP,<Rm>{,<shift>}', cond, 0, 0, 0, 0, 0, 1, 0, S, 1, 1, 0, 1, Rd, imm5, type_, 0, Rm),
    ('SVC<c> #<imm24>', cond, 1, 1, 1, 1, imm24),
    ('SWP<c> <Rt>,<Rt2>,[<Rn>]', cond, 0, 0, 0, 1, 0, 0, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2),
    ('SWPB<c> <Rt>,<Rt2>,[<Rn>]', cond, 0, 0, 0, 1, 0, 1, 0, 0, Rn, Rt, (0), (0), (0), (0), 1, 0, 0, 1, Rt2),
    ('SXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 1, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('SXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 0, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('SXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 1, 1, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('SXTB<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('SXTB16<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('SXTH<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('TEQ<c> <Rn>,#<const>', cond, 0, 0, 1, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm12),
    ('TEQ<c> <Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), imm5, type_, 0, Rm),
    ('TEQ<c> <Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 0, 0, 1, 1, Rn, (0), (0), (0), (0), Rs, 0, type_, 1, Rm),
    ('TST<c> <Rn>,#<const>', cond, 0, 0, 1, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm12),
    ('TST<c> <Rn>,<Rm>{,<shift>}', cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), imm5, type_, 0, Rm),
    ('TST<c> <Rn>,<Rm>,<type> <Rs>', cond, 0, 0, 0, 1, 0, 0, 0, 1, Rn, (0), (0), (0), (0), Rs, 0, type_, 1, Rm),
    ('UADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('UADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('UASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>', cond, 0, 1, 1, 1, 1, 1, 1, widthm1, Rd, lsb, 1, 0, 1, Rn),
    ('UHADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('UHADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('UHASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('UHSAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('UHSUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('UHSUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('UMAAL<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 0, 1, 0, 0, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn),
    ('UMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 1, 0, 1, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn),
    ('UMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>', cond, 0, 0, 0, 0, 1, 0, 0, S, RdHi, RdLo, Rm, 1, 0, 0, 1, Rn),
    ('UQADD16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 0, 1, Rm),
    ('UQADD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 0, 0, 1, Rm),
    ('UQASX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rm),
    ('UQSAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('UQSUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('UQSUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 1, 0, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('USAD8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 1, 1, 0, 0, 0, Rd, 1, 1, 1, 1, Rm, 0, 0, 0, 1, Rn),
    ('USADA8<c> <Rd>,<Rn>,<Rm>,<Ra>', cond, 0, 1, 1, 1, 1, 0, 0, 0, Rd, Ra, Rm, 0, 0, 0, 1, Rn),
    ('USAT<c> <Rd>,#<imm5>,<Rn>{,<shift>}', cond, 0, 1, 1, 0, 1, 1, 1, sat_imm, Rd, imm5, sh, 0, 1, Rn),
    ('USAT16<c> <Rd>,#<imm4>,<Rn>', cond, 0, 1, 1, 0, 1, 1, 1, 0, sat_imm, Rd, (1), (1), (1), (1), 0, 0, 1, 1, Rn),
    ('USAX<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 0, 1, Rm),
    ('USUB16<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 0, 1, 1, 1, Rm),
    ('USUB8<c> <Rd>,<Rn>,<Rm>', cond, 0, 1, 1, 0, 0, 1, 0, 1, Rn, Rd, (1), (1), (1), (1), 1, 1, 1, 1, Rm),
    ('UXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 1, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('UXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 0, 0, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('UXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 1, 1, Rn, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('UXTB<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('UXTB16<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('UXTH<c> <Rd>,<Rm>{,<rotation>}', cond, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, Rd, rotate, (0), (0), 0, 1, 1, 1, Rm),
    ('WFE<c>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 0),
    ('WFI<c>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 1, 1),
    ('YIELD<c>', cond, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, (1), (1), (1), (1), (0), (0), (0), (0), 0, 0, 0, 0, 0, 0, 0, 1),
    ('MRS<c> <Rd>,<spec_reg>', cond, 0, 0, 0, 1, 0, R, 0, 0, (1), (1), (1), (1), Rd, (0), (0), (0), (0), 0, 0, 0, 0, (0), (0), (0), (0)),
    ('SMC<c> #<imm4>', cond, 0, 0, 0, 1, 0, 1, 1, 0, (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), 0, 1, 1, 1, imm4),
]
