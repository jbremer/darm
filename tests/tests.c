#include <stdio.h>
#include <string.h>
#include "../darm.h"

struct {
    uint32_t w;
    int r;
    darm_t d;
} tests[] = {
    {0xe0a10082, 0, {
        .instr = I_ADC, .instr_type = T_ARITH_SHIFT, .cond = 0b1110, .S = 0,
        .Rd = 0, .Rn = 1, .Rm = 2, .type = 0, .shift_is_reg = 0, .shift = 1}},
    {0xe2821003, 0, {
        .instr = I_ADD, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .Rn = 2, .imm = 3}},
    {0xe257502a, 0, {
        .instr = I_SUB, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 1,
        .Rd = 5, .Rn = 7, .imm = 42}},
    {0xe04d2007, 0, {
        .instr = I_SUB, .instr_type = T_ARITH_SHIFT, .cond = 0b1110, .S = 0,
        .Rd = 2, .Rn = SP, .Rm = 7}},
    {0xe28d4001, 0, {
        .instr = I_ADD, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 4, .Rn = SP, .imm = 1}},
    {0xe28f4030, 0, {
        .instr = I_ADR, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 4, .imm = 48, .U = 1}},
    {0xe24f3000, 0, {
        .instr = I_ADR, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 3, .imm = 0, .U = 0}},
    {0xe24f329a, 0, {
        .instr = I_ADR, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 3, .imm = 666, .U = 0}},
    {0xe1a02458, 0, {
        .instr = I_ASR, .instr_type = T_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 2, .Rm = 4, .Rn = 8, .type = 2}},
    {0xe1a02f03, 0, {
        .instr = I_LSL, .instr_type = T_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 2, .shift = 30, .Rm = 3}},
    {0xeb00014e, 0, {
        .instr = I_BL, .instr_type = T_BRNCHSC, .cond = 0b1110, .imm = 1336}},
    {0xeaffff00, 0, {
        .instr = I_B, .instr_type = T_BRNCHSC, .cond = 0b1110, .imm = -1024}},
    {0xef000001, 0, {
        .instr = I_SVC, .instr_type = T_BRNCHSC, .cond = 0b1110, .imm = 1}},
    {0xe1200071, 0, {
        .instr = I_BKPT, .instr_type = T_BRNCHMISC, .cond = 0b1110,
        .imm = 1}},
    {0xe12fff14, 0, {
        .instr = I_BX, .instr_type = T_BRNCHMISC, .cond = 0b1110, .Rm = 4}},
    {0xe3e04020, 0, {
        .instr = I_MVN, .instr_type = T_MOV_IMM, .cond = 0b1110, .Rd = 4,
        .imm = 32}},
    {0xe1140505, 0, {
        .instr = I_TST, .instr_type = T_CMP_OP, .cond = 0b1110, .Rn = 4,
        .Rm = 5, .shift_is_reg = 0, .type = 0, .shift = 10}},
    {0xe15a017b, 0, {
        .instr = I_CMP, .instr_type = T_CMP_OP, .cond = 0b1110, .Rn = 10,
        .Rm = 11, .type = 3, .shift_is_reg = 1, .Rs = 1}},
    {0xe357029a, 0, {
        .instr = I_CMP, .instr_type = T_CMP_IMM, .cond = 0b1110, .Rn = 7,
        .imm = 666}},
    {0xe320f000, 0, {.instr = I_NOP, .instr_type = T_OPLESS, .cond = 0b1110}},
    {0xe320f003, 0, {.instr = I_WFI, .instr_type = T_OPLESS, .cond = 0b1110}},
    {0xe1a0c064, 0, {
        .instr = I_RRX, .instr_type = T_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 12, .shift = 0, .Rm = 4, .type = 3}},
    {0xe3a013e8, 0, {
        .instr = I_MOV, .instr_type = T_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .imm = 1000}},
    {0xe3012f40, 0, {
        .instr = I_MOVW, .instr_type = T_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 2, .imm = 8000}},
    {0xe1a00000, 0, {
        .instr = I_NOP, .instr_type = T_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 0, .Rm = 0}},
    {0xf57ff01f, 0, {
        .instr = I_CLREX, .instr_type = T_UNCOND, .cond = 0b1111}},
    {0xf57ff04c, 0, {
        .instr = I_DSB, .instr_type = T_UNCOND, .cond = 0b1111,
        .option = 12}},
    {0xf1010200, 0, {
        .instr = I_SETEND, .instr_type = T_UNCOND, .cond = 0b1111, .E = 1}}
};

int main()
{
    int failure = 0;
    for (uint32_t i = 0; i < ARRAYSIZE(tests); i++) {
        darm_t d; int ret;

        ret = armv7_disassemble(&d, tests[i].w);

        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

        if(ret != tests[i].r || memcmp(&tests[i].d, &d, sizeof(darm_t))) {
            printf("incorrect encoding for 0x%08x\n", d.w);

            const char *mnemonic = armv7_mnemonic_by_index(d.instr);
            const char *enctype = armv7_enctype_by_index(d.instr_type);

            printf(
                "ret: %d, instr: I_%s, instr-type: T_%s, cond: %d, S: %d\n"
                "Rd: %d, Rn: %d, Rm: %d, op-imm: %d\n"
                "type: %d, shift-is-reg: %d, Rs: %d, shift: %d\n"
                "E: %d, option: %d, U: %d\n",
                ret, mnemonic, enctype, d.cond, d.S, d.Rd, d.Rn, d.Rm,
                d.imm, d.type, d.shift_is_reg, d.Rs, d.shift,
                d.E, d.option, d.U);
            failure = 1;
        }
    }
    if(failure == 0) {
        printf("[x] unittests were successful\n");
    }
    return failure;
}
