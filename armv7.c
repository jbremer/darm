#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "darm.h"

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
