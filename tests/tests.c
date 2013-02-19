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
    {0xe2821003, {
        .instr = I_ADD, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 1,
        .Rn = 2, .op_imm = 3}},
    {0xe257502a, {
        .instr = I_SUB, .instr_type = 1, .cond = 0b1110, .S = 1, .Rd = 5,
        .Rn = 7, .op_imm = 42}},
    {0xe28d4001, {
        .instr = I_ADD, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 4,
        .Rn = SP, .op_imm = 1}},
    {0xe28f4030, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 4,
        .op_imm = 48, .add = 1}},
    {0xe24f3000, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 3,
        .op_imm = 0, .add = 0}},
    {0xe24f329a, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 3,
        .op_imm = 666, .add = 0}},
};

int main(int argc, char *argv[])
{
    int success = 0;
    for (int i = 0; i < ARRAYSIZE(tests); i++) {
        darm_t d = {}; int ret;

        ret = armv7_disassemble(&d, tests[i].w);

        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

        if(memcmp(&tests[i].d, &d, sizeof(darm_t))) {
            printf("incorrect encoding for 0x%08x\n", d.w);

            printf(
                "ret: %d, instr: %d, instr-type: %d, cond: %d, S: %d\n"
                "Rd: %d, Rn: %d, Rm: %d, op-imm: %d, add: %d\n"
                "type: %d, shift-is-reg: %d, Rs: %d, shift: %d\n",
                ret, d.instr, d.instr_type, d.cond, d.S, d.Rd, d.Rn, d.Rm,
                d.op_imm, d.add, d.type, d.shift_is_reg, d.Rs, d.shift);
            success = 1;
        }
    }
    if(success == 0) {
        printf("[x] unittests were successful\n");
    }
    return success;
}
