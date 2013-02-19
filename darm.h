#ifndef __DARM__
#define __DARM__

#include "armv7-tbl.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

typedef struct _darm_t {
    // the original encoded instruction
    uint32_t        w;

    // the instruction label
    armv7_instr_t   instr;
    uint32_t        instr_type;

    // conditional flags, if any
    uint32_t        cond;
    // does this instruction update the conditional flags?
    uint32_t        S;

    // register operands
    uint32_t        Rd; // destination
    uint32_t        Rn; // first operand
    uint32_t        Rm; // second operand

    // register shift info
    uint32_t        type;
    uint32_t        shift_is_reg; // shift from register Rs or not?
    uint32_t        Rs;
    uint32_t        shift;
} darm_t;

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

int armv7_disassemble(darm_t *d, uint32_t w);

#endif
