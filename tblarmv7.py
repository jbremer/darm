import sys
from tablegen import Instruction, BitPattern, Macro, Table
from tablegen import Flag, Register, Immediate


# TODO cond cannot be 0b1111
cond = BitPattern(4, 'cond')
S = Flag(1, 'S', False)
Rn = Register(4, 'Rn')
Rm = Register(4, 'Rm')
Rd = Register(4, 'Rd')
Rs = Register(4, 'Rs')
typ = BitPattern(2, 'type')

imm5 = Immediate(5, 'imm5')
imm12 = Immediate(12, 'imm12')

ARMExpandImm = Macro('ARMExpandImm')


table = [
    Instruction('adc', (cond, 0, 0, 1, 0, 1, 0, 1, S, Rn, Rd, imm12), imm=ARMExpandImm(imm12)),
    Instruction('adc', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, imm5, typ, 0, Rm)),
    Instruction('adc', (cond, 0, 0, 0, 0, 1, 0, 1, S, Rn, Rd, Rs, 0, typ, 1, Rm)),
]

if __name__ == '__main__':
    t = Table(table, 32)
    sm, lut = t.create()

    sys.stdout = open('darm-tables.c', 'w')
    print '#include <stdint.h>'
    print 'const uint8_t g_armv7_sm[%d] = {' % len(sm.table)
    print '   ', ', '.join(str(_) for _ in sm.table)
    print '};'
    print 'const uint16_t g_armv7_lut[%d] = {' % len(lut.table)
    print '   ', ', '.join(str(_) for _ in lut.table)
    print '};'

    sys.stdout = open('darm-tables.h', 'w')
    print '#ifndef __DARM_ARMV7_TBL__'
    print '#define __DARM_ARMV7_TBL__'
    print '#include <stdint.h>'
    print 'extern const uint8_t g_armv7_sm[%d];' % len(sm.table)
    print 'extern const uint16_t g_armv7_lut[%d];' % len(lut.table)
    print '#endif'
