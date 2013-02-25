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
    return enum_table('armv7_instr',
                      ('I_%s' % x for x in instruction_names(arr)))


def instruction_names_table(arr):
    """Table of strings of all instructions."""
    return string_table('armv7_mnemonics', instruction_names(arr))


def instruction_types_table(arr):
    """Lookup table for the types of instructions."""
    arr = ['T_%s' % arr[x][1][0] if x in arr else 'T_INVLD'
           for x in xrange(256)]
    return typed_table('armv7_enctype_t', 'armv7_instr_types', arr)


def instruction_names_index_table(arr):
    """Lookup table for instruction label for each instruction index."""
    arr = ['I_%s' % arr[x][0] if x in arr else 'I_INVLD'
           for x in xrange(256)]
    return typed_table('armv7_instr_t', 'armv7_instr_labels', arr)


def type_lookup_table(name, *args):
    """Create a lookup table for a certain instruction type."""
    arr = ('I_%s' % x.upper() if x else 'I_INVLD' for x in args)
    return typed_table('armv7_instr_t', '%s_instr_lookup' % name, arr)


def type_encoding_enum(enumname, arr):
    text = []
    for name, info, encodings, _, affects in arr:
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
    return string_table(tblname, (x[0] for x in arr))


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
        '<Rn>', 'n',
        '<Rm>', 'm',
        '<Ra>', 'a',
        '<Rt>', 't',
        '<Rt2>', '2',
        '<RdHi>', 'h',
        '<RdLo>', 'l',

        # immediate values
        '#<const>', 'i',
        '#<imm4>', 'i',
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

d = darmtbl

# we specify various instruction types
cond_instr_types = [
    ('INVLD', 'Invalid or non-existent type',
     ['I_INVLD'], lambda x, y, z: False),
    ('UNCOND', 'All unconditional instructions',
     ['ins <endian_specifier>', 'ins [<Rn>,#+/-<imm12>]',
     'ins [<Rn>,#<imm12>]', 'ins', 'ins #<option>', 'ins <label>'],
     lambda x, y, z: False),
    ('MUL', 'All multiplication instructions',
     ['ins{S}<c> <Rd>,<Rn>,<Rm>', 'ins{S}<c> <Rd>,<Rn>,<Rm>,<Ra>',
     'ins{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>'],
     lambda x, y, z: x[1:5] == (0, 0, 0, 0) and x[-5:-1] == (1, 0, 0, 1)),
    ('STACK0', 'Various STR and LDR instructions',
     ['ins<c> <Rt>,[<Rn>,#+/-<imm12>]', 'ins<c> <Rt>,[<Rn>],#+/-<imm12>',
     'ins<c> <Rt>,[<Rn>],+/-<Rm>{,<shift>}'],
     lambda x, y, z: x[1:3] == (0, 1) and not (x[3] == 1 and x[-2] == 1)),
    ('STACK1', 'Various unprivileged STR and LDR instructions',
     ['ins<c> <Rt>,[<Rn>],+/-<Rm>', 'ins<c> <Rt>,[<Rn>]{,#+/-<imm8>}'],
     lambda x, y, z: x[-5] == 1 and x[-2] == 1 and x[-4:-2] != (0, 0) and
        x[1:5] == (0, 0, 0, 0) and x[7] == 1),
    ('STACK2', 'Various other STR and LDR instructions',
     ['ins<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>',
      'ins<c> <Rt>,[<Rn>],+/-<Rm>',
      'ins<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>',
      'ins<c> <Rt>,<Rt2>,[<Rn>,#+/-<imm8>]',
      'ins<c> <Rt>,[<Rn>,#+/-<imm8>]', ],
     lambda x, y, z: x[1:4] == (0, 0, 0) and x[-2] == 1 and x[-5] == 1 and
        x[-4:-2] != (0, 0) and not x[-1] in (0, 1) and
        not (x[4] == 0 and x[7] == 1)),
    ('ARITH_SHIFT',
     'Arithmetic instructions which take a shift for the second source',
     ['ins{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}',
      'ins{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>'],
     lambda x, y, z: d.Rn in x and d.Rd in x and x[-3] == d.type_
     and x[-1] == d.Rm),
    ('ARITH_IMM',
     'Arithmetic instructions which take an immediate as second source',
     ['ins{S}<c> <Rd>,<Rn>,#<const>'],
     lambda x, y, z: d.Rn in x and d.Rd in x and d.imm12 in x),
    ('BITS', 'Bit field magic',
     [],
     lambda x, y, z: d.lsb in x),
    ('BRNCHSC', 'Branch and System Call instructions',
     ['B(L)<c> <label>', 'SVC<c> #<imm24>'],
     lambda x, y, z: x[-1] == d.imm24),
    ('BRNCHMISC', 'Branch and Misc instructions',
     ['B(L)X(J)<c> <Rm>', 'BKPT #<imm16>', 'MSR<c> <spec_reg>,<Rn>'],
     lambda x, y, z: x[1:9] == (0, 0, 0, 1, 0, 0, 1, 0)),
    ('MOV_IMM', 'Move immediate to a register (possibly negating it)',
     ['ins{S}<c> <Rd>,#<const>'],
     lambda x, y, z: x[-1] == d.imm12 and x[-2] == d.Rd),
    ('CMP_OP', 'Comparison instructions which take two operands',
     ['ins<c> <Rn>,<Rm>{,<shift>}', 'ins<c> <Rn>,<Rm>,<type> <Rs>'],
     lambda x, y, z: x[-1] == d.Rm and x[-3] == d.type_ and
        (x[-4] == d.imm5 and x[-8:-4] == (0, 0, 0, 0) or
         x[-5] == d.Rs and x[-9:-5] == (0, 0, 0, 0))),
    ('CMP_IMM', 'Comparison instructions which take an immediate',
     ['ins<c> <Rn>,#<const>'],
     lambda x, y, z: x[-1] == d.imm12 and x[-6] == d.Rn),
    ('OPLESS', 'Instructions which don\'t take any operands',
     ['ins<c>'],
     lambda x, y, z: len(x) == 29),
    ('DST_SRC', 'Manipulate and move a register to another register',
     ['ins{S}<c> <Rd>,<Rm>', 'ins{S}<c> <Rd>,<Rm>,#<imm>',
     'ins{S}<c> <Rd>,<Rn>,<Rm>'],
     lambda x, y, z: z == 26 or z == 27),
    ('LDSTREGS', 'Load or store multiple registers at once',
     ['ins<c> <Rn>{!},<registers>'],
     lambda x, y, z: x[-1] == d.register_list),
    ('BITREV', 'Bit reverse instructions',
     ['ins<c> <Rd>,<Rm>'],
     lambda x, y, z: x[-1] == d.Rm and x[-10] == d.Rd and x[-11] != d.Rn),
    ('MISC', 'Various miscellaneous instructions',
     ['ins{S}<c> <Rd>,<Rm>,<type> <Rs>', 'ins{S}<c> <Rd>,<Rm>{,<shift>}',
     'ins<c> #<imm4>', 'ins<c> #<option>',
     'ins<c> <Rd>,<Rn>,<Rm>{,<type> #<imm>}', 'ins<c> <Rd>,<Rn>,<Rm>'],
     lambda x, y, z: instruction_name(y) in ('MVN', 'SMC', 'DBG', 'PKH',
                                             'SEL')),
    ('SM', 'Various signed multiply instructions', [],
     lambda x, y, z: y[:2] == 'SM'),
    ('PAS', 'Parallel signed and unsigned addition and subtraction',
     ['ins<c> <Rd>,<Rn>,<Rm>'],
     lambda x, y, z: z in (97, 98, 99, 101, 102, 103)),
]

if __name__ == '__main__':
    uncond_table = {}
    cond_table = {}

    # the last item (a list) will contain the instructions affected by this
    # encoding type
    cond_instr_types = [list(x) + [[]] for x in cond_instr_types]

    # list of encoding types which should not be emitted in the table (because
    # they are handled somewhere else, in a somewhat hardcoded fashion)
    type_ignore = 'MUL', 'STACK0', 'STACK1', 'STACK2'

    for description in darmtbl.ARMv7:
        instr = description[0]
        bits = description[1:]

        identifier = []
        remainder = []
        for x in xrange(1 if bits[0] == darmtbl.cond else 4, len(bits)):
            if isinstance(bits[x], int):
                identifier.append(str(bits[x]))
            elif len(identifier) + bits[x].bitsize > 8:
                identifier += ['01'] * (8-len(identifier))
                remainder = bits[x:]
                break
            else:
                identifier += ['01'] * bits[x].bitsize

        # first handle all unconditional instructions, i.e., whitelist those
        # instructions that have already been implemented
        if bits[:4] == (1, 1, 1, 1) and \
                bits[4:7] in ((0, 0, 0), (0, 1, 0), (0, 1, 1), (1, 0, 1)):
            # hardcoded index for the T_UNCOND type encoding
            cond_instr_types[1][-1].append(instr)
            continue

        for x in itertools.product(*identifier[:8]):
            idx = sum(int(x[y])*2**(7-y) for y in xrange(8))

            # for each conditional instruction, check which type of
            # instruction this is
            for y in cond_instr_types:
                if bits[0] == d.cond and y[3](bits, instr, idx):
                    if not y[0] in type_ignore:
                        cond_table[idx] = instruction_name(instr), y
                    y[-1].append(instr)
                    break

    # make a list of unique instructions affected by each encoding type,
    # we remove the first item from the instruction names, as this is I_INVLD
    cond_instr_types = [x[:4] + [instruction_names(x[4])[1:]]
                        for x in cond_instr_types]

    # python magic!
    sys.stdout = open(sys.argv[1], 'w')

    # print the license
    print '/*'
    print __doc__.strip()
    print '*/'
    print

    fmtstrs = generate_format_strings(darmtbl.ARMv7)
    # until we remove all unused instructions..
    instrcnt = len(open('instructions.txt').readlines())

    if sys.argv[1][-2:] == '.h':

        # print required headers
        print '#ifndef __ARMV7_TBL__'
        print '#define __ARMV7_TBL__'
        print
        print '#include <stdint.h>'
        print

        # print all instruction labels
        print instruction_names_enum(open('instructions.txt'))

        # print type info for each encoding type
        print type_encoding_enum('armv7_enctype', cond_instr_types)

        # print some required definitions
        print 'armv7_enctype_t armv7_instr_types[256];'

        def type_lut(name, bits):
            print 'armv7_instr_t type_%s_instr_lookup[%d];' % (name, 2**bits)

        print 'armv7_instr_t armv7_instr_labels[256];'
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
        count = len(instruction_names(open('instructions.txt')))
        print 'const char *armv7_mnemonics[%d];' % count
        print 'const char *armv7_enctypes[%d];' % len(cond_instr_types)
        print 'const char *armv7_registers[16];'
        print 'const char *armv7_format_strings[%d][3];' % instrcnt
        print

        print '#endif'
        print

    elif sys.argv[1][-2:] == '.c':

        # print a header
        print '#include <stdio.h>'
        print '#include <stdint.h>'
        print '#include "%s"' % (sys.argv[1][:-2] + '.h')
        print

        # print a table containing all the types of instructions
        print instruction_types_table(cond_table)

        # print a table containing the instruction label for each entry
        print instruction_names_index_table(cond_table)

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

        print type_lookup_table('type_shift',
                                *[t_shift[x] for x in xrange(16)])

        t4 = 'msr', 'bx', 'bxj', 'blx', None, 'qsub', None, 'bkpt', 'smlaw', \
            None, 'smulw', None, 'smlaw', None, 'smulw', None
        print type_lookup_table('type_brnchmisc', *t4)

        t_opless = 'nop', 'yield', 'wfe', 'wfi', 'sev', None, None, None
        print type_lookup_table('type_opless', *t_opless)

        t_uncond2 = None, 'clrex', None, None, 'dsb', 'dmb', 'isb', None
        print type_lookup_table('type_uncond2', *t_uncond2)

        t_mul = 'mul', 'mla', 'umaal', 'mls', 'umull', 'umlal', \
            'smull', 'smlal'
        print type_lookup_table('type_mul', *t_mul)

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

        print type_lookup_table('type_stack0',
                                *[t_stack0[x] for x in xrange(32)])

        t_stack1 = None, None, 'strht', 'ldrht', None, 'ldrsbt', \
            None, 'ldrsht'
        print type_lookup_table('type_stack1', *t_stack1)

        t_stack2 = None, None, 'strh', 'ldrh', 'ldrd', 'ldrsb', \
            'strd', 'ldrsh'
        print type_lookup_table('type_stack2', *t_stack2)

        print type_lookup_table('type_bits', None, 'sbfx', 'bfi', 'ubfx')

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
        print type_lookup_table('type_pas',
                                *[t_pas.get(x) for x in xrange(64)])

        print instruction_names_table(open('instructions.txt'))
        print type_encoding_table('armv7_enctypes', cond_instr_types)

        reg = 'r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 FP IP SP LR PC'
        print string_table('armv7_registers', reg.split())

        lines = []
        for instr, fmtstr in fmtstrs.items():
            fmtstr = ', '.join('"%s"' % x for x in set(fmtstr))
            lines.append('    [I_%s] = {%s},' % (instr, fmtstr))
        print 'const char *armv7_format_strings[%d][3] = {' % instrcnt
        print '\n'.join(sorted(lines))
        print '};'
