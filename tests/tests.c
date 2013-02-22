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
    {0xe24f3210, 0, {
        .instr = I_ADR, .instr_type = T_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 3, .imm = 1, .U = 0}},
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
    {0xe35704f0, 0, {
        .instr = I_CMP, .instr_type = T_CMP_IMM, .cond = 0b1110, .Rn = 7,
        .imm = 0xf0000000}},
    {0xe320f000, 0, {.instr = I_NOP, .instr_type = T_OPLESS, .cond = 0b1110}},
    {0xe320f003, 0, {.instr = I_WFI, .instr_type = T_OPLESS, .cond = 0b1110}},
    {0xe1a0c064, 0, {
        .instr = I_RRX, .instr_type = T_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 12, .shift = 0, .Rm = 4, .type = 3}},
    {0xe3a013e8, 0, {
        .instr = I_MOV, .instr_type = T_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .imm = 0xa0000003}},
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
        .instr = I_SETEND, .instr_type = T_UNCOND, .cond = 0b1111, .E = 1}},
    {0xf4d2f001, 0, {
        .instr = I_PLI, .instr_type = T_UNCOND, .cond = 0b1111, .U = 1,
        .Rn = 2, .imm = 1}},
    {0xfb000000, 0, {
        .instr = I_BLX, .instr_type = T_UNCOND, .cond = 0b1111, .H = 1,
        .imm = 2}},
    {0xe3a01a01, 0, {
        .instr = I_MOV, .instr_type = T_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .imm = 0x1000}},
    {0xe0140298, 0, {
        .instr = I_MUL, .instr_type = T_MUL, .cond = 0b1110, .S = 1,
        .Rd = 4, .Rm = 2, .Rn = 8}},
    {0x00976592, 0, {
        .instr = I_UMULL, .instr_type = T_MUL, .cond = C_EQ, .S = 1,
        .RdHi = 7, .RdLo = 6, .Rm = 5, .Rn = 2}},
    // for now, when failing, some info might still be set, so for now we'll
    // hardcode the incorrect info as well..
    {0xe0700090, -1, {
        .instr = I_MLS, .instr_type = T_MUL, .cond = 0b1110, .S = 1}},
    {0xe7932384, 0, {
        .instr = I_LDR, .instr_type = T_STACK0, .cond = 0b1110, .Rt = 2,
        .Rn = 3, .Rm = 4, .P = 1, .W = 0, .U = 1, .shift_is_reg = 1,
        .type = 0, .shift = 7}},
    {0xe6e386c5, 0, {
        .instr = I_STRBT, .instr_type = T_STACK0, .cond = 0b1110, .Rn = 3,
        .Rt = 8, .shift_is_reg = 1, .shift = 13, .type = 2, .Rm = 5,
        .U = 1, .P = 0, .W = 1}},
    {0x90b480b3, 0, {
        .instr = I_LDRHT, .instr_type = T_STACK1, .cond = C_LS, .U = 1,
        .Rn = 4, .Rt = 8, .Rm = 3, .P = 1, .R = 1}},
    {0xe0c42fdf, 0, {
        .instr = I_LDRD, .instr_type = T_STACK2, .cond = 0b1110, .U = 1,
        .P = 0, .W = 0, .R = 0, .Rn = 4, .Rt = 2, .imm = 0xff}},
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
            darm_dump(&d);
            failure = 1;
        }
    }
    if(failure == 0) {
        printf("[x] unittests were successful\n");
    }
    return failure;
}
