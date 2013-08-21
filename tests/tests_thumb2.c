#include <stdio.h>
#include <string.h>
#include "../darm.h"
#include "../thumb2.h"
#include "tests.h"

struct {
    uint32_t w;
    int r;
    const char *s;
    darm_t d;
} thumb2_tests[] = {

    // we switch to thumb2
    {0, 0, NULL, {.instr = I_INVLD}},

    {0xbe03, 0, "adc", {
        .instr = I_BKPT, .instr_type = T_THUMB_ONLY_IMM8, .cond = C_AL,
        .I = B_SET, .imm = 3}},

};

// Test the internal functions of thumb2
int test_thumb2_functions() {

    int failure = 0;

    uint16_t value1 = 0xFE;

    // Zero extend to 32 bits
    if (thumb_expand_imm(value1) != value1) {
        print_failure("Zero expand in thumb_expand_imm failed");
        failure = 1;
    }

    // Bit 0:7 interleaved with 0
    if (thumb_expand_imm(0x155) != 0x550055) {
        print_failure("Bit interleaving in thumb_expand_imm failed");
        failure = 1;
    }

    // Bit 0:7 interleaved with 0
    if (thumb_expand_imm(0x255) != 0x55005500) {
        print_failure("Bit interleaving in thumb_expand_imm failed");
        failure = 1;
    }

    // Bit 0:7 concatenated
    if (thumb_expand_imm(0x355) != 0x55555555) {
        print_failure("Bit concatenation in thumb_expand_imm failed");
        failure = 1;
    }

    if (thumb_expand_imm(0xC55) != 0xD500) {
        print_failure("Bit rotate in thumb_expand_imm failed");
        failure = 1;
    }


    if (failure == 0) 
        print_success("Passed thumb2 helper function tests");
    
    return failure;
}

