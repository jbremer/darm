#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "darm.h"
#include "armv7-tbl.h"

struct {
    const char *mnemonic_extension;
    const char *meaning_integer;
    const char *meaning_fp;
} g_condition_codes[] = {
    {"EQ", "Equal", "Equal"},
    {"NE", "Not equal", "Not equal, or unordered"},
    {"CS", "Carry Set", "Greater than, equal, or unordered"},
    {"CC", "Carry Clear", "Less than"},
    {"MI", "Minus, negative", "Less than"},
    {"PL", "Plus, positive or zero", "Greater than, equal, or unordered"},
    {"VS", "Overflow", "Unordered"},
    {"VC", "No overflow", "Not unordered"},
    {"HI", "Unsigned higher", "Greater than, unordered"},
    {"LS", "Unsigned lower or same", "Greater than, or unordered"},
    {"GE", "Signed greater than or equal", "Greater than, or unordered"},
    {"LT", "Signed less than", "Less than, or unordered"},
    {"GT", "Signed greater than", "Greater than"},
    {"LE", "Signed less than or equal", "Less than, equal, or unordered"},
    {"AL", "Always (unconditional)", "Always (unconditional)"},

    // alias for CS
    {"HS", "Carry Set", "Greater than, equal, or unordered"},
    // alias for CC
    {"LO", "Carry Clear", "Less than"},
};

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_fp,
    int omit_always_mnemonic)
{
    if(condition_flag < 0 || condition_flag > 0b1110) return NULL;

    if(meaning_integer != NULL) {
        *meaning_integer = g_condition_codes[condition_flag].meaning_integer;
    }

    if(meaning_fp != NULL) {
        *meaning_fp = g_condition_codes[condition_flag].meaning_fp;
    }

    // the "AL" mnemonic extension can be omitted
    if(omit_always_mnemonic != 0 && condition_flag == 0b1110) {
        return "";
    }

    // return the mnemonic extension
    return g_condition_codes[condition_flag].mnemonic_extension;
}

int armv7_condition_index(const char *condition_code)
{
    if(condition_code == NULL) return -1;

    // the "AL" condition flag
    if(condition_code[0] == 0) return 0b1110;

    for (int i = 0; i < ARRAYSIZE(g_condition_codes); i++) {
        if(!strcmp(condition_code, g_condition_codes[i].mnemonic_extension)) {
            return i;
        }
    }

    return -1;
}

static const char *shift_types[] = {
    "LSL", "LSR", "ASR", "ROR",
};

void armv7_shift_decode(darm_t *d, const char **type, uint32_t *immediate)
{
    if(d->type == 0 && d->Rs == 0) {
        *type = NULL, *immediate = 0;
    }
    else if(d->type == 0b11 && d->Rs == 0) {
        *type = "RRX", *immediate = 0;
    }
    else {
        *type = shift_types[d->type];
        *immediate = d->Rs;

        // 32 is encoded as 0
        if((d->type == 0b01 || d->type == 0b10) && d->Rs == 0) {
            *immediate = 32;
        }
    }
}

static int armv7_disas_cond(darm_t *d, uint32_t w)
{
    // the instruction label
    d->instr = armv7_instr_labels[(w >> 20) & 0xff];

    // do a lookup for the type of instruction
    switch (armv7_instr_types[(w >> 20) & 0xff]) {
    case 0:
        d->S = (w >> 20) & 1;
        d->Rd = (w >> 12) & 0b1111;
        d->Rn = (w >> 16) & 0b1111;
        d->Rm = w & 0b1111;
        d->type = (w >> 5) & 0b11;

        // type == 1, shift with the value of the lower bits of Rs
        d->shift_reg = (w >> 4) & 1;
        if(d->shift_reg != 0) {
            d->Rs = (w >> 8) & 0b1111;
        }
        else {
            d->shift = (w >> 7) & 0b11111;
        }
        return 0;
    }
    return -1;
}

int armv7_disassemble(darm_t *d, uint32_t w)
{
    int ret = -1;

    d->cond = (w >> 28) & 0b1111;

    if(d->cond == 0b1111) {
        // TODO handle unconditional instructions
    }
    else {
        ret = armv7_disas_cond(d, w);
    }

    // return error
    if(ret < 0) return ret;

    // TODO

    return 0;
}
