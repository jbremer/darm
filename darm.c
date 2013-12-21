#include <stdio.h>
#include <stdint.h>
#include "darm.h"
#include "darm-tables.h"

typedef enum _darm_sm_opcode_t {
    // Halt execution, invalid instruction.
    SM_HLT,

    // Follow either branch of a node, depending on the particular bit
    // in the instruction.
    SM_STEP,

    // This instruction has been disassembled correctly, return success.
    SM_RETN,

    // Extract an immediate.
    SM_IMM,

    // Extract various general purpose registers.
    SM_Rd, SM_Rn, SM_Rm, SM_Ra, SM_Rt, SM_Rt2, SM_RdHi, SM_RdLo, SM_Rs,
} darm_sm_opcode_t;

static inline uint32_t _extract_field(uint32_t insn,
    uint32_t idx, uint32_t bits)
{
    return (insn >> idx) & ((1 << bits) - 1);
}

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

        case SM_RETN:
            printf("dump.. :)\n");
            return 0;

        case SM_IMM:
            d->imm = _extract_field(insn, 0, sm[off+1]);
            off += 2;
            break;

#define SM_REG(name) \
        case SM_##name: \
            d->name = _extract_field(insn, sm[off+1], 4); \
            off += 2; \
            break;

        SM_REG(Rd); SM_REG(Rn); SM_REG(Rm); SM_REG(Ra); SM_REG(Rt);
        SM_REG(Rt2); SM_REG(RdHi); SM_REG(RdLo); SM_REG(Rs);
        }
    }
    return 0;
}

int darm_armv7(darm_t *d, uint32_t insn)
{
    return darm_disassemble(d, insn, g_armv7_sm, g_armv7_lut);
}
