#include <stdio.h>
#include <string.h>
#include "../darm.h"

struct {
    uint32_t w;
    int r;
    darm_t d;
} tests[] = {
    {0xe0a10082, 0, {
        .instr = I_ADC, .instr_type = 0, .cond = 0b1110, .S = 0, .Rd = 0,
        .Rn = 1, .Rm = 2, .type = 0, .shift_is_reg = 0, .shift = 1}},
    {0xe2821003, 0, {
        .instr = I_ADD, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 1,
        .Rn = 2, .imm = 3}},
    {0xe257502a, 0, {
        .instr = I_SUB, .instr_type = 1, .cond = 0b1110, .S = 1, .Rd = 5,
        .Rn = 7, .imm = 42}},
    {0xe04d2007, 0, {
        .instr = I_SUB, .instr_type = 0, .cond = 0b1110, .S = 0, .Rd = 2,
        .Rn = SP, .Rm = 7}},
    {0xe28d4001, 0, {
        .instr = I_ADD, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 4,
        .Rn = SP, .imm = 1}},
    {0xe28f4030, 0, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 4,
        .imm = 48, .add = 1}},
    {0xe24f3000, 0, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 3,
        .imm = 0, .add = 0}},
    {0xe24f329a, 0, {
        .instr = I_ADR, .instr_type = 1, .cond = 0b1110, .S = 0, .Rd = 3,
        .imm = 666, .add = 0}},
    {0xe1a02458, 0, {
        .instr = I_ASR, .instr_type = 2, .cond = 0b1110, .S = 0, .Rd = 2,
        .Rm = 4, .Rn = 8}},
    {0xe1a02f03, 0, {
        .instr = I_LSL, .instr_type = 2, .cond = 0b1110, .S = 0, .Rd = 2,
        .imm = 30, .Rm = 3}},
    {0xeb00014e, 0, {
        .instr = I_BL, .instr_type = 3, .cond = 0b1110, .imm = 1336}},
    {0xeaffff00, 0, {
        .instr = I_B, .instr_type = 3, .cond = 0b1110, .imm = -1024}},
    {0xef000001, 0, {
        .instr = I_SVC, .instr_type = 3, .cond = 0b1110, .imm = 1}},
    {0xe1200071, 0, {
        .instr = I_BKPT, .instr_type = 4, .cond = 0b1110, .imm = 1}},
    {0xe12fff14, 0, {
        .instr = I_BX, .instr_type = 4, .cond = 0b1110, .Rm = 4}},
    {0xe3e04020, 0, {
        .instr = I_MVN, .instr_type = 5, .cond = 0b1110, .Rd = 4, .imm = 32}},
    {0xe1140505, 0, {
        .instr = I_TST, .instr_type = 6, .cond = 0b1110, .Rn = 4, .Rm = 5,
        .shift_is_reg = 0, .type = 0, .shift = 10}},
    {0xe15a017b, 0, {
        .instr = I_CMP, .instr_type = 6, .cond = 0b1110, .Rn = 10, .Rm = 11,
        .type = 3, .shift_is_reg = 1, .Rs = 1}},
    {0xe357029a, 0, {
        .instr = I_CMP, .instr_type = T_CMP_IMM, .cond = 0b1110, .Rn = 7,
        .imm = 666}},
};

int main(int argc, char *argv[])
{
    int failure = 0;
    for (int i = 0; i < ARRAYSIZE(tests); i++) {
        darm_t d = {}; int ret;

        ret = armv7_disassemble(&d, tests[i].w);

        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

        if(ret != tests[i].r || memcmp(&tests[i].d, &d, sizeof(darm_t))) {
            printf("incorrect encoding for 0x%08x\n", d.w);

            printf(
                "ret: %d, instr: %d, instr-type: %d, cond: %d, S: %d\n"
                "Rd: %d, Rn: %d, Rm: %d, op-imm: %d, add: %d\n"
                "type: %d, shift-is-reg: %d, Rs: %d, shift: %d\n",
                ret, d.instr, d.instr_type, d.cond, d.S, d.Rd, d.Rn, d.Rm,
                d.imm, d.add, d.type, d.shift_is_reg, d.Rs, d.shift);
            failure = 1;
        }
    }
    if(failure == 0) {
        printf("[x] unittests were successful\n");
    }
    return failure;
}
