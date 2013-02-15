import textwrap


def instruction_name(x):
    return x.split('{')[0].split('<')[0].split()[0]


def instruction_names(arr):
    """List of all unique instruction names."""
    return sorted(set(instruction_name(x[0]) for x in arr))


def instruction_names_enum(arr):
    """Enumeration of all instruction names."""
    text = ', '.join('I_%s' % x for x in instruction_names(arr))
    text = '\n    '.join(textwrap.wrap(text, 74))
    return 'typedef enum {\n    %s\n} armv7_instr_t;' % text


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
