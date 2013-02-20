import darmtbl
import sys
import textwrap


def instruction_name(x):
    return x.split('{')[0].split('<')[0].split()[0]


def instruction_names(arr):
    """List of all unique instruction names."""
    return sorted(set(instruction_name(x) for x in arr))


def instruction_names_enum(arr):
    """Enumeration of all instruction names."""
    text = ', '.join('I_%s' % x for x in instruction_names(arr))
    text = '\n    '.join(textwrap.wrap(text, 74))
    return 'typedef enum {\n    %s\n} armv7_instr_t;\n' % text


def instruction_names_table(arr):
    """Table of strings of all instructions."""
    text = ', '.join('"%s"' % x for x in instruction_names(arr))
    text = '\n    '.join(textwrap.wrap(text, 74))
    return 'const char *armv7_mnemonics[] = {\n    %s\n};' % text


def updates_condition_flags(arr):
    """List of all instructions that have the S flag."""
    return sorted(set(instruction_name(x[0]) for x in arr if darmtbl.S in x))


def updates_condition_flags_table(arr):
    """Lookup table returning True for instructions that have the S flag."""
    names = instruction_names(arr)
    flags = updates_condition_flags(arr)
    table = ', '.join(('1' if x in flags else '0') for x in names)
    text = '\n    '.join(textwrap.wrap(table, 74))
    return 'uint8_t updates_condition_flags[] = {\n    %s\n};' % text


def instruction_types_table(arr):
    """Lookup table for the types of instructions."""
    table = ', '.join(str(arr[x][0]) if x in arr else '-1'
                      for x in xrange(256))
    text = '\n    '.join(textwrap.wrap(table, 74))
    return 'uint8_t armv7_instr_types[] = {\n    %s\n};\n' % text


def instruction_names_index_table(arr):
    """Lookup table for instruction label for each instruction index."""
    table = ', '.join('I_%s' % str(arr[x][1]) if x in arr else '-1'
                      for x in xrange(256))
    text = '\n    '.join(textwrap.wrap(table, 74))
    return 'armv7_instr_t armv7_instr_labels[] = {\n    %s\n};\n' % text


def type_lookup_table(name, *args):
    """Create a lookup table for a certain instruction type."""
    table = ', '.join('I_%s' % x.upper() if x else '-1' for x in args)
    text = '\n    '.join(textwrap.wrap(table, 74))
    return 'armv7_instr_t %s_instr_lookup[] = {\n    %s\n};\n' % (name, text)


def type_encoding_info(enumname, arr):
    text = []
    for name, info, encodings, fn in arr:
        text.append(
            '    // info:\n' +
            '    // %s\n    //\n' % info +
            '    // encodings:\n    // ' +
            '\n    // '.join(encodings) + '\n' +
            '    T_%s,' % name)

    return 'typedef enum _%s_t {\n%s\n} %s_t;\n' % (enumname,
                                                    '\n\n'.join(text),
                                                    enumname)

d = darmtbl

# we specify various instruction types
cond_instr_types = [
    ('ARITH_SHIFT',
     'Arithmetic instructions which take a shift for the second source',
     ['ins{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}',
      'ins{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>'],
     lambda x: d.Rn in x and d.Rd in x and x[-3] == d.type_
     and x[-1] == d.Rm),
    ('ARITH_IMM',
     'Arithmetic instructions which take an immediate as second source',
     ['ins{S}<c> <Rd>,<Rn>,#<const>'],
     lambda x: d.Rn in x and d.Rd in x and d.imm12 in x),
    ('SHIFT', 'Shift instructions',
     ['ins{S}<c> <Rd>,<Rn>,<Rm>', 'ins{S}<c> <Rd>,<Rm>,#<imm>'],
     lambda x: x[-1] == d.Rn and x[-6] == d.Rm and x[-7] == d.Rd or
        x[-1] == d.Rm and x[-5] == d.imm5),
    ('BRNCHSC', 'Branch and System Call instructions',
     ['B(L)<c> <label>', 'SVC<c> #<imm24>'],
     lambda x: x[-1] == d.imm24),
    ('BRNCHMISC', 'Branch and Misc instructions',
     ['B(L)X(J)<c> <Rm>', 'BKPT #<imm16>', 'MSR<c> <spec_reg>,<Rn>'],
     lambda x: x[1:9] == (0, 0, 0, 1, 0, 0, 1, 0)),
    ('MOV_IMM', 'Move immediate to a register (possibly negating it)',
     ['ins{S}<c> <Rd>,#<const>'],
     lambda x: x[-1] == d.imm12 and x[-2] == d.Rd),
    ('CMP_OP', 'Comparison instructions which take two operands',
     ['ins<c> <Rn>,<Rm>{,<shift>}', 'ins<c> <Rn>,<Rm>,<type> <Rs>'],
     lambda x: x[-1] == d.Rm and x[-3] == d.type_ and
        (x[-4] == d.imm5 and x[-8:-4] == (0, 0, 0, 0) or
         x[-5] == d.Rs and x[-9:-5] == (0, 0, 0, 0))),
    ('CMP_IMM', 'Comparison instructions which take an immediate',
     ['ins<c> <Rn>,#<const>'],
     lambda x: x[-1] == d.imm12 and x[-6] == d.Rn),
]

if __name__ == '__main__':
    uncond_table = {}
    cond_table = {}
    for description in darmtbl.ARMv7:
        instr = description[0]
        bits = description[1:]

        identifier = []
        remainder = []
        for x in xrange(1 if bits[0] == darmtbl.cond else 4, len(bits)-1):
            if isinstance(bits[x], int):
                identifier.append(bits[x])
            else:
                remainder = bits[x:]
                break

        for x in xrange(2**max(8-len(identifier), 0)):
            idx = sum(identifier[y]*2**(7-y) for y in xrange(len(identifier)))
            idx = int(idx + x)

            # for each conditional instruction, check which type of
            # instruction this is
            for instr_idx, y in enumerate(cond_instr_types):
                if bits[0] == d.cond and y[3](bits):
                    cond_table[idx] = instr_idx, instruction_name(instr)

    # python magic!
    sys.stdout = open(sys.argv[1], 'w')

    if sys.argv[1][-2:] == '.h':

        # print required headers
        print '#ifndef __ARMV7_TBL__'
        print '#define __ARMV7_TBL__'
        print
        print '#include <stdint.h>'
        print

        # print all instruction labels
        print instruction_names_enum(open('instructions.txt'))

        # print some required definitions
        print 'uint8_t armv7_instr_types[256];'
        print 'armv7_instr_t armv7_instr_labels[256];'
        print 'armv7_instr_t type3_instr_lookup[4];'
        print 'armv7_instr_t type4_instr_lookup[16];'
        print

        # print type info for each encoding type
        print type_encoding_info('armv7_enctype', cond_instr_types)

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

        # print a lookup table for type3
        print type_lookup_table('type3', 'lsl', 'lsr', 'asr', 'ror')

        t4 = 'msr', 'bx', 'bxj', 'blx', None, 'qsub', None, 'bkpt', 'smlaw', \
            None, 'smulw', None, 'smlaw', None, 'smulw', None
        print type_lookup_table('type4', *t4)
