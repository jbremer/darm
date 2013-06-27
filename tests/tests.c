#include <stdio.h>
#include <string.h>
#include "../darm.h"

struct {
    uint32_t w;
    int r;
    darm_t d;
} tests[] = {
    {0xe0a13082, 0, {
        .instr = I_ADC, .instr_type = T_ARM_ARITH_SHIFT, .cond = 0b1110,
        .S = 0, .Rd = 3, .Rn = 1, .Rm = 2, .shift_type = S_LSL, .shift = 1}},
    {0xe2821003, 0, {
        .instr = I_ADD, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .Rn = 2, .imm = 3, .I = B_SET}},
    {0xe257502a, 0, {
        .instr = I_SUB, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 1,
        .Rd = 5, .Rn = 7, .imm = 42, .I = B_SET}},
    {0xe04d2007, 0, {
        .instr = I_SUB, .instr_type = T_ARM_ARITH_SHIFT, .cond = 0b1110,
        .S = 0, .Rd = 2, .Rn = SP, .Rm = 7}},
    {0xe28d4001, 0, {
        .instr = I_ADD, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 4, .Rn = SP, .imm = 1, .I = B_SET}},
    {0xe28f4030, 0, {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 4, .imm = 48, .I = B_SET, .U = 1}},
    {0xe24f3000, 0, {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 3, .imm = 0, .I = B_SET, .U = 0}},
    {0xe24f3210, 0, {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = 0b1110, .S = 0,
        .Rd = 3, .imm = 1, .I = B_SET, .U = 0}},
    {0xe1a02458, 0, {
        .instr = I_ASR, .instr_type = T_ARM_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 2, .Rm = 4, .Rn = 8, .shift_type = S_ASR}},
    {0xe1a02f03, 0, {
        .instr = I_LSL, .instr_type = T_ARM_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 2, .shift = 30, .Rm = 3}},
    {0xeb00014e, 0, {
        .instr = I_BL, .instr_type = T_ARM_BRNCHSC, .cond = 0b1110,
        .imm = 1336, .I = B_SET}},
    {0xeaffff00, 0, {
        .instr = I_B, .instr_type = T_ARM_BRNCHSC, .cond = 0b1110,
        .imm = -1024, .I = B_SET}},
    {0xef000001, 0, {
        .instr = I_SVC, .instr_type = T_ARM_BRNCHSC, .cond = 0b1110,
        .imm = 1, .I = B_SET}},
    {0xe1200071, 0, {
        .instr = I_BKPT, .instr_type = T_ARM_BRNCHMISC, .cond = 0b1110,
        .imm = 1, .I = B_SET}},
    {0xe12fff14, 0, {
        .instr = I_BX, .instr_type = T_ARM_BRNCHMISC, .cond = C_AL, .Rm = 4}},
    {0xe3e04020, 0, {
        .instr = I_MVN, .instr_type = T_ARM_MOV_IMM, .cond = 0b1110, .Rd = 4,
        .imm = 32, .I = B_SET}},
    {0xe1140505, 0, {
        .instr = I_TST, .instr_type = T_ARM_CMP_OP, .cond = 0b1110, .Rn = 4,
        .Rm = 5, .shift_type = S_LSL, .shift = 10}},
    {0xe15a017b, 0, {
        .instr = I_CMP, .instr_type = T_ARM_CMP_OP, .cond = 0b1110, .Rn = 10,
        .Rm = 11, .shift_type = S_ROR, .Rs = 1}},
    {0xe35704f0, 0, {
        .instr = I_CMP, .instr_type = T_ARM_CMP_IMM, .cond = 0b1110, .Rn = 7,
        .imm = 0xf0000000, .I = B_SET}},
    {0xe320f000, 0, {.instr = I_NOP, .instr_type = T_ARM_OPLESS,
        .cond = 0b1110}},
    {0xe320f003, 0, {.instr = I_WFI, .instr_type = T_ARM_OPLESS,
        .cond = 0b1110}},
    {0xe1a0c064, 0, {
        .instr = I_RRX, .instr_type = T_ARM_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 12, .shift = 0, .Rm = 4, .shift_type = S_ROR}},
    {0xe3a013e8, 0, {
        .instr = I_MOV, .instr_type = T_ARM_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .imm = 0xa0000003, .I = B_SET}},
    {0xe3012f40, 0, {
        .instr = I_MOVW, .instr_type = T_ARM_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 2, .imm = 8000, .I = B_SET}},
    {0xe1a00000, 0, {
        .instr = I_NOP, .instr_type = T_ARM_DST_SRC, .cond = 0b1110, .S = 0,
        .Rd = 0, .Rm = 0}},
    {0xf57ff01f, 0, {
        .instr = I_CLREX, .instr_type = T_ARM_UNCOND, .cond = 0b1111}},
    {0xf57ff04c, 0, {
        .instr = I_DSB, .instr_type = T_ARM_UNCOND, .cond = 0b1111,
        .option = 12}},
    {0xf1010200, 0, {
        .instr = I_SETEND, .instr_type = T_ARM_UNCOND, .cond = 0b1111,
        .E = 1}},
    {0xf4d2f001, 0, {
        .instr = I_PLI, .instr_type = T_ARM_UNCOND, .cond = 0b1111, .U = 1,
        .Rn = 2, .imm = 1, .I = B_SET}},
    {0xfb000000, 0, {
        .instr = I_BLX, .instr_type = T_ARM_UNCOND, .cond = 0b1111, .H = 1,
        .imm = 2, .I = B_SET}},
    {0xe3a01a01, 0, {
        .instr = I_MOV, .instr_type = T_ARM_MOV_IMM, .cond = 0b1110, .S = 0,
        .Rd = 1, .imm = 0x1000, .I = B_SET}},
    {0xe0140298, 0, {
        .instr = I_MUL, .instr_type = T_ARM_MUL, .cond = 0b1110, .S = 1,
        .Rd = 4, .Rm = 2, .Rn = 8}},
    {0x00976592, 0, {
        .instr = I_UMULL, .instr_type = T_ARM_MUL, .cond = C_EQ, .S = 1,
        .RdHi = 7, .RdLo = 6, .Rm = 5, .Rn = 2}},
    // for now, when failing, some info might still be set, so for now we'll
    // hardcode the incorrect info as well..
    {0xe0712394, -1, {
        .instr = I_MLS, .instr_type = T_ARM_MUL, .cond = 0b1110, .S = 1,
        .Rn = 4, .Rm = 3}},
    {0xe7932384, 0, {
        .instr = I_LDR, .instr_type = T_ARM_STACK0, .cond = 0b1110, .Rt = 2,
        .Rn = 3, .Rm = 4, .P = 1, .W = 0, .U = 1,
        .shift_type = S_LSL, .shift = 7}},
    {0xe6e386c5, 0, {
        .instr = I_STRBT, .instr_type = T_ARM_STACK0, .cond = 0b1110, .Rn = 3,
        .Rt = 8, .shift = 13, .shift_type = S_ASR, .Rm = 5,
        .U = 1, .P = 0, .W = 1}},
    {0x90b480b3, 0, {
        .instr = I_LDRHT, .instr_type = T_ARM_STACK1, .cond = C_LS, .U = 1,
        .Rn = 4, .Rt = 8, .Rm = 3, .P = 0}},
    {0xe0c42fdf, 0, {
        .instr = I_LDRD, .instr_type = T_ARM_STACK2, .cond = 0b1110, .U = 1,
        .P = 0, .W = 0, .R = 0, .Rn = 4, .Rt = 2, .imm = 0xff, .I = B_SET}},
    {0xe7c8411f, 0, {
        .instr = I_BFC, .instr_type = T_ARM_BITS, .cond = 0b1110, .Rd = 4,
        .lsb = 2, .width = 6}},
    {0xe7e42153, 0, {
        .instr = I_UBFX, .instr_type = T_ARM_BITS, .cond = 0b1110, .Rd = 2,
        .Rn = 3, .lsb = 2, .width = 5}},
    {0xe52d4004, 0, {
        .instr = I_PUSH, .instr_type = T_ARM_STACK0, .cond = 0b1110,
        .Rn = SP, .Rt = 4, .imm = 4, .I = B_SET, .U = 0, .P = 1, .W = 1}},
    {0xe49d7004, 0, {
        .instr = I_POP, .instr_type = T_ARM_STACK0, .cond = 0b1110,
        .Rn = SP, .Rt = 7, .imm = 4, .I = B_SET, .U = 1, .P = 0, .W = 0}},
    {0xe8bd00f6, 0, {
        .instr = I_POP, .instr_type = T_ARM_LDSTREGS, .cond = 0b1110,
        .Rn = SP, .W = 1, .reglist = 0b11110110}},
    {0xe6ff4f32, 0, {
        .instr = I_RBIT, .instr_type = T_ARM_BITREV, .cond = 0b1110,
        .Rd = 4, .Rm = 2}},
    {0xe6ff4fb2, 0, {
        .instr = I_REVSH, .instr_type = T_ARM_BITREV, .cond = 0b1110,
        .Rd = 4, .Rm = 2}},
    {0xe6842351, 0, {
        .instr = I_PKH, .instr_type = T_ARM_MISC, .cond = 0b1110,
        .Rn = 4, .Rd = 2, .Rm = 1, .shift_type = S_ASR, .shift = 6, .T = 1}},
    {0xe1e041c1, 0, {
        .instr = I_MVN, .instr_type = T_ARM_MISC, .cond = 0b1110,
        .Rd = 4, .shift = 3, .shift_type = S_ASR, .Rm = 1}},
    {0xe1600075, 0, {
        .instr = I_SMC, .instr_type = T_ARM_MISC, .cond = 0b1110,
        .imm = 5, .I = B_SET}},
    {0xe16601e8, 0, {
        .instr = I_SMUL, .instr_type = T_ARM_SM, .cond = 0b1110, .Rd = 6,
        .Rm = 1, .M = 1, .N = 1, .Rn = 8}},
    {0xe7042113, 0, {
        .instr = I_SMLAD, .instr_type = T_ARM_SM, .cond = 0b1110, .Rd = 4,
        .Ra = 2, .Rm = 1, .M = 0, .Rn = 3}},
    {0xe75123d4, 0, {
        .instr = I_SMMLS, .instr_type = T_ARM_SM, .cond = 0b1110, .Rd = 1,
        .Ra = 2, .Rm = 3, .R = 0, .Rn = 4}},
    {0xe5242000, 0, {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = 0b1110, .Rn = 4,
        .Rt = 2, .P = 1, .W = 1, .U = 0, .imm = 0, .I = B_SET}},
    {0xe5a350f0, 0, {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = 0b1110, .Rn = 3,
        .Rt = 5, .P = 1, .W = 1, .U = 1, .imm = 0xf0, .I = B_SET}},
    {0xa6112f53, 0, {
        .instr = I_SSAX, .instr_type = T_ARM_PAS, .cond = 0b1010, .Rn = 1,
        .Rd = 2, .Rm = 3}},
    {0xe6614ff5, 0, {
        .instr = I_UQSUB8, .instr_type = T_ARM_PAS, .cond = 0b1110, .Rn = 1,
        .Rd = 4, .Rm = 5}},
    {0xe1018052, 0, {
        .instr = I_QADD, .instr_type = T_ARM_SAT, .cond = 0b1110, .Rn = 1,
        .Rd = 8, .Rm = 2}},
    {0xe1c12f94, 0, {
        .instr = I_STREXB, .instr_type = T_ARM_SYNC, .cond = 0b1110,
        .Rn = 1, .Rd = 2, .Rt = 4}},
    {0xe1413094, 0, {
        .instr = I_SWPB, .instr_type = T_ARM_SYNC, .cond = 0b1110,
        .Rn = 1, .Rt = 3, .Rt2 = 4, .B = 1}},
    {0xe6e141d2, 0, {
        .instr = I_USAT, .instr_type = T_ARM_PUSR, .cond = 0b1110,
        .imm = 1, .I = B_SET, .Rd = 4, .shift = 3, .shift_type = S_ASR,
        .Rn = 2}},
    {0xe6b21474, 0, {
        .instr = I_SXTAH, .instr_type = T_ARM_PUSR, .cond = 0b1110,
        .Rn = 2, .Rd = 1, .rotate = 0b1000, .Rm = 4}},
    {0xe0f12394, 0, {
        .instr = I_SMLAL, .instr_type = T_ARM_MUL, .cond = 0b1110, .S = 1,
        .RdHi = 1, .RdLo = 2, .Rm = 3, .Rn = 4}},
    {0xe5a43000, 0, {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = 0b1110, .S = 0,
        .P = B_SET, .U = B_SET, .W = B_SET, .Rn = 4, .Rt = 3, .I = B_SET,
        .imm = 0}},
    {0xe5a43003, 0, {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = 0b1110, .S = 0,
        .P = B_SET, .U = B_SET, .W = B_SET, .Rn = 4, .Rt = 3, .I = B_SET,
        .imm = 3}},
    {0xe28fc601, 0, {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL,
        .S = B_UNSET, .U = B_SET, .I = B_SET, .imm = 0x100000, .Rd = 12}},

    // we switch to thumb (oboy)
    {0, 0, {.instr = I_INVLD}},

    {0xbe03, 0, {
        .instr = I_BKPT, .instr_type = T_THUMB_ONLY_IMM8, .I = B_SET,
        .cond = C_AL, .imm = 3}},
    {0xb5f0, 0, {
        .instr = I_PUSH, .instr_type = T_THUMB_PUSHPOP, .cond = C_AL,
        .reglist=0b100000011110000}},
    {0x264c, 0, {
        .instr = I_MOVS, .instr_type = T_THUMB_REG_IMM, .I = B_SET,
        .cond = C_AL, .Rd = 6, .imm = 0x4c}},
    {0x1ae3, 0, {
        .instr = I_SUBS, .instr_type = T_THUMB_ARITH_REG_REG, .cond = C_AL,
        .Rd = 3, .Rn = 4, .Rm = 3}},
};

static int _darm_thumb_disasm(darm_t *d, uint32_t w)
{
    return darm_thumb_disasm(d, w);
}

static int _darm_thumb2_disasm(darm_t *d, uint32_t w)
{
    return darm_thumb2_disasm(d, w >> 16, w & 0xffff);
}

int main()
{
    int failure = 0;

    int disasm_index = 0;
    int (*disasms[])(darm_t *d, uint32_t w) = {
        &darm_armv7_disasm, &_darm_thumb_disasm, &_darm_thumb2_disasm,
    };

    for (uint32_t i = 0; i < ARRAYSIZE(tests); i++) {
        darm_t d; int ret;

        if(tests[i].w == 0) {
            disasm_index++;
            continue;
        }

        // update the registers in the tests in order not to be 0, but R_INVLD
        // instead
        darm_t *p = &tests[i].d;
        if(p->Rd == 0) p->Rd = R_INVLD;
        if(p->Rn == 0) p->Rn = R_INVLD;
        if(p->Rm == 0) p->Rm = R_INVLD;
        if(p->Ra == 0) p->Ra = R_INVLD;
        if(p->Rt == 0) p->Rt = R_INVLD;
        if(p->Rt2 == 0) p->Rt2 = R_INVLD;
        if(p->RdHi == 0) p->RdHi = R_INVLD;
        if(p->RdLo == 0) p->RdLo = R_INVLD;
        if(p->Rs == 0) p->Rs = R_INVLD;
        if(p->option == 0) p->option = O_INVLD;

        if(p->shift_type == S_LSL && p->Rs == R_INVLD && p->shift == 0) {
            p->shift_type = S_INVLD;
        }

        ret = disasms[disasm_index](&d, tests[i].w);

        darm_str_t str;
        memset(&str, 0, sizeof(str));
        if(darm_str2(&d, &str, 1) == 0) {
            printf("%s\n", str.instr);
            fflush(stdout);
        }
        else if(ret == 0) {
            printf("error decoding instr..\n");
        }


        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

#define C(x) p->x != d.x

    // if d.x, the output from the disasm, is invalid or unset, then the set
    // value must be unset, otherwise, it must be set (this is because we
    // can't specify B_INVLD in the unittests without making everything
    // unreadable)
#define F(x) \
    ((d.x == B_INVLD || d.x == B_UNSET) ? p->x != B_UNSET : p->x != B_SET)

        // enter ugly code
        if(ret != tests[i].r || C(w) || C(instr) || C(instr_type) ||
                C(cond) || F(S) || F(E) || C(option) || F(U) || F(H) ||
                F(P) || F(R) || F(W) || C(Rd) || C(Rn) || C(Rm) || C(Ra) ||
                C(Rt) || C(RdHi) || C(RdLo) || F(I) || C(imm) ||
                C(shift_type) || C(Rs) || C(shift) || C(lsb) ||
                C(width) || C(reglist) || F(T) || F(M) || F(N) ||
                C(Rt2) || F(B)) {
            // leave ugly code
            printf("incorrect encoding for 0x%08x, ret %d\n", d.w, ret);
            printf("decoded:\n");
            darm_dump(&d);
            printf("test values:\n");
            darm_dump(&tests[i].d);
            failure = 1;
        }
    }
    if(failure == 0) {
        printf("[x] unittests were successful\n");
    }
    return failure;
}
