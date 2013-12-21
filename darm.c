#include <stdio.h>
#include <stdint.h>
#include "darm-tables.h"

typedef struct _darm_t {
} darm_t;

typedef enum _darm_sm_opcode_t {
    // Halt execution, invalid instruction.
    SM_HLT,

    // Follow either branch of a node, depending on the particular bit
    // in the instruction.
    SM_STEP,

    SM_DUMP,
} darm_sm_opcode_t;

// Disassembles any instruction according to the state machine and lookup
// table. The input instruction can be either 16 or 32 bit, depending on
// the target state machine (which will have been created accordingly.)
static int darm_disassemble(darm_t *d, uint32_t insn,
    const uint8_t *sm, const uint16_t *lut)
{
    uint32_t off = 0, value;
    while (1) {
        switch (sm[off]) {
        case SM_HLT:
            printf("hlt..\n");
            return -1;

        case SM_STEP:
            value = (insn >> sm[off+1]) & 1;
            printf("leaf.. bit %d -> %d\n", sm[off+1], value);
            off = lut[sm[off+2] + sm[off+3]*256 + value];
            break;

        case SM_DUMP:
            printf("dump.. :)\n");
            return 0;
        }
    }
    return 0;
}

int darm_armv7(darm_t *d, uint32_t insn)
{
    return darm_disassemble(d, insn, g_armv7_sm, g_armv7_lut);
}
