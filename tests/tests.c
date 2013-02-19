#include <stdio.h>
#include <string.h>
#include "../darm.h"

struct {
    uint32_t w;
    darm_t d;
} tests[] = {
    {0xe0a10082, {
        .instr = I_ADC, .instr_type = 0, .cond = 0b1110, .S = 0, .Rd = 0,
        .Rn = 1, .Rm = 2, .type = 0, .shift_is_reg = 0, .shift = 1}},
};

int main(int argc, char *argv[])
{
    for (int i = 0; i < ARRAYSIZE(tests); i++) {
        darm_t d = {}; int ret;

        ret = armv7_disassemble(&d, tests[i].w);

        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

        if(memcmp(&tests[i].d, &d, sizeof(darm_t))) {
            printf("incorrect encoding for 0x%08x\n", d.w);

            printf(
                "ret: %d, instr: %d, instr-type: %d, cond: %d, S: %d\n"
                "Rd: %d, Rn: %d, Rm: %d, op-imm: %d\n"
                "type: %d, shift-is-reg: %d, Rs: %d, shift: %d\n",
                ret, d.instr, d.instr_type, d.cond, d.S, d.Rd, d.Rn, d.Rm,
                d.op_imm, d.type, d.shift_is_reg, d.Rs, d.shift);
        }
    }
}
