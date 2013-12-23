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

import itertools
import sys
import textwrap
from tablegen import Table
from tables import armv7


def generate_c_table(l):
    return '\n    '.join(textwrap.wrap(', '.join('%s' % _ for _ in l), 74))


def magic_open(fname):
    sys.stdout = open(fname, 'w')
    print('/*')
    print(__doc__.strip())
    print('*/')


if __name__ == '__main__':
    t = Table(armv7.table, 32)
    sm, lut = t.create()

    insns = sorted(set(_.name for _ in armv7.table))

    magic_open('darm-tables.c')
    print('#include <stdint.h>')
    print('#include "darm.h"')
    print('#include "darm-instr.h"')
    print('#include "darm-internal.h"')
    print('#define offsetof(st, m) ((uint32_t)(&((st *)0)->m))')
    print('#define O(m) offsetof(darm_t, m)')
    print('#define L(x) (x) % 256')
    print('#define H(x) (x) / 256')
    print('const uint8_t g_armv7_sm[%d] = {' % len(sm.table))
    print('    ' + generate_c_table(sm.table))
    print('};')
    print('const uint16_t g_armv7_lut[%d] = {' % len(lut.table))
    print('    ' + generate_c_table(lut.table))
    print('};')
    print('const char *g_darm_instr[%d] = {' % len(insns))
    print('    ' + generate_c_table('"%s"' % _ for _ in insns))
    print('};')

    magic_open('darm-tables.h')
    print('#ifndef __DARM_TABLES__')
    print('#define __DARM_TABLES__')
    print('#include <stdint.h>')
    print('extern const uint8_t g_armv7_sm[%d];' % len(sm.table))
    print('extern const uint16_t g_armv7_lut[%d];' % len(lut.table))
    print('extern const char *g_darm_instr[%d];' % len(insns))
    print('#endif')

    magic_open('darm-instr.h')
    print('#ifndef __DARM_INSTR__')
    print('#define __DARM_INSTR__')
    print('typedef enum _darm_instr_t {')
    l = ['INVLD'] + [_.upper() for _ in insns] + ['INSTRCNT']
    lines = textwrap.wrap(', '.join('I_%s' % _ for _ in l), 74)
    print('    ' + '\n    '.join(lines))
    print('} darm_instr_t;')
    print('#endif')

    magic_open('darm-internal.h')
    print('#ifndef __DARM_INTERNAL__')
    print('#define __DARM_INTERNAL__')

    print("""typedef enum _darm_sm_opcode_t {
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

    // This instruction has been disassembled correctly, return success.
    SM_RETN,

    // Assign the instruction index.
    SM_INSTR,

    // Extracts a couple of bits from the instruction and stores them in
    // the given field in the darm_t object.
    SM_EXTR,

    // Extract an immediate.
    SM_IMM,

    // Extra information for generating a human-readable string.
    SM_STR,

    SM_ARMExpandImm,
} darm_sm_opcode_t;

typedef enum _darm_string_opcode_t {
    STR_HLT, STR_RETN,
} darm_string_opcode_t;""")

    # define constants 0b0 up upto 0b11111111
    for x in range(256):
        print('#define %s %d' % (bin(x)[1:], x))

    # define partial constants with leading zeroes, such as 0b0001
    for x in range(2, 7):
        for y in itertools.product('01', repeat=x):
            num = ''.join(y)
            print('#define b%s %d' % (num, int(num, 2)))

    print('#endif')
