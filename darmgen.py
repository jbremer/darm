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
import darmtbl
import darmtbl2
import itertools
import sys
import textwrap


def instruction_name(x):
    return x.split('{')[0].split('<')[0].split()[0]


def instruction_names(arr):
    """List of all unique instruction names."""
    return ['INVLD'] + sorted(set(instruction_name(x) for x in arr))


def enum_table(name, arr):
    """Enumeration."""
    text = '\n    '.join(textwrap.wrap(', '.join(arr), 74))
    return 'typedef enum _%s_t {\n    %s\n} %s_t;\n' % (name, text, name)


def typed_table(typ, name, arr):
    """A table with a given type."""
    text = '\n    '.join(textwrap.wrap(', '.join(arr), 74))

    # if it's not a pointer, append a space
    if typ[-1] != '*':
        typ += ' '
    return '%s%s[] = {\n    %s\n};\n' % (typ, name, text)


def string_table(name, arr):
    """A string table."""
    return typed_table('const char *', name, ('"%s"' % x for x in arr))


def instruction_names_enum(arr):
    """Enumeration of all instruction names."""
    return enum_table('darm_instr',
                      ['I_%s' % x for x in instruction_names(arr)] +
                      ['I_INSTRCNT'])


def instruction_names_table(arr):
    """Table of strings of all instructions."""
    return string_table('darm_mnemonics', instruction_names(arr))


def instruction_types_table(arr, kind):
    """Lookup table for the types of instructions."""
    arr = ['T_%s' % arr[x][1][1] if x in arr else 'T_INVLD'
           for x in range(256)]
    return typed_table('darm_enctype_t', '%s_instr_types' % kind, arr)


def instruction_names_index_table(arr, kind):
    """Lookup table for instruction label for each instruction index."""
    arr = ['I_%s' % arr[x][0] if x in arr else 'I_INVLD'
           for x in range(256)]
    return typed_table('darm_instr_t', '%s_instr_labels' % kind, arr)


def type_lookup_table(name, *args):
    """Create a lookup table for a certain instruction type."""
    arr = ('I_%s' % x.upper() if x else 'I_INVLD' for x in args)
    return typed_table('darm_instr_t', '%s_instr_lookup' % name, arr)


def type_encoding_enum(enumname, arr):
    text = []
    for _, name, info, encodings, _, affects in arr:
        text.append(
            '    // info:\n' +
            '    // %s\n    //\n' % info +
            '    // encodings:\n    // ' +
            '\n    // '.join(encodings) + '\n    //\n' +
            '    // affects:\n    // ' +
            '\n    // '.join(textwrap.wrap(', '.join(affects), 74)) + '\n' +
            '    T_%s,' % name)

    return 'typedef enum _%s_t {\n%s\n} %s_t;\n' % (enumname,
                                                    '\n\n'.join(text),
                                                    enumname)


def type_encoding_table(tblname, arr):
    """Table of strings of all instructions."""
    return string_table(tblname, (x[1] for x in arr))


def generate_format_strings(arr):
    ret = {}

    # a set of rules to transform a string representation as given by the
    # armv7 manual, into our own custom format string
    rules = [
        # if this instruction updates the condition flags, then an S is added
        # to the end of the instruction
        '{S}', 's',

        # if this instruction is conditional, then the condition under which
        # it executes is appended to the instruction
        '<c>', 'c',

        # memory address
        '[<Rn>,#+/-<imm12>]', 'M',
        '[<Rn>,+/-<Rm>{,<shift>}]', 'M',

        # memory address with Rn as base register and an immediate or Rm
        # operand as offset
        '[<Rn>]', 'B',
        '#+/-<imm12>', 'O',
        '#+/-<imm8>', 'O',
        '+/-<Rm>', 'O',

        # various register operands
        '<Rd>', 'd',
        '<Rd3>', 'd',
        '<Rdn>', 'd',
        '<Rdn3>', 'd',
        '<Rn>', 'n',
        '<Rn3>', 'n',
        '<Rm>', 'm',
        '<Rm3>', 'm',
        '<Ra>', 'a',
        '<Rt>', 't',
        '<Rt2>', '2',
        '<RdHi>', 'h',
        '<RdLo>', 'l',

        # immediate values
        '#<const>', 'i',
        '#<imm2>', 'i',
        '#<imm4>', 'i',
        '#<imm5>', 'i',
        '#<imm7>', 'i',
        '#<imm8>', 'i',
        '#<imm12>', 'i',
        '#<imm16>', 'i',
        '#<imm24>', 'i',

        # immediate and register shift
        '{,<shift>}', 'S',
        '#<shift>', 'S',
        '<type> <Rs>', 'S',

        # some bit instructions take a lsb and width as operand
        '#<lsb>', 'L',
        '#<width>', 'w',

        # for branch instructions
        '<label>', 'b',

        # option immediate for various obscure instructions
        '#<option>', 'o',

        # either a list of registers, reglist, or a single register
        '<registers>', 'r',

        # exclamation mark to specify the write-back bit
        '{!}', '!',

        # the SETEND instruction takes a one or zero as operand
        '<endian_specifier>', 'e',

        # some signed multiplication instructions take an X flag, which
        # means that you can swap halfwords of the second operand
        '{X}', 'x',

        # certain signed multiplication instructions take these flags in
        # order to swap halfwords that are being used
        '<x><y>', 'X',

        # rounding flag for various signed multiplication instructions
        '{R}', 'R',

        # rotation of operands
        '{,<rotation>}', 'A',

        # the PKH instruction has either a TB or BT postfix, specified by
        # the T member of the darm object
        '<T>', 'T',
    ]

    for row in arr:
        full = row[0]

        instr = instruction_name(full)

        # strip the instruction
        full = full[len(instr):]

        # apply all rules
        for k, v in zip(*[iter(rules)]*2):
            full = full.replace(k, v)

        full = full.replace(',', '').replace(' ', '')
        if instr not in ret:
            ret[instr] = [full]
        elif ret[instr][0] == full[:len(ret[instr][0])]:
            ret[instr][0] = full
        else:
            ret[instr].append(full)
    return ret


def magic_open(fname):
    # python magic!
    sys.stdout = open(fname, 'w')

    # print the license
    print('/*')
    print(__doc__.strip())
    print('*/')

d = darmtbl
d2 = darmtbl2


def notype(*x):
    return (0,) + x


def armv7(*x):
    return (1,) + x


def thumb(*x):
    return (2,) + x


def thumb2(*x):
    return (3,) + x


# we specify various instruction types
instr_types = [
    notype('INVLD', 'Invalid or non-existent type',
           ['I_INVLD'], lambda x, y, z: False),
    armv7('ADR', 'ADR Instruction, which is an optimization of ADD',
          ['ADR<c> <Rd>,<label>'], lambda x, y, z: y[:3] == 'ADR'),
    armv7('UNCOND', 'All unconditional instructions',
          ['ins <endian_specifier>', 'ins [<Rn>,#+/-<imm12>]',
           'ins [<Rn>,#<imm12>]', 'ins', 'ins #<option>', 'ins <label>'],
          lambda x, y, z: False),
    armv7('MUL', 'All multiplication instructions',
          ['ins{S}<c> <Rd>,<Rn>,<Rm>', 'ins{S}<c> <Rd>,<Rn>,<Rm>,<Ra>',
           'ins{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>'],
          lambda x, y, z: x[1:5] == (0,)*4 and x[-5:-1] == (1, 0, 0, 1)),
    armv7('STACK0', 'Various STR and LDR instructions',
          ['ins<c> <Rt>,[<Rn>,#+/-<imm12>]', 'ins<c> <Rt>,[<Rn>],#+/-<imm12>',
           'ins<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}'],
          lambda x, y, z: x[1:3] == (0, 1) and not (x[3] == 1 == x[-2])),
    armv7('STACK1', 'Various unprivileged STR and LDR instructions',
          ['ins<c> <Rt>,[<Rn>],+/-<Rm>', 'ins<c> <Rt>,[<Rn>]{,#+/-<imm8>}'],
          lambda x, y, z: x[-5] == 1 and x[-2] == 1 and x[-4:-2] != (0, 0) and
          x[1:5] == (0, 0, 0, 0) and x[7] == 1),
    armv7('STACK2', 'Various other STR and LDR instructions',
          ['ins<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>',
           'ins<c> <Rt>,[<Rn>],+/-<Rm>',
           'ins<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>',
           'ins<c> <Rt>,<Rt2>,[<Rn>,#+/-<imm8>]',
           'ins<c> <Rt>,[<Rn>,#+/-<imm8>]', ],
          lambda x, y, z: x[1:4] == (0,)*3 and x[-2] == 1 and x[-5] == 1 and
          x[-4:-2] != (0, 0) and not x[-1] in (0, 1) and
          not (x[4] == 0 and x[7] == 1)),
    armv7('ARITH_SHIFT',
          'Arithmetic instructions which take a shift for the second source',
          ['ins{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}',
           'ins{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>'],
          lambda x, y, z: d.Rn in x and d.Rd in x and x[-3] == d.type_
          and x[-1] == d.Rm),
    armv7('ARITH_IMM',
          'Arithmetic instructions which take an immediate as second source',
          ['ins{S}<c> <Rd>,<Rn>,#<const>'],
          lambda x, y, z: d.Rn in x and d.Rd in x and d.imm12 in x),
    armv7('BITS', 'Bit field magic',
          [], lambda x, y, z: d.lsb in x),
    armv7('BRNCHSC', 'Branch and System Call instructions',
          ['B(L)<c> <label>', 'SVC<c> #<imm24>'],
          lambda x, y, z: x[-1] == d.imm24),
    armv7('BRNCHMISC', 'Branch and Misc instructions',
          ['B(L)X(J)<c> <Rm>', 'BKPT #<imm16>', 'MSR<c> <spec_reg>,<Rn>'],
          lambda x, y, z: x[1:9] == (0, 0, 0, 1, 0, 0, 1, 0) and
          not y[0] == 'Q'),
    armv7('MOV_IMM', 'Move immediate to a register (possibly negating it)',
          ['ins{S}<c> <Rd>,#<const>'],
          lambda x, y, z: x[-1] == d.imm12 and x[-2] == d.Rd),
    armv7('CMP_OP', 'Comparison instructions which take two operands',
          ['ins<c> <Rn>,<Rm>{,<shift>}', 'ins<c> <Rn>,<Rm>,<type> <Rs>'],
          lambda x, y, z: x[-1] == d.Rm and x[-3] == d.type_ and
          (x[-4] == d.imm5 and x[-8:-4] == (0, 0, 0, 0) or
           x[-5] == d.Rs and x[-9:-5] == (0, 0, 0, 0))),
    armv7('CMP_IMM', 'Comparison instructions which take an immediate',
          ['ins<c> <Rn>,#<const>'],
          lambda x, y, z: x[-1] == d.imm12 and x[-6] == d.Rn),
    armv7('OPLESS', 'Instructions which don\'t take any operands',
          ['ins<c>'],
          lambda x, y, z: len(x) == 29),
    armv7('DST_SRC', 'Manipulate and move a register to another register',
          ['ins{S}<c> <Rd>,<Rm>', 'ins{S}<c> <Rd>,<Rm>,#<imm>',
           'ins{S}<c> <Rd>,<Rn>,<Rm>'],
          lambda x, y, z: z == 26 or z == 27),
    armv7('LDSTREGS', 'Load or store multiple registers at once',
          ['ins<c> <Rn>{!},<registers>'],
          lambda x, y, z: x[-1] == d.register_list),
    armv7('BITREV', 'Bit reverse instructions',
          ['ins<c> <Rd>,<Rm>'],
          lambda x, y, z: x[-1] == d.Rm and x[-10] == d.Rd and
          x[-11] != d.Rn),
    armv7('MISC', 'Various miscellaneous instructions',
          ['ins{S}<c> <Rd>,<Rm>,<type> <Rs>', 'ins{S}<c> <Rd>,<Rm>{,<shift>}',
           'ins<c> #<imm4>', 'ins<c> #<option>',
           'ins<c> <Rd>,<Rn>,<Rm>{,<type> #<imm>}', 'ins<c> <Rd>,<Rn>,<Rm>'],
          lambda x, y, z: instruction_name(y) in ('MVN', 'SMC', 'DBG', 'PKH',
                                                  'SEL')),
    armv7('SM', 'Various signed multiply instructions', [],
          lambda x, y, z: y[:2] == 'SM'),
    armv7('PAS', 'Parallel signed and unsigned addition and subtraction',
          ['ins<c> <Rd>,<Rn>,<Rm>'],
          lambda x, y, z: z in (97, 98, 99, 101, 102, 103)),
    armv7('SAT', 'Saturating addition and subtraction instructions',
          ['ins<c> <Rd>,<Rn>,<Rm>'],
          lambda x, y, z: y[0] == 'Q'),
    armv7('SYNC', 'Synchronization primitives',
          ['ins{B}<c> <Rt>,<Rt2>,[<Rn>]', 'ins<c> <Rd>,<Rt>,[<Rn>]',
           'ins<c> <Rt>,<Rt2>,[<Rn>]', 'ins<c> <Rt>,[<Rn>]'],
          lambda x, y, z: x[1:5] == (0, 0, 0, 1) and
          (x[-5:-1] == (1, 0, 0, 1) or x[-8:-4] == (1, 0, 0, 1))),
    armv7('PUSR', 'Packing, unpacking, saturation, and reversal instructions',
          ['ins<c> <Rd>,#<imm>,<Rn>', 'ins<c> <Rd>,#<imm>,<Rn>{,<shift>}',
           'ins<c> <Rd>,<Rn>,<Rm>{,<rotation>}',
           'ins<c> <Rd>,<Rm>{,<rotation>}'],
          lambda x, y, z: x[1:6] == (0, 1, 1, 0, 1)),
    thumb('ONLY_IMM8', 'Instructions which only take an 8-byte immediate',
          ['ins<c> #<imm8>'],
          lambda x, y, z: d2.imm8 in x and len(x) == 9),
    thumb('PUSHPOP', 'Push and pop instruction',
          ['ins<c> <registers>'],
          lambda x, y, z: x[0:4] == (1, 0, 1, 1) and x[5:7] == (1, 0) and x[-1] == d2.register_list8),
    thumb('REG_IMM', 'Apply immediate to a register',
          ['ins <Rd3>,#<const>',
           'ins{S}<c> <Rd>,#<const>',
           'ins{S}<c> <Rd3>,#<const>',
           'ins{S}<c> <Rdn>,#<const>'],	
          lambda x, y, z: x[0:3] == (0, 0, 1) and x[-1] == d2.imm8 and x[-2] in (d2.Rd, d2.Rd3, d2.Rdn)),
	thumb('ARITH_REG_REG', 'Arithmetic with registers',
			['ins{S}<c> <Rd>,<Rn>,<Rm>',
			'ins{S} <Rd3>, <Rn3>, <Rm3>',
			'ins <Rd3>, <Rn3>, <Rm3>'
				],
			lambda x, y, z: x[0:5] == (0,0,0,1,1) and x[-3:] == (d2.Rm3, d2.Rn3, d2.Rd3)),
]

if __name__ == '__main__':
    armv7_table, thumb_table, thumb2_table = {}, {}, {}

    # the last item (a list) will contain the instructions affected by this
    # encoding type
    instr_types = [list(x) + [[]] for x in instr_types]

    # prepend the instruction set to the encoding types
    insns_types = '', 'ARM_', 'THUMB_', 'THUMB2_'
    instr_types = [[x[0]] + [insns_types[x[0]] + x[1]] + x[2:6]
                   for x in instr_types]

    # list of encoding types which should not be emitted in the table (because
    # they are handled somewhere else, in a somewhat hardcoded fashion)
    type_ignore = 'ARM_MUL', 'ARM_STACK0', 'ARM_STACK1', 'ARM_STACK2', \
        'ARM_SAT', 'ARM_SYNC', 'ARM_PUSR', 'ARM_ADR'

    for description in darmtbl.ARMv7:
        instr = description[0]
        bits = description[1:]

        identifier = []
        remainder = []
        for x in range(1 if bits[0] == darmtbl.cond else 4, len(bits)):
            if isinstance(bits[x], int):
                identifier.append(str(bits[x]))
            elif len(identifier) + bits[x].bitsize > 8:
                identifier += ['01'] * (8-len(identifier))
                remainder = bits[x:]
            else:
                identifier += ['01'] * bits[x].bitsize

        # first handle all unconditional instructions, i.e., whitelist those
        # instructions that have already been implemented
        if bits[:4] == (1, 1, 1, 1) and \
                bits[4:7] in ((0, 0, 0), (0, 1, 0), (0, 1, 1), (1, 0, 1)):
            # hardcoded index for the T_UNCOND type encoding
            instr_types[2][-1].append(instr)
            continue

        for x in itertools.product(*identifier[:8]):
            idx = sum(int(x[y])*2**(7-y) for y in range(8))

            # for each conditional instruction, check which type of
            # instruction this is
            for y in instr_types:
                if y[0] == 1 and bits[0] == d.cond and y[4](bits, instr, idx):
                    if not y[1] in type_ignore:
                        armv7_table[idx] = instruction_name(instr), y
                    y[-1].append(instr)
                    break

    for description in darmtbl2.thumbs:
        instr = description[0]
        bits = description[1:]

        bitcount = sum(1 if isinstance(x, int) else x.bitsize for x in bits)
        if bitcount in (16, 17, 18, 19):
            # TODO fix >16
            identifier, remainder = [], []
            for x in range(len(bits)):
                if isinstance(bits[x], int):
                    identifier.append(str(bits[x]))
                elif len(identifier) + bits[x].bitsize > 8:
                    identifier += ['01'] * (8-len(identifier))
                    remainder = bits[x:]
                else:
                    identifier += ['01'] * bits[x].bitsize
            for x in itertools.product(*identifier[:8]):
                idx = sum(int(x[y])*2**(7-y) for y in range(8))
                for y in (_ for _ in instr_types if _[0] == 2):
                    if y[4](bits, instr, 0):
                        thumb_table[idx] = instruction_name(instr), y
                        # debug matching
                        print instr, thumb_table[idx]
                        y[-1].append(instr)

    # make a list of unique instructions affected by each encoding type,
    # we remove the first item from the instruction names, as this is I_INVLD
    instr_types = [x[:5] + [instruction_names(x[5])[1:]] for x in instr_types]

    #
    # darm-tbl.h
    #

    magic_open('darm-tbl.h')

    fmtstrs = generate_format_strings(darmtbl.ARMv7)
    # until we remove all unused instructions..
    instrcnt = len(open('instructions.txt').readlines())

    # print required headers
    print('#ifndef __DARM_TBL__')
    print('#define __DARM_TBL__')
    print('#include <stdint.h>')

    # print type info for each encoding type
    print(type_encoding_enum('darm_enctype', instr_types))

    # print all instruction labels
    print(instruction_names_enum(open('instructions.txt')))
    count = len(instruction_names(open('instructions.txt')))
    print('extern const char *darm_mnemonics[%d];' % count)
    print('extern const char *darm_enctypes[%d];' % len(instr_types))
    print('extern const char *darm_registers[16];')

    # define constants 0b0 up upto 0b11111111
    for x in range(256):
        print('#define %s %d' % (bin(x)[1:], x))

    # define partial constants with leading zeroes, such as 0b0001
    for x in range(2, 7):
        for y in itertools.product('01', repeat=x):
            num = ''.join(y)
            print('#define b%s %d' % (num, int(num, 2)))

    print('#endif')

    #
    # thumb-tbl.h
    #

    magic_open('thumb-tbl.h')

    # print required headers
    print('#ifndef __THUMB_TBL__')
    print('#define __THUMB_TBL__')
    print('#include <stdint.h>')
    print('#include "darm-tbl.h"')

    # print some required definitions
    print('extern darm_enctype_t thumb_instr_types[256];')
    print('extern darm_instr_t thumb_instr_labels[256];')
    print('extern const char *thumb_registers[9];')

    thumb_fmtstrs = generate_format_strings(darmtbl2.thumbs)
    print('const char *thumb_format_strings[%d][3];' % instrcnt)
    print('#endif')

    #
    # armv7-tbl.h
    #

    magic_open('armv7-tbl.h')

    # print required headers
    print('#ifndef __ARMV7_TBL__')
    print('#define __ARMV7_TBL__')
    print('#include <stdint.h>')
    print('#include "darm-tbl.h"')

    # print some required definitions
    print('extern darm_enctype_t armv7_instr_types[256];')
    print('extern darm_enctype_t thumb2_instr_types[256];')
    def type_lut(name, bits):
        print('darm_instr_t type_%s_instr_lookup[%d];' % (name, 2**bits))

    print('extern darm_instr_t armv7_instr_labels[256];')
    type_lut('shift', 4)
    type_lut('brnchmisc', 4)
    type_lut('opless', 3)
    type_lut('uncond2', 3)
    type_lut('mul', 3)
    type_lut('stack0', 5)
    type_lut('stack1', 3)
    type_lut('stack2', 3)
    type_lut('bits', 2)
    type_lut('pas', 6)
    type_lut('sat', 2)
    type_lut('sync', 4)
    type_lut('pusr', 4)
    print('const char *armv7_format_strings[%d][3];' % instrcnt)

    print('#endif')

    #
    # darm-tbl.c
    #

    magic_open('darm-tbl.c')
    print('#include <stdio.h>')
    print('#include <stdint.h>')
    print('#include "darm-tbl.h"')
    print(instruction_names_table(open('instructions.txt')))
    print(type_encoding_table('darm_enctypes', instr_types))

    reg = 'r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 SP LR PC'
    print(string_table('darm_registers', reg.split()))

    #
    # thumb-tbl.c
    #

    magic_open('thumb-tbl.c')
    print('#include <stdio.h>')
    print('#include <stdint.h>')
    print('#include "thumb-tbl.h"')

    reg = 'r0 r1 r2 r3 r4 r5 r6 r7 LR'
    print(string_table('thumb_registers', reg.split()))

    # print a table containing all the types of instructions
    print(instruction_types_table(thumb_table, 'thumb'))

    # print a table containing the instruction label for each entry
    print(instruction_names_index_table(thumb_table, 'thumb'))

    lines = []
    for instr, fmtstr in thumb_fmtstrs.items():
        fmtstr = ', '.join('"%s"' % x for x in set(fmtstr))
        lines.append('    [I_%s] = {%s},' % (instr, fmtstr))
    print('const char *thumb_format_strings[%d][3] = {' % instrcnt)
    print('\n'.join(sorted(lines)))
    print('};')

    #
    # armv7-tbl.c
    #

    magic_open('armv7-tbl.c')
    print('#include <stdio.h>')
    print('#include <stdint.h>')
    print('#include "armv7-tbl.h"')

    # print a table containing all the types of instructions
    print(instruction_types_table(armv7_table, 'armv7'))

    # print a table containing the instruction label for each entry
    print(instruction_names_index_table(armv7_table, 'armv7'))

    # print a lookup table for the shift type (which is a sub-type of
    # the dst-src type), the None types represent instructions of the
    # STR family, which we'll handle in the next handler, T_STR.
    t_shift = {
        0b0000: 'lsl',
        0b0001: 'lsl',
        0b0010: 'lsr',
        0b0011: 'lsr',
        0b0100: 'asr',
        0b0101: 'asr',
        0b0110: 'ror',
        0b0111: 'ror',
        0b1000: 'lsl',
        0b1001: None,
        0b1010: 'lsr',
        0b1011: None,
        0b1100: 'asr',
        0b1101: None,
        0b1110: 'ror',
        0b1111: None}

    print(type_lookup_table('type_shift',
                            *[t_shift[x] for x in range(16)]))

    t4 = 'msr', 'bx', 'bxj', 'blx', None, 'qsub', None, 'bkpt', 'smlaw', \
        None, 'smulw', None, 'smlaw', None, 'smulw', None
    print(type_lookup_table('type_brnchmisc', *t4))

    t_opless = 'nop', 'yield', 'wfe', 'wfi', 'sev', None, None, None
    print(type_lookup_table('type_opless', *t_opless))

    t_uncond2 = None, 'clrex', None, None, 'dsb', 'dmb', 'isb', None
    print(type_lookup_table('type_uncond2', *t_uncond2))

    t_mul = 'mul', 'mla', 'umaal', 'mls', 'umull', 'umlal', \
        'smull', 'smlal'
    print(type_lookup_table('type_mul', *t_mul))

    t_stack0 = {
        0b00000: 'str',
        0b00001: 'ldr',
        0b00010: 'strt',
        0b00011: 'ldrt',
        0b00100: 'strb',
        0b00101: 'ldrb',
        0b00110: 'strbt',
        0b00111: 'ldrbt',
        0b01000: 'str',
        0b01001: 'ldr',
        0b01010: 'strt',
        0b01011: 'ldrt',
        0b01100: 'strb',
        0b01101: 'ldrb',
        0b01110: 'strbt',
        0b01111: 'ldrbt',
        0b10000: 'str',
        0b10001: 'ldr',
        0b10010: 'str',
        0b10011: 'ldr',
        0b10100: 'strb',
        0b10101: 'ldrb',
        0b10110: 'strb',
        0b10111: 'ldrb',
        0b11000: 'str',
        0b11001: 'ldr',
        0b11010: 'str',
        0b11011: 'ldr',
        0b11100: 'strb',
        0b11101: 'ldrb',
        0b11110: 'strb',
        0b11111: 'ldrb',
    }

    print(type_lookup_table('type_stack0',
                            *[t_stack0[x] for x in range(32)]))

    t_stack1 = None, None, 'strht', 'ldrht', None, 'ldrsbt', \
        None, 'ldrsht'
    print(type_lookup_table('type_stack1', *t_stack1))

    t_stack2 = None, None, 'strh', 'ldrh', 'ldrd', 'ldrsb', \
        'strd', 'ldrsh'
    print(type_lookup_table('type_stack2', *t_stack2))

    print(type_lookup_table('type_bits', None, 'sbfx', 'bfi', 'ubfx'))

    t_pas = {
        0b000: 'add16',
        0b001: 'asx',
        0b010: 'sax',
        0b011: 'sub16',
        0b100: 'add8',
        0b111: 'sub8',
    }
    t_pas_prefix = 's', 'q', 'sh', 'u', 'uq', 'uh'
    t_pas = dict(((1 + (idx > 2) + idx) * 2**3 + k, x + v)
                 for idx, x in enumerate(t_pas_prefix)
                 for k, v in t_pas.items())
    print(type_lookup_table('type_pas',
                            *[t_pas.get(x) for x in range(64)]))

    print(type_lookup_table('type_sat', 'qadd', 'qsub', 'qdadd', 'qdsub'))

    t_sync = 'swp', None, None, None, 'swpb', None, None, None, \
        'strex', 'ldrex', 'strexd', 'ldrexd', 'strexb', 'ldrexb', \
        'strexh', 'ldrexh'
    print(type_lookup_table('type_sync', *t_sync))

    t_pusr = 'sxtab16', 'sxtb16', None, None, 'sxtab', 'sxtb', \
        'sxtah', 'sxth', 'uxtab16', 'uxtb16', None, None, \
        'uxtab', 'uxtb', 'uxtah', 'uxth'
    print(type_lookup_table('type_pusr', *t_pusr))

    lines = []
    for instr, fmtstr in fmtstrs.items():
        fmtstr = ', '.join('"%s"' % x for x in set(fmtstr))
        lines.append('    [I_%s] = {%s},' % (instr, fmtstr))
    print('const char *armv7_format_strings[%d][3] = {' % instrcnt)
    print('\n'.join(sorted(lines)))
    print('};')
