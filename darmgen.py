import textwrap


def instruction_names(arr):
    """List of all unique instruction names."""
    def name(x):
        return x.split('{')[0].split('<')[0].split()[0]

    return list(set(name(x[0]) for x in arr))


def instruction_names_enum(arr):
    """Enumeration of all instruction names."""
    text = ', '.join('I_%s' % x for x in arr)
    text = '\n    '.join(textwrap.wrap(text, 74))
    return 'typedef enum {\n    %s\n} armv7_instr_t;' % text


def instruction_names_table(arr):
    """Table of strings of all instructions."""
    text = ', '.join('"%s"' % x for x in arr)
    text = '\n    '.join(textwrap.wrap(text, 74))
    return 'const char *armv7_mnemonics[] = {\n    %s\n};' % text
