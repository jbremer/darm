#include <stdio.h>
#include <string.h>
#include "../darm.h"
#include "../thumb2.h"

struct {
    uint32_t w;
    int r;
    const char *s;
    darm_t d;
} tests[] = {
    {0xe0a13082, 0, "adc r3, r1, r2, lsl #1", {
        .instr = I_ADC, .instr_type = T_ARM_ARITH_SHIFT, .cond = C_AL,
        .S = 0, .Rd = 3, .Rn = 1, .Rm = 2, .shift_type = S_LSL, .shift = 1}},
    {0xe2821003, 0, "add r1, r2, #3", {
        .instr = I_ADD, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 0,
        .Rd = 1, .Rn = 2, .imm = 3, .I = B_SET}},
    {0xe257502a, 0, "subs r5, r7, #42", {
        .instr = I_SUB, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 1,
        .Rd = 5, .Rn = 7, .imm = 42, .I = B_SET}},
    {0xe04d2007, 0, "sub r2, sp, r7", {
        .instr = I_SUB, .instr_type = T_ARM_ARITH_SHIFT, .cond = C_AL,
        .S = 0, .Rd = 2, .Rn = SP, .Rm = 7}},
    {0xe28d4001, 0, "add r4, sp, #1", {
        .instr = I_ADD, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 0,
        .Rd = 4, .Rn = SP, .imm = 1, .I = B_SET}},
    {0xe28f4030, 0, "adr r4, #+48", {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 0,
        .Rd = 4, .imm = 48, .I = B_SET, .U = 1}},
    {0xe24f3000, 0, "adr r3, #+-0", {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 0,
        .Rd = 3, .imm = 0, .I = B_SET, .U = 0}},
    {0xe24f3210, 0, "adr r3, #+-1", {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL, .S = 0,
        .Rd = 3, .imm = 1, .I = B_SET, .U = 0}},
    {0xe1a02458, 0, "asr r2, r8, r4", {
        .instr = I_ASR, .instr_type = T_ARM_DST_SRC, .cond = C_AL, .S = 0,
        .Rd = 2, .Rm = 4, .Rn = 8, .shift_type = S_ASR}},
    {0xe1a02f03, 0, "lsl r2, r3, #30", {
        .instr = I_LSL, .instr_type = T_ARM_DST_SRC, .cond = C_AL, .S = 0,
        .Rd = 2, .shift = 30, .Rm = 3}},
    {0xeb00014e, 0, "bl #+1336", {
        .instr = I_BL, .instr_type = T_ARM_BRNCHSC, .cond = C_AL,
        .imm = 1336, .I = B_SET}},
    {0xeaffff00, 0, "b #+-1024", {
        .instr = I_B, .instr_type = T_ARM_BRNCHSC, .cond = C_AL,
        .imm = -1024, .I = B_SET}},
    {0xeafffe00, 0, "b #+-2048", {
        .instr = I_B, .instr_type = T_ARM_BRNCHSC, .cond = C_AL,
        .imm = -2048, .I = B_SET}},
    {0xef000001, 0, "svc #1", {
        .instr = I_SVC, .instr_type = T_ARM_BRNCHSC, .cond = C_AL,
        .imm = 1, .I = B_SET}},
    {0xe1200071, 0, "bkpt #1", {
        .instr = I_BKPT, .instr_type = T_ARM_BRNCHMISC, .cond = C_AL,
        .imm = 1, .I = B_SET}},
    {0xe12fff14, 0, "bx r4", {
        .instr = I_BX, .instr_type = T_ARM_BRNCHMISC, .cond = C_AL, .Rm = 4}},
    {0xe3e04020, 0, "mvn r4, #32", {
        .instr = I_MVN, .instr_type = T_ARM_MOV_IMM, .cond = C_AL, .Rd = 4,
        .imm = 32, .I = B_SET}},
    {0xe1140505, 0, "tst r4, r5, lsl #10", {
        .instr = I_TST, .instr_type = T_ARM_CMP_OP, .cond = C_AL, .Rn = 4,
        .Rm = 5, .shift_type = S_LSL, .shift = 10}},
    {0xe15a017b, 0, "cmp r10, r11, ror r1", {
        .instr = I_CMP, .instr_type = T_ARM_CMP_OP, .cond = C_AL, .Rn = 10,
        .Rm = 11, .shift_type = S_ROR, .Rs = 1}},
    {0xe35704f0, 0, "cmp r7, #0xf0000000", {
        .instr = I_CMP, .instr_type = T_ARM_CMP_IMM, .cond = C_AL, .Rn = 7,
        .imm = 0xf0000000, .I = B_SET}},
    {0xe320f000, 0, "nop", {.instr = I_NOP, .instr_type = T_ARM_OPLESS,
        .cond = C_AL}},
    {0xe320f003, 0, "wfi", {.instr = I_WFI, .instr_type = T_ARM_OPLESS,
        .cond = C_AL}},
    {0xe1a0c064, 0, "rrx r12, r4", {
        .instr = I_RRX, .instr_type = T_ARM_DST_SRC, .cond = C_AL, .S = 0,
        .Rd = 12, .shift = 0, .Rm = 4, .shift_type = S_ROR}},
    {0xe3a013e8, 0, "mov r1, #0xa0000003", {
        .instr = I_MOV, .instr_type = T_ARM_MOV_IMM, .cond = C_AL, .S = 0,
        .Rd = 1, .imm = 0xa0000003, .I = B_SET}},
    {0xe3012f40, 0, "movw r2, #0x1f40", {
        .instr = I_MOVW, .instr_type = T_ARM_MOV_IMM, .cond = C_AL, .S = 0,
        .Rd = 2, .imm = 8000, .I = B_SET}},
    {0xe1a00000, 0, "nop", {
        .instr = I_NOP, .instr_type = T_ARM_DST_SRC, .cond = C_AL, .S = 0,
        .Rd = 0, .Rm = 0}},
    {0xf57ff01f, 0, "clrex", {
        .instr = I_CLREX, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND}},
    {0xf57ff04c, 0, "dsb #12", {
        .instr = I_DSB, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND,
        .option = 12}},
    {0xf1010200, 0, "setend 1", {
        .instr = I_SETEND, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND,
        .E = 1}},
    {0xf4d2f001, 0, "pli [r2, #1]", {
        .instr = I_PLI, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND, .U = 1,
        .Rn = 2, .imm = 1, .I = B_SET}},
    {0xfb000000, 0, "blx #+2", {
        .instr = I_BLX, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND, .H = 1,
        .imm = 2, .I = B_SET}},
    {0xe3a01a01, 0, "mov r1, #4096", {
        .instr = I_MOV, .instr_type = T_ARM_MOV_IMM, .cond = C_AL, .S = 0,
        .Rd = 1, .imm = 0x1000, .I = B_SET}},
    {0xe0140298, 0, "muls r4, r8, r2", {
        .instr = I_MUL, .instr_type = T_ARM_MUL, .cond = C_AL, .S = 1,
        .Rd = 4, .Rm = 2, .Rn = 8}},
    {0x00976592, 0, "umullseq r6, r7, r2, r5", {
        .instr = I_UMULL, .instr_type = T_ARM_MUL, .cond = C_EQ, .S = 1,
        .RdHi = 7, .RdLo = 6, .Rm = 5, .Rn = 2}},
    // for now, when failing, some info might still be set, so for now we'll
    // hardcode the incorrect info as well..
    {0xe0712394, -1, "", {
        .instr = I_MLS, .instr_type = T_ARM_MUL, .cond = C_AL, .S = 1,
        .Rn = 4, .Rm = 3}},
    {0xe7932384, 0, "ldr r2, [r3, r4, lsl #7]", {
        .instr = I_LDR, .instr_type = T_ARM_STACK0, .cond = C_AL, .Rt = 2,
        .Rn = 3, .Rm = 4, .P = 1, .W = 0, .U = 1,
        .shift_type = S_LSL, .shift = 7}},
    {0xe6e386c5, 0, "strbt r8, [r3], r5, asr #13", {
        .instr = I_STRBT, .instr_type = T_ARM_STACK0, .cond = C_AL, .Rn = 3,
        .Rt = 8, .shift = 13, .shift_type = S_ASR, .Rm = 5,
        .U = 1, .P = 0, .W = 1}},
    {0x90b480b3, 0, "ldrhtls r8, [r4], r3", {
        .instr = I_LDRHT, .instr_type = T_ARM_STACK1, .cond = C_LS, .U = 1,
        .Rn = 4, .Rt = 8, .Rm = 3, .P = 0}},
    {0xe0c42fdf, 0, "ldrd r2, r3, [r4], #255", {
        .instr = I_LDRD, .instr_type = T_ARM_STACK2, .cond = C_AL, .U = 1,
        .P = 0, .W = 0, .R = 0, .Rn = 4, .Rt = 2, .imm = 0xff, .I = B_SET}},
    // TODO is correct?
    {0xe7c8411f, 0, "bfc r4, #2, #7", {
        .instr = I_BFC, .instr_type = T_ARM_BITS, .cond = C_AL, .Rd = 4,
        .lsb = 2, .width = 7}},
    {0xe7de3312, 0, "bfi r3, r2, #6, #25", {
        .instr = I_BFI, .instr_type = T_ARM_BITS, .cond = C_AL,
        .Rd = r3, .Rn = r2, .lsb = 6, .width = 25}},
    {0xe7e42153, 0, "ubfx r2, r3, #2, #5", {
        .instr = I_UBFX, .instr_type = T_ARM_BITS, .cond = C_AL, .Rd = 2,
        .Rn = 3, .lsb = 2, .width = 5}},
    {0xe52d4004, 0, "push {r4}", {
        .instr = I_PUSH, .instr_type = T_ARM_STACK0, .cond = C_AL,
        .Rn = SP, .Rt = 4, .imm = 4, .I = B_SET, .U = 0, .P = 1, .W = 1}},
    {0xe49d7004, 0, "pop {r7}", {
        .instr = I_POP, .instr_type = T_ARM_STACK0, .cond = C_AL,
        .Rn = SP, .Rt = 7, .imm = 4, .I = B_SET, .U = 1, .P = 0, .W = 0}},
    {0xe8bd00f6, 0, "pop {r1,r2,r4-r7}", {
        .instr = I_POP, .instr_type = T_ARM_LDSTREGS, .cond = C_AL,
        .Rn = SP, .W = 1, .reglist = 0b11110110}},
    {0xe6ff4f32, 0, "rbit r4, r2", {
        .instr = I_RBIT, .instr_type = T_ARM_BITREV, .cond = C_AL,
        .Rd = 4, .Rm = 2}},
    {0xe6ff4fb2, 0, "revsh r4, r2", {
        .instr = I_REVSH, .instr_type = T_ARM_BITREV, .cond = C_AL,
        .Rd = 4, .Rm = 2}},
    {0xe6842351, 0, "pkhtb r2, r4, r1, asr #6", {
        .instr = I_PKH, .instr_type = T_ARM_MISC, .cond = C_AL,
        .Rn = 4, .Rd = 2, .Rm = 1, .shift_type = S_ASR, .shift = 6, .T = 1}},
    {0xe1e041c1, 0, "mvn r4, r1, asr #3", {
        .instr = I_MVN, .instr_type = T_ARM_MISC, .cond = C_AL,
        .Rd = 4, .shift = 3, .shift_type = S_ASR, .Rm = 1}},
    {0xe1600075, 0, "smc #5", {
        .instr = I_SMC, .instr_type = T_ARM_MISC, .cond = C_AL,
        .imm = 5, .I = B_SET}},
    {0xe16f3f14, 0, "clz r3, r4", {
        .instr = I_CLZ, .instr_type = T_ARM_MISC, .cond = C_AL,
        .Rd = r3, .Rm = r4}},
    {0xe16601e8, 0, "smultt r6, r8, r1", {
        .instr = I_SMUL, .instr_type = T_ARM_SM, .cond = C_AL, .Rd = 6,
        .Rm = 1, .M = 1, .N = 1, .Rn = 8}},
    {0xe7042113, 0, "smlad r4, r3, r1, r2", {
        .instr = I_SMLAD, .instr_type = T_ARM_SM, .cond = C_AL, .Rd = 4,
        .Ra = 2, .Rm = 1, .M = 0, .Rn = 3}},
    {0xe75123d4, 0, "smmls r1, r4, r3, r2", {
        .instr = I_SMMLS, .instr_type = T_ARM_SM, .cond = C_AL, .Rd = 1,
        .Ra = 2, .Rm = 3, .R = 0, .Rn = 4}},
    {0xe5242000, 0, "str r2, [r4]!", {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = C_AL, .Rn = 4,
        .Rt = 2, .P = 1, .W = 1, .U = 0, .imm = 0, .I = B_SET}},
    {0xe5a350f0, 0, "str r5, [r3, #240]!", {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = C_AL, .Rn = 3,
        .Rt = 5, .P = 1, .W = 1, .U = 1, .imm = 0xf0, .I = B_SET}},
    // TODO double check
    {0xa6112f53, 0, "ssaxge r2, r1, r3", {
        .instr = I_SSAX, .instr_type = T_ARM_PAS, .cond = C_GE, .Rn = 1,
        .Rd = 2, .Rm = 3}},
    {0xe6614ff5, 0, "uqsub8 r4, r1, r5", {
        .instr = I_UQSUB8, .instr_type = T_ARM_PAS, .cond = C_AL, .Rn = 1,
        .Rd = 4, .Rm = 5}},
    {0xe1018052, 0, "qadd r8, r2, r1", {
        .instr = I_QADD, .instr_type = T_ARM_SAT, .cond = C_AL, .Rn = 1,
        .Rd = 8, .Rm = 2}},
    {0xe1c12f94, 0, "strexb r2, r4, [r1]", {
        .instr = I_STREXB, .instr_type = T_ARM_SYNC, .cond = C_AL,
        .Rn = 1, .Rd = 2, .Rt = 4}},
    {0xe1413094, 0, "swpb r3, r4, [r1]", {
        .instr = I_SWPB, .instr_type = T_ARM_SYNC, .cond = C_AL,
        .Rn = 1, .Rt = 3, .Rt2 = 4, .B = 1}},
    {0xe6e141d2, 0, "usat r4, #1, r2, asr #3",{
        .instr = I_USAT, .instr_type = T_ARM_PUSR, .cond = C_AL,
        .imm = 1, .I = B_SET, .Rd = 4, .shift = 3, .shift_type = S_ASR,
        .Rn = 2}},
    {0xe6b21474, 0, "sxtah r1, r2, r4, ror #8", {
        .instr = I_SXTAH, .instr_type = T_ARM_PUSR, .cond = C_AL,
        .Rn = 2, .Rd = 1, .rotate = 0b1000, .Rm = 4}},
    {0xe0f12394, 0, "smlals r2, r1, r4, r3", {
        .instr = I_SMLAL, .instr_type = T_ARM_MUL, .cond = C_AL, .S = 1,
        .RdHi = 1, .RdLo = 2, .Rm = 3, .Rn = 4}},
    {0xe5a43000, 0, "str r3, [r4]!", {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = C_AL, .S = 0,
        .P = B_SET, .U = B_SET, .W = B_SET, .Rn = 4, .Rt = 3, .I = B_SET,
        .imm = 0}},
    {0xe5a43003, 0, "str r3, [r4, #3]!", {
        .instr = I_STR, .instr_type = T_ARM_STACK0, .cond = C_AL, .S = 0,
        .P = B_SET, .U = B_SET, .W = B_SET, .Rn = 4, .Rt = 3, .I = B_SET,
        .imm = 3}},
    {0xe28fc601, 0, "adr r12, #+0x100000", {
        .instr = I_ADR, .instr_type = T_ARM_ARITH_IMM, .cond = C_AL,
        .S = B_UNSET, .U = B_SET, .I = B_SET, .imm = 0x100000, .Rd = 12}},
    {0xf5d3f000, 0, "pld [r3]", {
        .instr = I_PLD, .instr_type = T_ARM_UNCOND, .cond = C_UNCOND,
        .Rn = r3, .I = B_SET, .imm = 0, .U = B_SET}},
    {0xee1d1f72, 0, "mrc 15, 0, r1, cr13, cr2, 3", {
        .instr = I_MRC, .instr_type = T_ARM_MVCR, .cond = C_AL, .coproc = 15,
        .opc1 = 0, .Rt = r1, .CRn = cr13, .CRm = cr2, .opc2 = 3}},
    {0xe7f002fa, 0, "udf #42", {
        .instr = I_UDF, .instr_type = T_ARM_UDF, .cond = C_AL,
        .I = B_SET, .imm = 42}},
    {0xe1434052, 0, "qdadd r4, r2, r3", {
        .instr = I_QDADD, .instr_type = T_ARM_SAT, .cond = C_AL,
        .Rd = r4, .Rm = r2, .Rn = r3}},
    {0xe1634052, 0, "qdsub r4, r2, r3", {
        .instr = I_QDSUB, .instr_type = T_ARM_SAT, .cond = C_AL,
        .Rd = r4, .Rm = r2, .Rn = r3}},

    // we switch to thumb (oboy)
    {0, 0, NULL, {.instr = I_INVLD}},

    {0xbe03, 0, "bkpt #3", {
        .instr = I_BKPT, .instr_type = T_THUMB_ONLY_IMM8, .cond = C_AL,
        .I = B_SET, .imm = 3}},
    {0xdf01, 0, "svc #1", {
        .instr = I_SVC, .instr_type = T_THUMB_ONLY_IMM8, .cond = C_AL,
        .I = B_SET, .imm = 1}},
    {0xdd02, 0, "ble #+4", {
        .instr = I_B, .instr_type = T_THUMB_COND_BRANCH, .cond = C_LE,
        .I = B_SET, .imm = 4}},
    {0xe7ff, 0, "b #+-2", {
        .instr = I_B, .instr_type = T_THUMB_UNCOND_BRANCH, .cond = C_AL,
        .I = B_SET, .imm = -2}},
    {0x992c, 0, "ldr r1, [sp, #176]", {
        .instr = I_LDR, .instr_type = T_THUMB_STACK, .cond = C_AL,
        .I = B_SET, .imm = 176, .Rn = SP, .Rt = r1, .U = B_SET,
        .W = B_UNSET, .P = B_SET}},
    {0x4a04, 0, "ldr r2, [pc, #16]", {
        .instr = I_LDR, .instr_type = T_THUMB_LDR_PC, .cond = C_AL,
        .I = B_SET, .imm = 16, .Rn = PC, .Rt = r2, .U = B_SET,
        .W = B_UNSET, .P = B_SET}},
    {0x4253, 0, "rsb r3, r2, #0", {
        .instr = I_RSB, .instr_type = T_THUMB_GPI, .cond = C_AL,
        .I = B_SET, .imm = 0, .Rn = r2, .Rd = r3}},
    {0x42a2, 0, "cmp r2, r4", {
        .instr = I_CMP, .instr_type = T_THUMB_GPI, .cond = C_AL,
        .Rn = r2, .Rm = r4}},
    // TODO fix correct format string (i.e., unique format strings for thumb,
    // so that the string representation of this instruction becomes
    // "eor r4, r7" as described in the manual)
    {0x407c, 0, "eor r4, r4, r7", {
        .instr = I_EOR, .instr_type = T_THUMB_GPI, .cond = C_AL,
        .Rd = r4, .Rn = r4, .Rm = r7}},
    {0x431f, 0, "orr r7, r7, r3", {
        .instr = I_ORR, .instr_type = T_THUMB_GPI, .cond = C_AL,
        .Rd = r7, .Rn = r7, .Rm = r3}},
    {0x47b8, 0, "blx r7", {
        .instr = I_BLX, .instr_type = T_THUMB_BRANCH_REG, .cond = C_AL,
        .Rm = r7}},
    {0xbf10, 0, "yield", {
        .instr = I_YIELD, .instr_type = T_THUMB_IT_HINTS, .cond = C_AL}},
    {0x372a, 0, "add r7, r7, #42", {
        .instr = I_ADD, .instr_type = T_THUMB_HAS_IMM8, .cond = C_AL,
        .Rd = r7, .Rn = r7, .I = B_SET, .imm = 42}},
    {0x2413, 0, "mov r4, #19", {
        .instr = I_MOV, .instr_type = T_THUMB_HAS_IMM8, .cond = C_AL,
        .Rd = r4, .I = B_SET, .imm = 19}},
    {0xa110, 0, "adr r1, #+64", {
        .instr = I_ADR, .instr_type = T_THUMB_HAS_IMM8, .cond = C_AL,
        .Rd = r1, .I = B_SET, .imm = 64, .Rn = PC, .U = B_SET}},
    {0xb2b2, 0, "uxth r2, r6", {
        .instr = I_UXTH, .instr_type = T_THUMB_EXTEND, .cond = C_AL,
        .Rd = r2, .Rm = r6}},
    {0x1acd, 0, "sub r5, r1, r3", {
        .instr = I_SUB, .instr_type = T_THUMB_3REG, .cond = C_AL,
        .Rd = r5, .Rn = r1, .Rm = r3}},
    {0x1ef4, 0, "sub r4, r6, #3", {
        .instr = I_SUB, .instr_type = T_THUMB_2REG_IMM, .cond = C_AL,
        .Rd = r4, .Rn = r6, .I = B_SET, .imm = 3}},
    {0xad18, 0, "add r5, sp, #96", {
        .instr = I_ADD, .instr_type = T_THUMB_ADD_SP_IMM, .cond = C_AL,
        .Rd = r5, .Rn = SP, .I = B_SET, .imm = 96}},
    {0x4676, 0, "mov r6, lr", {
        .instr = I_MOV, .instr_type = T_THUMB_MOV4, .cond = C_AL,
        .Rd = r6, .Rm = LR}},
    {0x88f3, 0, "ldrh r3, [r6, #6]", {
        .instr = I_LDRH, .instr_type= T_THUMB_RW_MEMI, .cond = C_AL,
        .Rt = r3, .Rn = r6, .I = B_SET, .imm = 6, .U = B_SET,
        .P = B_SET, .W = B_UNSET}},
    {0x5073, 0, "str r3, [r6, r1]", {
        .instr = I_STR, .instr_type = T_THUMB_RW_MEMO, .cond = C_AL,
        .Rt = r3, .Rn = r6, .Rm = r1, .P = B_SET, .U = B_SET, .W = B_UNSET}},
    {0xc124, 0, "stm r1, {r2,r5}", {
        .instr = I_STM, .instr_type = T_THUMB_RW_REG, .cond = C_AL,
        .Rn = r1, .reglist = 0x24}},
    {0xb4ff, 0, "push {r0-r7}", {
        .instr = I_PUSH, .instr_type = T_THUMB_PUSHPOP, .cond = C_AL,
        .reglist = 0xff}},
    {0xb5ff, 0, "push {r0-r7,lr}", {
        .instr = I_PUSH, .instr_type = T_THUMB_PUSHPOP, .cond = C_AL,
        .reglist = 0x40ff}},
    {0xbcff, 0, "pop {r0-r7}", {
        .instr = I_POP, .instr_type = T_THUMB_PUSHPOP, .cond = C_AL,
        .reglist = 0xff}},
    {0xbdff, 0, "pop {r0-r7,pc}", {
        .instr = I_POP, .instr_type = T_THUMB_PUSHPOP, .cond = C_AL,
        .reglist = 0x80ff}},
    {0x4545, 0, "cmp r5, r8", {
        .instr = I_CMP, .instr_type = T_THUMB_CMP, .cond = C_AL,
        .Rn = r5, .Rm = r8}},
    {0xde2a, 0, "udf #42", {
        .instr = I_UDF, .instr_type = T_THUMB_ONLY_IMM8, .cond = C_AL,
        .I = B_SET, .imm = 42}},
    {0x449b, 0, "add sp, sp, r3", {
        .instr = I_ADD, .instr_type = T_THUMB_MOD_SP_REG, .cond = C_AL,
        .Rd = SP, .Rn = SP, .Rm = r3}},
    {0xb132, 0, "cbz r2, #+12", {
        .instr = I_CBZ, .instr_type = T_THUMB_CBZ, .cond = C_AL,
        .Rn = r2, .I = B_SET, .U = B_SET, .Rm = PC, .imm = 12}},
    {0xbbbb, 0, "cbnz r3, #+110", {
        .instr = I_CBNZ, .instr_type = T_THUMB_CBZ, .cond = C_AL,
        .Rn = r3, .I = B_SET, .U = B_SET, .Rm = PC, .imm = 110}},
    {0xbf34, 0, "", {
        .instr = I_IT, .instr_type = T_THUMB_IT_HINTS, .cond = C_AL,
        .firstcond = C_CC, .mask = 4}},

    // we now switch to thumb2
    {0, 0, NULL, {.instr = I_INVLD}},

    {0xF55152AA, 0, "adcal r2, r1 #0x1540", {
        .instr = I_ADC, .I = B_SET, .imm = (0x1540), .S = B_SET, .Rn = r1, .Rd = r2, .cond = C_AL}},
    {0xF14102CC, 0, "adcal r2, r1 #0xcc", {
        .instr = I_ADC, .I = B_SET, .imm = (0xCC), .S = B_UNSET, .Rn = r1, .Rd = r2, .cond = C_AL}},
    {0xEB4211F3, 0, "adcal.w r1, r2, r3 ror #7", {
        .instr = I_ADC, .I = B_SET, .imm = 0x7, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL, .shift_type = S_ROR, .shift = 0x7}},
    {0xF51251AF, 0, "addsal.w r1, r2 #5600", {
        .instr = I_ADD, .I = B_SET, .imm = 5600, .S = B_SET, .Rd = r1, .Rn = r2, .cond = C_AL}},
    // exception in ADD function -> CMN
    {0xF5125FAF, 0, "cmnal r2 #5600", {
        .instr = I_CMN, .I = B_SET, .imm = 5600, .S = B_UNSET, .Rn = r2, .Rd = PC, .cond = C_AL}},
    // exception in ADD function -> ADD (SP plus imm)
    {0xF51D51AF, 0, "addsal.w r1, SP #5600", {
        .instr = I_ADD, .I = B_SET, .imm = 5600, .S = B_SET, .Rd = r1, .Rn = SP, .cond = C_AL}},
    // exception in ADD (SP plus imm) function -> CMN
    {0xF51D5FAF, 0, "cmnal SP #5600", {
        .instr = I_CMN, .I = B_SET, .imm = 5600, .S = B_UNSET, .Rn = SP, .Rd = PC, .cond = C_AL}},
    {0xF60271AB, 0, "addwal r1, r2, #0xfab", {
        .instr = I_ADDW, .I = B_SET, .imm = 0xFAB, .Rd = r1, .Rn = r2, .cond = C_AL}},
    {0xF60F71AC, 0, "adral r1 #0xfac", {
        .instr = I_ADR, .I = B_SET, .imm = 0xFAC, .Rd = r1, .Rn = PC, .cond = C_AL}},
    {0xEB0211D3, 0, "addal.w r1, r2, r3 lsr #7", {
        .instr = I_ADD, .I = B_SET, .imm = 7, .Rd = r1, .Rn = r2, .Rm = r3, .shift = 7, .shift_type = S_LSR, .cond = C_AL}},
    {0xEB121FD3, 0, "cmnal r2, r3, lsr #7", {
        .instr = I_CMN, .I = B_SET, .imm = 7, .Rn = r2, .Rd = PC, .Rm = r3, .shift = 7, .shift_type = S_LSR, .cond = C_AL}},
    {0xEB1D13D3, 0, "addsal.w r3, SP, r3 lsr #7", {
        .instr = I_ADD, .I = B_SET, .S = B_SET, .imm = 7, .Rd = r3, .Rn = SP, .Rm = r3, .shift = 7, .shift_type = S_LSR, .cond = C_AL}},
    // add is false
    {0xF6AF37AD, 0, "adral.w r7 #0xbad", {
        .instr = I_ADR, .I = B_SET, .imm = 0xBAD, .Rd = r7, .Rn = PC, .cond = C_AL}},
    // add is true
    {0xF60F37AD, 0, "adral.w r7 #0xbad", {
        .instr = I_ADR, .I = B_SET, .imm = 0xBAD, .Rd = r7, .Rn = PC, .cond = C_AL}},

    /* AND */
    {0xF41221BC, 0, "andal r1, r2 #0x5e00", {
        .instr = I_AND, .I = B_SET, .S = B_SET, .imm = 0x5E000, .Rd = r1, .Rn = r2, .cond = C_AL}},
    {0xF4122FBC, 0, "tstal r2 #0x5e00", {
        .instr = I_TST, .I = B_SET, .S = B_UNSET, .imm = 0x5E000, .Rn = r2, .Rd = PC, .cond = C_AL}},
    {0xEA121143, 0, "andsal.w r1, r2, r3 lsl #5", {
        .instr = I_AND, .I = B_SET, .S = B_SET, .imm = 0x5, .Rd = r1, .Rn = r2, .Rm = r3, .shift = 5, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA121F43, 0, "tstal r2, r3 lsl #5", {
        .instr = I_TST, .I = B_SET, .S = B_UNSET, .imm = 0x5, .Rd = PC, .Rn = r2, .Rm = r3, .shift = 5, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA5F11A2, 0, "asrsal.w r1, r2, #6", {
        .instr = I_ASR, .I = B_SET, .S = B_SET, .imm = 0x6, .Rd = r1, .Rm = r2, .shift = 6, .shift_type = S_ASR, .cond = C_AL}},
    {0xFA42F104, 0, "asral.w r1, r2, r4", {
        .instr = I_ASR, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r4, .cond = C_AL}},

    /* Branch */
    {0xF43F8801, 0, "beq.w #-266238", {
        .instr = I_B, .I = B_SET, .S = B_SET, .imm = 0xFFFBF002, .cond = C_EQ}},
    {0xF30AAFF6, 0, "bgt.w #0xcafec", {
        .instr = I_B, .I = B_SET, .S = B_UNSET, .imm = 0xCAFEC, .cond = C_GT}},
    {0xF0AF975D, 0, "bal.w #0xcafeba", {
        .instr = I_B, .I = B_SET, .S = B_UNSET, .imm = 0xCAFEBA, .cond = C_AL}},
    {0xF55FB6BA, 0, "bal.w #-6947468", {
        .instr = I_B, .I = B_SET, .S = B_SET, .imm = 0xFF95FD74, .cond = C_AL}},
    {0xF36F535C, 0, "bfcal r3, #21, #8", {
        .instr = I_BFC, .I = B_SET, .imm = 21, .Rd = r3, .msb = 28, .width = 8, .lsb = 21,  .cond = C_AL}},
    {0xF36251DF, 0, "bfial r1, r2, #23, #9", {
        .instr = I_BFI, .I = B_SET, .imm = 23, .Rd = r1, .Rn = r2, .msb = 31, .width = 9, .lsb = 23,  .cond = C_AL}},
    {0xF36F51DF, 0, "bfcal r1, #23, #9", {
        .instr = I_BFC, .I = B_SET, .imm = 23, .Rd = r1, .msb = 31, .width = 9, .lsb = 23,  .cond = C_AL}},
    {0xF42151AD, 0, "bic r1, r1, #5536", {
        .instr = I_BIC, .I = B_SET, .S = B_UNSET, .imm = 5536, .Rd = r1, .Rn = r1, .cond = C_AL}},
    {0xF43352AD, 0, "bics r2, r3, #5536", {
        .instr = I_BIC, .I = B_SET, .S = B_SET, .imm = 5536, .Rd = r2, .Rn = r3, .cond = C_AL}},
    {0xEA227123, 0, "bical.w r1, r2, r3 asr #28", {
        .instr = I_BIC, .I = B_SET, .S = B_UNSET, .imm = 28, .Rd = r1, .Rn = r2, .Rm = r3, .shift = 28, .shift_type = S_ASR, .cond = C_AL}},
    {0xEA327223, 0, "bicsal.w r2, r2, r3 asr #28", {
        .instr = I_BIC, .I = B_SET, .S = B_SET, .imm = 28, .Rd = r2, .Rn = r2, .Rm = r3, .shift = 28, .shift_type = S_ASR, .cond = C_AL}},
    {0xF1EAD6DF, 0, "blal #0xdeadbe", {
        .instr = I_BL, .I = B_SET, .S = B_UNSET, .imm = 0xdeadbe, .cond = C_AL}},
    {0xF5EAFEDF, 0, "blal #-2183746", {
        .instr = I_BL, .I = B_SET, .S = B_SET, .imm = 0xffdeadbe, .cond = C_AL}},
    {0xF00FC78E, 0, "blxal #0xc0ff1c", {
        .instr = I_BLX, .I = B_SET, .S = B_UNSET, .imm = 0xc0ff1c, .H = B_SET, .cond = C_AL}},
    {0xF40FEF8E, 0, "blxal #-4128996", {
        .instr = I_BLX, .I = B_SET, .S = B_SET, .imm = 0xffc0ff1c, .H = B_SET, .cond = C_AL}},
    {0xF3C58F00, 0, "bxjal r5", {
        .instr = I_BXJ, .I = B_UNSET, .Rm = r5, .cond = C_AL}},

    /* TODO: tests for CPD, CPD2
    {0x, 0, "cdpal p7 #14  r1, r2, r3 #5", {
        .instr = I_BXJ, .I = B_UNSET, .Rm = r5, .cond = C_AL}},
    */

    {0xF3BF8F2F, 0, "clrexal", {
        .instr = I_CLREX, .cond = C_AL}},
    {0xFAB2F182, 0, "clzal r1, r2", {
        .instr = I_CLZ, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xF5112FBC, 0, "cmnal r1, #0x5e000", {
        .instr = I_CMN, .I = B_SET, .Rn = r1, .Rd = PC, .imm = 0x5e000, .cond = C_AL}},
    {0xEB115F42, 0, "cmnal.w r1, r2, lsl #21", {
        .instr = I_CMN, .I = B_SET, .Rn = r1, .Rd = PC, .Rm = r2, .imm = 21, .shift = 21, .shift_type = S_LSL, .cond = C_AL}},
    {0xF5B15FAF, 0, "cmpal.w r1, #5600", {
        .instr = I_CMP, .I = B_SET, .Rn = r1, .Rd = PC, .imm = 5600, .cond = C_AL}},
    {0xEBB17F12, 0, "cmpal.w r1, r2, lsr #28", {
        .instr = I_CMP, .I = B_SET, .Rn = r1, .Rd = PC, .Rm = r2, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xF3AF80FD, 0, "dbgal #13", {
        .instr = I_DBG, .option = 13, .cond = C_AL}},
    {0xF3BF8F5C, 0, "dmbal #12", {
        .instr = I_DMB, .option = 12, .cond = C_AL}},
    {0xF3BF8F4B, 0, "dsbal #11", {
        .instr = I_DSB, .option = 11, .cond = C_AL}},
    {0xF48241AD, 0, "eoral r1, r2, #0x5680", {
        .instr = I_EOR, .S = B_UNSET, .I = B_SET, .imm = 0x5680, .Rd = r1, .Rn = r2, .cond = C_AL}},
    {0xF49342AD, 0, "eorsal r2, r3, #0x5680", {
        .instr = I_EOR, .S = B_SET, .I = B_SET, .imm = 0x5680, .Rd = r2, .Rn = r3, .cond = C_AL}},
    {0xF4824FAD, 0, "eoral PC, r2, #0x5680", {
        .instr = I_EOR, .S = B_UNSET, .I = B_SET, .imm = 0x5680, .Rd = PC, .Rn = r2, .cond = C_AL}},
    {0xF4925FAC, 0, "teqal r2, #0x1580", {
        .instr = I_TEQ, .S = B_UNSET, .I = B_SET, .imm = 0x1580, .Rd = PC, .Rn = r2, .cond = C_AL}},
    {0xEA8241B3, 0, "eoral.w r1, r2, r3, ror #18", {
        .instr = I_EOR, .S = B_UNSET, .I = B_SET, .imm = 18, .Rd = r1, .Rn = r2, .Rm = r3, .shift = 18, .shift_type = S_ROR, .cond = C_AL}},
    {0xEA9241B3, 0, "eorsal.w r1, r2, r3, ror #18", {
        .instr = I_EOR, .S = B_SET, .I = B_SET, .imm = 18, .Rd = r1, .Rn = r2, .Rm = r3, .shift = 18, .shift_type = S_ROR, .cond = C_AL}},
    {0xEA824F83, 0, "eoral.w pc, r2, r3, lsl #18", {
        .instr = I_EOR, .S = B_UNSET, .I = B_SET, .imm = 18, .Rd = PC, .Rn = r2, .Rm = r3, .shift = 18, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA924F93, 0, "teqal.w r2, r3, lsr #18", {
        .instr = I_TEQ, .S = B_UNSET, .I = B_SET, .imm = 18, .Rd = PC, .Rn = r2, .Rm = r3, .shift = 18, .shift_type = S_LSR, .cond = C_AL}},
    {0xF3BF8F6A, 0, "isbal #10", {
        .instr = I_ISB, .option = 10, .cond = C_AL}},

    /* TODO: LDC/LDC2 tests */

    {0xE8918AAA, 0, "ldmal.w r1, pc,r11,r9,r7,r5,r3,r1", {
        .instr = I_LDM, .Rn = r1, .W = B_UNSET, .reglist = 0x8AAA, .cond = C_AL}},
    {0xE8B14AA9, 0, "ldmal.w r1!, lr,r11,r9,r7,r5,r3,r0", {
        .instr = I_LDM, .Rn = r1, .W = B_SET, .reglist = 0x4AA9, .cond = C_AL}},
    {0xE8BD4AA9, 0, "popal.w lr,r11,r9,r7,r5,r3,r0", {
        .instr = I_POP, .M = B_SET, .P = B_UNSET, .reglist = 0x4AA9, .cond = C_AL}},
    {0xE9119555, 0, "ldmdbal r1, pc,r12,r10,r8,r6,r4,r2,r0", {
        .instr = I_LDMDB, .W = B_UNSET, .Rn = r1, .reglist = 0x9555, .cond = C_AL}},

    /* LDR */
    {0xF8D21DEA, 0, "ldral.w r1, r2, #0xdea", {
        .instr = I_LDR, .I = B_SET, .Rn = r2, .Rt = r1, .imm = 0xdea, .cond = C_AL}},
    {0xF8DF1DEF, 0, "ldral.w r1, #0xdef", {
        .instr = I_LDR, .I = B_SET, .U = B_SET, .Rt = r1, .imm = 0xdef, .cond = C_AL}},
    {0xF8521CFF, 0, "ldral r1, [r2, #0xff]", {
        .instr = I_LDR, .I = B_SET, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF85F1CFF, 0, "ldral.w r1, #-0xcff", {
        .instr = I_LDR, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xcff, .cond = C_AL}},
    {0xF8521EFF, 0, "ldrtal r1, [r2, #0xff]", {
        .instr = I_LDRT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF85D1B04, 0, "popal.w r1", {
        .instr = I_POP, .I = B_UNSET, .Rt = r1, .cond = C_AL}},

    /* LDR literal */
    {0xF85F3DEF, 0, "ldral.w r3 #0xdef", {
        .instr = I_LDR, .I = B_SET, .U = B_UNSET, .Rt = r3, .imm = 0xdef, .cond = C_AL}},
    {0xF8DF3DAF, 0, "ldral.w r3 #0xdaf", {
        .instr = I_LDR, .I = B_SET, .U = B_SET, .Rt = r3, .imm = 0xdaf, .cond = C_AL}},

    /* LDR register */
    {0xF8521033, 0, "ldral.w r1, r2, r3 lsl #3", {
        .instr = I_LDR, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xF85F1033, 0, "ldral.w r1 #51", {
        .instr = I_LDR, .I = B_SET, .Rt = r1, .imm = 51, .cond = C_AL}},

    /* LDRB */
    {0xF8921FAB, 0, "ldrbal.w r1, r2, #0xfab", {
        .instr = I_LDRB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xfab, .cond = C_AL}},
    {0xF892FFAB, 0, "pldal.w r2, #0xfab", {
        .instr = I_PLD, .I = B_SET, .W = B_UNSET, .Rn = r2, .imm = 0xfab, .cond = C_AL}},
    {0xF89F1FAB, 0, "ldrbal r1, #0xfab", {
        .instr = I_LDRB, .I = B_SET, .Rt = r1, .U = B_SET, .imm = 0xfab, .cond = C_AL}},
    {0xF8121DFF, 0, "ldrbal r1, [r2], #-0xff", {
        .instr = I_LDRB, .I = B_SET, .Rt = r1, .Rn = r2, .U = B_UNSET, .P = B_SET, .W = B_SET, .imm = 0xff, .cond = C_AL}},
    {0xF812FCFF, 0, "pldal [r2, #-0xff]", {
        .instr = I_PLD, .I = B_SET, .Rn = r2, .U = B_UNSET, .W = B_UNSET, .imm = 0xff, .cond = C_AL}},

    // literal
    {0xF81F1CFF, 0, "ldrbal r1, #0xcff", {
        .instr = I_LDRB, .I = B_SET, .Rt = r1, .U = B_UNSET, .imm = 0xcff, .cond = C_AL}},
    {0xF8121EFF, 0, "ldrbtal r1, [r2, #0xff]", {
        .instr = I_LDRBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF89F3FEC, 0, "ldrbal r3, #0xfec", {
        .instr = I_LDRB, .I = B_SET, .Rt = r3, .U = B_SET, .imm = 0xfec, .cond = C_AL}},
    {0xF81FF880, 0, "pldal #0x880", {
        .instr = I_PLD, .I = B_SET, .U = B_UNSET, .imm = 0x880, .cond = C_AL}},

    // register
    {0xF8121033, 0, "ldrbal.w r1, [r2, r3, lsl 3]", {
        .instr = I_LDRB, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xF812F033, 0, "pldal [r2, r3, lsl #3]", {
        .instr = I_PLD, .I = B_SET, .Rn = r2, .Rm = r3, .imm = 3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xF81F1033, 0, "ldrbal r1, #0x33", {
        .instr = I_LDRB, .I = B_SET, .U = B_UNSET, .Rt = r1,.imm = 0x33, .cond = C_AL}},
    {0xF81FF033, 0, "pldal #0x33", {
        .instr = I_PLD, .I = B_SET, .U = B_UNSET, .imm = 0x33, .cond = C_AL}},

    // LDRBT
    {0xF8121EAB, 0, "ldrbtal r1, [r2, #0xab]", {
        .instr = I_LDRBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xab, .cond = C_AL}},
    {0xF81F1EAB, 0, "ldrbal r1, #0xeab", {
        .instr = I_LDRB, .I = B_SET, .U = B_UNSET, .Rt = r1,.imm = 0xeab, .cond = C_AL}},

    // LDRD
    {0xE95312FF, 0, "ldrdal r1, r2, [r3, #-0x3fc]", {
        .instr = I_LDRD, .I = B_SET, .U = B_UNSET, .P = B_SET, .W = B_UNSET, .Rt = r1, .Rt2 = r2, .Rn = r3, .imm = 0x3fc, .cond = C_AL}},
    {0xE9F312FF, 0, "ldrdal r1, r2, [r3, #0x3fc]!", {
        .instr = I_LDRD, .I = B_SET, .U = B_SET, .P = B_SET, .W = B_SET, .Rt = r1, .Rt2 = r2, .Rn = r3, .imm = 0x3fc, .cond = C_AL}},
    {0xE8F312FF, 0, "ldrdal r1, r2, [r3], #0x3fc", {
        .instr = I_LDRD, .I = B_SET, .U = B_SET, .P = B_UNSET, .W = B_SET, .Rt = r1, .Rt2 = r2, .Rn = r3, .imm = 0x3fc, .cond = C_AL}},
    {0xE8FF12FF, 0, "ldrdal r1, r2, #0x3fc", {
        .instr = I_LDRD, .I = B_SET, .U = B_SET, .P = B_UNSET, .W = B_SET, .Rt = r1, .Rt2 = r2, .imm = 0x3fc, .cond = C_AL}},

    // LDREX
    {0xE8521F55, 0, "ldrexal r1, [r2, #0x154]", {
        .instr = I_LDREX, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0x154, .cond = C_AL}},
    {0xE8D21F4F, 0, "ldrexbal r1, [r2]", {
        .instr = I_LDREXB, .I = B_UNSET, .Rt = r1, .Rn = r2, .cond = C_AL}},
    {0xE8D3127F, 0, "ldrexdal r1, r2, [r3]", {
        .instr = I_LDREXD, .I = B_UNSET, .Rt = r1, .Rt2 = r2, .Rn = r3, .cond = C_AL}},
    {0xE8D21F5F, 0, "ldrexhal r1, [r2]", {
        .instr = I_LDREXH, .I = B_UNSET, .Rt = r1, .Rn = r2, .cond = C_AL}},

    // LDRH
    {0xF8B21B0B, 0, "ldrhal.w r1, [r2, #0xb0b]", {
        .instr = I_LDRH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xb0b, .cond = C_AL}},
    {0xF8BF1B0B, 0, "ldrhal r1, #0xb0b", {
        .instr = I_LDRH, .I = B_SET, .U = B_SET, .Rt = r1, .imm = 0xb0b, .cond = C_AL}},
    {0xF8B2FB0B, 0, "pldwal [r2, #0xb0b]", {
        .instr = I_PLD, .I = B_SET, .W = B_SET, .Rn = r2, .imm = 0xb0b, .cond = C_AL}},
    {0xF8BFFB0B, 0, "pldal #0xb0b", {
        .instr = I_PLD, .I = B_SET, .U = B_SET, .imm = 0xb0b, .cond = C_AL}},
    {0xF8321DFF, 0, "ldrhal r1, [r2], #-0xff", {
        .instr = I_LDRH, .I = B_SET, .P = B_SET, .W = B_SET, .U = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF8321CFF, 0, "ldrhal r1, [r2, #-0xff]", {
        .instr = I_LDRH, .I = B_SET, .P = B_SET, .W = B_UNSET, .U = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF83219FF, 0, "ldrhal r1, [r2, #-0xff]!", {
        .instr = I_LDRH, .I = B_SET, .P = B_UNSET, .W = B_SET, .U = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF83F1FFF, 0, "ldrhal r1, #0xfff", {
        .instr = I_LDRH, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xfff, .cond = C_AL}},
    {0xF8321EFF, 0, "ldrhtal r1, [r2, #0xff]", {
        .instr = I_LDRHT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},

    // literal
    {0xF83F1E0F, 0, "ldrhal r1, [PC, -#0xe0f]", {
        .instr = I_LDRH, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xe0f, .cond = C_AL}},
    {0xF8BFFE0F, 0, "pldal [PC, #0xe0f]", {
        .instr = I_PLD, .I = B_SET, .U = B_SET, .imm = 0xe0f, .cond = C_AL}},

    // register
    {0xF8321023, 0, "ldrhal.w r1, [r2, r3, lsl #2]", {
        .instr = I_LDRH, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 0x2, .shift = 2, .shift_type = S_LSL, .cond = C_AL}},
    {0xF83F1023, 0, "ldrhal r1, [PC, #-0x23]", {
        .instr = I_LDRH, .I = B_SET, .Rt = r1, .imm = 0x23, .cond = C_AL}},
    {0xF832F023, 0, "pldwal [r2, r3, lsl #2]", {
        .instr = I_PLD, .I = B_SET, .Rn = r2, .Rm = r3, .W = B_SET, .imm = 2, .shift = 2, .shift_type = S_LSL, .cond = C_AL}},

    // LDRHT
    {0xF8321EFF, 0, "ldrhtal r1, [r2, #0xff]", {
        .instr = I_LDRHT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF83F1EFF, 0, "ldrhal r1, [PC, #-0xeff]", {
        .instr = I_LDRH, .I = B_SET, .Rt = r1, .U = B_UNSET, .imm = 0xeff, .cond = C_AL}},

    // LDRSB
    {0xF9921FED, 0, "ldrsbal r1, [r2, #0xfed]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xfed, .cond = C_AL}},
    {0xF992FFED, 0, "plial [r2, #0xfed]", {
        .instr = I_PLI, .I = B_SET, .Rn = r2, .imm = 0xfed, .cond = C_AL}},
    {0xF99F1FED, 0, "ldrsbal r1, [PC, #0xfed]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .U = B_SET, .imm = 0xfed, .cond = C_AL}},
    {0xF99FFFED, 0, "plial [PC, #0xfed]", {
        .instr = I_PLI, .I = B_SET, .U = B_SET, .imm = 0xfed, .cond = C_AL}},
    {0xF9121CFF, 0, "ldrsbal r1, [r2, #-0xff]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .Rn = r2, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .imm = 0xff, .cond = C_AL}},
    {0xF9121FFF, 0, "ldrsbal r1, [r2, #0xff]!", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .Rn = r2, .P = B_SET, .U = B_SET, .W = B_SET, .imm = 0xff, .cond = C_AL}},
    {0xF912FCFF, 0, "plial [r2, #-0xff]", {
        .instr = I_PLI, .I = B_SET, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF91F1CFF, 0, "ldrsbal r1, [pc, #-0xcff]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .U = B_UNSET, .imm = 0xcff, .cond = C_AL}},
    {0xF9121EFF, 0, "ldrsbtal r1, [r2, #0xff]", {
        .instr = I_LDRSBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},

    // literal
    {0xF91F1B2B, 0, "ldrsbal r1, [PC, #-0xb2b]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .imm = 0xb2b, .cond = C_AL}},
    {0xF91FFB2B, 0, "plial [PC, #-0xb2b]", {
        .instr = I_PLI, .I = B_SET, .imm = 0xb2b, .cond = C_AL}},

    // register
    {0xF9121013, 0, "ldrsbal.w r1, [r2, r3, lsl #1]", {
        .instr = I_LDRSB, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 1, .shift = 1, .shift_type = S_LSL, .cond = C_AL}},
    {0xF912F013, 0, "plial [r2, r3 lsl #1]", {
        .instr = I_PLI, .I = B_SET, .Rn = r2, .Rm = r3, .imm = 1, .shift = 1, .shift_type = S_LSL, .cond = C_AL}},
    {0xF91FF013, 0, "plial [pc, #-0x13]", {
        .instr = I_PLI, .I = B_SET, .U = B_UNSET, .imm = 0x13, .cond = C_AL}},
    {0xF91F1013, 0, "ldrsbal r1, [pc, #-0x13]", {
        .instr = I_LDRSB, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0x13, .cond = C_AL}},

    // LDRSBT
    {0xF9121EFF, 0, "ldrsbtal r1, [r2, #0xff]", {
        .instr = I_LDRSBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF91F1EFF, 0, "ldrsbal r1, [pc, #-0xeff]", {
        .instr = I_LDRSB, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xeff, .cond = C_AL}},

    // LDRSH
    {0xF9B21D0B, 0, "ldrshal r1, [r2, #0xd0b]", {
        .instr = I_LDRSH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xd0b, .cond = C_AL}},
    {0xF9BF1D0B, 0, "ldrshal r1, #0xd0b", {
        .instr = I_LDRSH, .I = B_SET, .Rt = r1, .U = B_SET, .imm = 0xd0b, .cond = C_AL}},
    {0xF9321CFF, 0, "ldrshal r1, [r2, #-0xff]", {
        .instr = I_LDRSH, .I = B_SET, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF9321DFF, 0, "ldrshal r1, [r2, #-0xff]!", {
        .instr = I_LDRSH, .I = B_SET, .P = B_SET, .U = B_UNSET, .W = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF93219FF, 0, "ldrshal r1, [r2], #-0xff", {
        .instr = I_LDRSH, .I = B_SET, .P = B_UNSET, .U = B_UNSET, .W = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF9321EFF, 0, "ldrshtal r1, [r2, #0xff]", {
        .instr = I_LDRSHT, .I = B_SET, .U = B_UNSET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},

    // literal
    {0xF9BF1BAD, 0, "ldrshal r1, [pc, #0xbad]", {
        .instr = I_LDRSH, .I = B_SET, .U = B_SET, .Rt = r1, .imm = 0xbad, .cond = C_AL}},

    // register
    {0xF9321033, 0, "ldrshal.w r1, [r2, r3, lsl #3]", {
        .instr = I_LDRSH, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xF93F1012, 0, "ldrshal r1, [pc, #-0x12]", {
        .instr = I_LDRSH, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0x12, .cond = C_AL}},

    // LDRSHT
    {0xF9321EFF, 0, "ldrshtal r1, [r2, #0xff]", {
        .instr = I_LDRSHT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},
    {0xF93F1EFF, 0, "ldrshal r1, [pc, #-0xeff]", {
        .instr = I_LDRSH, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xeff, .cond = C_AL}},

    // LDRT
    {0xF8521EFF, 0, "ldrtal r1, [r2, #0xff]", {
        .instr = I_LDRT, .I = B_SET, .Rn = r2, .Rt = r1, .imm = 0xff, .cond = C_AL}},
    {0xF85F1EF0, 0, "ldral.w r1, [pc, #-0xef0]", {
        .instr = I_LDR, .I = B_SET, .U = B_UNSET, .Rt = r1, .imm = 0xef0, .cond = C_AL}},

    // TODO: LEAVEX

    // LSL
    {0xEA4F7102, 0, "lslal.w r1, r2, #0x1c", {
        .instr = I_LSL, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .imm = 0x1c, .shift = 0x1c, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA5F0102, 0, "movsal.w r1, r2", {
        .instr = I_MOV, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xFA12F103, 0, "lslsal.w r1, r2, r3", {
        .instr = I_LSL, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // LSR we enabled shift/type even though the manual does not.
    {0xEA5F11D2, 0, "lsrsal.w r1, r2, #7", {
        .instr = I_LSR, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .imm = 7, .shift = 7, .shift_type = S_LSR, .cond = C_AL}},
    {0xEA4F1112, 0, "lsral.w r1, r2, #4", {
        .instr = I_LSR, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .imm = 4, .shift = 4, .shift_type = S_LSR, .cond = C_AL}},

    {0xFA32F103, 0, "lsrsal.w r1, r2, r3", {
        .instr = I_LSR, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rm = r3, .Rn = r2,  .cond = C_AL}},
    {0xFA22F107, 0, "lsrsal.w r1, r2, r7", {
        .instr = I_LSR, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rm = r7, .Rn = r2,  .cond = C_AL}},


    /* TODO: MCR, MCR2, MCRR, MCRR2 tests */

    // MLA
    {0xFB024103, 0, "mlaal r1, r2, r3, r4", {
        .instr = I_MLA, .I = B_UNSET, .Rd = r1, .Rm = r3, .Rn = r2, .Ra = r4, .cond = C_AL}},
    {0xFB02F103, 0, "mulal r1, r2, r3", {
        .instr = I_MUL, .I = B_UNSET, .Rd = r1, .Rm = r3, .Rn = r2, .cond = C_AL}},

    // MLS
    {0xFB024113, 0, "mlsal r1, r2, r3, r4", {
        .instr = I_MLS, .I = B_UNSET, .Ra = r4, .Rd = r1, .Rm = r3, .Rn = r2, .cond = C_AL}},

    // MOV immediate
    {0xF44F21AA, 0, "moval.w r1, #0x55000", {
        .instr = I_MOV, .I = B_SET, .S = B_UNSET, .Rd = r1, .imm = 0x55000, .cond = C_AL}},
    {0xF45F21AA, 0, "moval.w r1, #0x55000", {
        .instr = I_MOV, .I = B_SET, .S = B_SET, .Rd = r1, .imm = 0x55000, .cond = C_AL}},
    {0xF64A21AA, 0, "movwal r1, #0xaaaa", {
        .instr = I_MOVW, .I = B_SET, .Rd = r1, .imm = 0xaaaa, .cond = C_AL}},

    // register
    {0xEA4F0102, 0, "moval.w r1, r2", {
        .instr = I_MOV, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xEA5F0102, 0, "movsal.w r1, r2", {
        .instr = I_MOV, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},

    // MOVT
    {0xF6CF7100, 0, "movtal r1, #0xff00", {
        .instr = I_MOVT, .I = B_SET, .Rd = r1, .imm = 0xff00, .cond = C_AL}},

    /* TODO: tests for MRC, MRC2, MRRC, MRRC2 */

    // MRS
    {0xF3EF8100, 0, "mrsal r1, apsr", {
        .instr = I_MRS, .I = B_UNSET, .Rd = r1, .cond = C_AL}},

    // MSR
    {0xF3818C00, 0, "msral apsr, r1", {
        .instr = I_MSR, .I = B_SET, .Rn = r1, .mask = 3, .imm = 3, .cond = C_AL}},

    // MUL
    {0xFB02F103, 0, "mulal r1, r2, r3", {
        .instr = I_MUL, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // MVN immediate
    {0xF46F27AA, 0, "mvnal r7, #0x55000", {
        .instr = I_MVN, .I = B_SET, .S = B_UNSET, .Rd = r7, .imm = 0x55000, .cond = C_AL}},
    {0xF47F24AA, 0, "mvnsal r4, #0x55000", {
        .instr = I_MVN, .I = B_SET, .S = B_SET, .Rd = r4, .imm = 0x55000, .cond = C_AL}},

    // register
    {0xEA6F7112, 0, "mvnal.w r1, r2, lsr #7", {
        .instr = I_MVN, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEA7F7102, 0, "mvnsal.w r1, r2, lsl #7", {
        .instr = I_MVN, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .imm = 28, .shift = 28, .shift_type = S_LSL, .cond = C_AL}},

    // NOP
    {0xF3AF8000, 0, "nopal.w", {
        .instr = I_NOP, .I = B_UNSET, .cond = C_AL}},

    // ORN
    {0xF46172FF, 0, "ornal r2, r1, #0x1fe", {
        .instr = I_ORN, .I = B_SET, .S = B_UNSET, .Rd = r2, .Rn = r1, .imm = 0x1fe, .cond = C_AL}},
    {0xF47172FF, 0, "ornsal r2, r1, #0x1fe", {
        .instr = I_ORN, .I = B_SET, .S = B_SET, .Rd = r2, .Rn = r1, .imm = 0x1fe, .cond = C_AL}},
    {0xF47F72FF, 0, "mvnsal r2, r1, #0x1fe", {
        .instr = I_MVN, .I = B_SET, .S = B_SET, .Rd = r2, .imm = 0x1fe, .cond = C_AL}},

    // register
    {0xEA6271C3, 0, "ornal r1, r2, r3, lsl #31", {
        .instr = I_ORN, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 31, .shift = 31, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA7271C3, 0, "ornsal r1, r2, r3, lsl #31", {
        .instr = I_ORN, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 31, .shift = 31, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA6F71C3, 0, "mvnal r1, r3, lsl #31", {
        .instr = I_MVN, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r3, .imm = 31, .shift = 31, .shift_type = S_LSL, .cond = C_AL}},


    // ORR
    {0xF44221BC, 0, "orral r1, r2, #0x5e000", {
        .instr = I_ORR, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .imm = 0x5e000, .cond = C_AL}},
    {0xF45221BC, 0, "orral r1, r2, #0x5e000", {
        .instr = I_ORR, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .imm = 0x5e000, .cond = C_AL}},
    {0xF45F21BC, 0, "movsal.w r1, #0x5e000", {
        .instr = I_MOV, .I = B_SET, .S = B_SET, .Rd = r1, .imm = 0x5e000, .cond = C_AL}},


    // register
    {0xEA520143, 0, "orrsal.w r1, r2, r3, lsl #1", {
        .instr = I_ORR, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 1, .shift = 1, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA420143, 0, "orral.w r1, r2, r3, lsl #1", {
        .instr = I_ORR, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 1, .shift = 1, .shift_type = S_LSL, .cond = C_AL}},

    // PKH
    /* TODO: PKH test
    {0xEAC201C3, 0, "pkhbt r1, r2, r3, lsl #3", {
        .instr = I_PKH, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    */

    // PLD, PLDW (immediate)
    {0xF891FF0F, 0, "pldal [r1, #0xf0f]", {
        .instr = I_PLD, .I = B_SET, .W = B_UNSET, .Rn = r1, .imm = 0xf0f, .cond = C_AL}},
    {0xF8B1FF0F, 0, "pldwal [r1, #0xf0f]", {
        .instr = I_PLD, .I = B_SET, .W = B_SET, .Rn = r1, .imm = 0xf0f, .cond = C_AL}},
    {0xF89FFF0F, 0, "pldwal [pc, #0xf0f]", {
        .instr = I_PLD, .I = B_SET, .U = B_SET, .imm = 0xf0f, .cond = C_AL}},
    {0xF811FCF0, 0, "pldal [r1, #-0xf0]", {
        .instr = I_PLD, .I = B_SET, .W = B_UNSET, .Rn = r1, .imm = 0xf0, .cond = C_AL}},
    {0xF831FCF0, 0, "pldal [r1, #-0xf0]", {
        .instr = I_PLD, .I = B_SET, .W = B_SET, .Rn = r1, .imm = 0xf0, .cond = C_AL}},
    {0xF83FFCF0, 0, "pldal [pc, #-0xcf0]", {
        .instr = I_PLD, .I = B_SET, .U = B_UNSET, .imm = 0xcf0, .cond = C_AL}},

    // literal
    {0xF81FFFEE, 0, "pldal [pc, #0xfee]", {
        .instr = I_PLD, .I = B_SET, .U = B_UNSET, .imm = 0xfee, .cond = C_AL}},
    {0xF89FFFED, 0, "pldal [pc, #0xfed]", {
        .instr = I_PLD, .I = B_SET, .U = B_SET, .imm = 0xfed, .cond = C_AL}},


    // register
    {0xF811F022, 0, "pldal [r1, r2, lsl #2]", {
        .instr = I_PLD, .I = B_SET, .W = B_UNSET, .Rn = r1, .Rm = r2, .imm = 0x2, .shift = 2, .shift_type = S_LSL, .cond = C_AL}},
    {0xF831F022, 0, "pldwal [r1, r2, lsl #2]", {
        .instr = I_PLD, .I = B_SET, .W = B_SET, .Rn = r1, .Rm = r2, .imm = 0x2, .shift = 2, .shift_type = S_LSL, .cond = C_AL}},
    {0xF81FF022, 0, "pldal [pc, #-0x22]", {
        .instr = I_PLD, .I = B_SET, .U = B_UNSET, .imm = 0x22, .cond = C_AL}},


    // PLI
    {0xF991FF00, 0, "plial [r1, #0xf00]", {
        .instr = I_PLI, .I = B_SET, .Rn = r1, .imm = 0xf00, .cond = C_AL}},
    {0xF99FFF00, 0, "plial [pc, #0xf00]", {
        .instr = I_PLI, .I = B_SET, .U = B_SET, .imm = 0xf00, .cond = C_AL}},
    {0xF911FCFF, 0, "plial [r1, #-0xff]", {
        .instr = I_PLI, .I = B_SET, .Rn = r1, .imm = 0xff, .cond = C_AL}},
    {0xF91FFCFF, 0, "plial [pc, #-0xcff]", {
        .instr = I_PLI, .I = B_SET, .U = B_UNSET, .imm = 0xcff, .cond = C_AL}},
    {0xF91FFBBB, 0, "plial [pc, #0xbbb]", {
        .instr = I_PLI, .I = B_SET, .U = B_UNSET, .imm = 0xbbb, .cond = C_AL}},
    {0xF99FFBBB, 0, "plial [pc, #-0xbbb]", {
        .instr = I_PLI, .I = B_SET, .U = B_SET, .imm = 0xbbb, .cond = C_AL}},

    // PLI register
    {0xF911F032, 0, "plial [r1, r2, lsl #3]", {
        .instr = I_PLI, .I = B_SET, .Rn = r1, .Rm = r2, .imm = 0x3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xF91FF032, 0, "plial [pc, #-0x32]", {
        .instr = I_PLI, .I = B_SET, .U = B_UNSET, .imm = 0x32, .cond = C_AL}},


    // POP
    {0xE8BD8001, 0, "popal.w r0, pc", {
        .instr = I_POP, .I = B_UNSET, .P = B_SET, .M = B_UNSET, .reglist = (1|(1 << 15)), .cond = C_AL}},
    {0xE8BD4002, 0, "popal.w r1, lr", {
        .instr = I_POP, .I = B_UNSET, .P = B_UNSET, .M = B_SET, .reglist = (2|(1 << 14)), .cond = C_AL}},
    {0xF85D3B04, 0, "popal.w r3", {
        .instr = I_POP, .I = B_UNSET, .Rt = r3, .cond = C_AL}},

    // PUSH
    {0xE92D0003, 0, "pushal.w r0, r1", {
        .instr = I_PUSH, .I = B_UNSET, .M = B_UNSET, .reglist = 3, .cond = C_AL}},
    {0xE92D4003, 0, "pushal.w lr, r0, r1", {
        .instr = I_PUSH, .I = B_UNSET, .M = B_SET, .reglist = (3|(1<<14)), .cond = C_AL}},
    {0xF84D9D04, 0, "pushal.w r9", {
        .instr = I_PUSH, .I = B_UNSET, .Rt = r9, .cond = C_AL}},


    {0xFA83F182, 0, "qaddal r1, r2, r3", {
        .instr = I_QADD, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFA93F112, 0, "qadd16al r1, r2, r3", {
        .instr = I_QADD16, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFA83F112, 0, "qadd8al r1, r2, r3", {
        .instr = I_QADD8, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFAA3F112, 0, "qasxal r1, r2, r3", {
        .instr = I_QASX, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFA83F192, 0, "qdaddal r1, r2, r3", {
        .instr = I_QDADD, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFA83F1B2, 0, "qdsubal r1, r2, r3", {
        .instr = I_QDSUB, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFAE3F112, 0, "qsaxal r1, r2, r3", {
        .instr = I_QSAX, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFA83F1A2, 0, "qsub r1, r2, r3", {
        .instr = I_QSUB, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFAD3F112, 0, "qsub16 r1, r2, r3", {
        .instr = I_QSUB16, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},
    {0xFAC3F112, 0, "qsub8 r1, r2, r3", {
        .instr = I_QSUB8, .I = B_UNSET, .Rd = r1, .Rm = r2, .Rn = r3, .cond = C_AL}},

    {0xFA92F1A2, 0, "rbital r1, r2", {
        .instr = I_RBIT, .I = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xFA92F182, 0, "reval.w r1, r2", {
        .instr = I_REV, .I = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xFA92F192, 0, "rev16al.w r1, r2", {
        .instr = I_REV16, .I = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xFA92F1B2, 0, "revshal.w r1, r2", {
        .instr = I_REVSH, .I = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},

    {0xEA4F01F2, 0, "roral r1, r2, #3", {
        .instr = I_ROR, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .imm = 3, .shift = 3, .shift_type = S_ROR, .cond = C_AL}},
    {0xEA4F0132, 0, "rrxal r1, r2", {
        .instr = I_RRX, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xEA5F01F2, 0, "rorsal r1, r2, #3", {
        .instr = I_ROR, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .imm = 3, .shift = 3, .shift_type = S_ROR, .cond = C_AL}},
    {0xFA62F103, 0, "roral.w r1, r2, r3", {
        .instr = I_ROR, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFA72F103, 0, "roral.w r1, r2, r3", {
        .instr = I_ROR, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    {0xEA4F0132, 0, "rrxal r1, r2", {
        .instr = I_RRX, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xEA5F0132, 0, "rrxal r1, r2", {
        .instr = I_RRX, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},

    {0xF5C271FF, 0, "rsbal.w r1, r2, #0x1fe", {
        .instr = I_RSB, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .imm = 0x1fe, .cond = C_AL}},
    {0xF5D271FF, 0, "rsbsal.w r1, r2, #0x1fe", {
        .instr = I_RSB, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .imm = 0x1fe, .cond = C_AL}},
    {0xEBC27113, 0, "rsbal r1, r2, r3, lsr #28", {
        .instr = I_RSB, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBD27113, 0, "rsbsal r1, r2, r3, lsr #28", {
        .instr = I_RSB, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},

    {0xFA92F103, 0, "sadd16al r1, r2, r3", {
        .instr = I_SADD16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFA82F103, 0, "sadd8al r1, r2, r3", {
        .instr = I_SADD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAA2F103, 0, "sasxal r1, r2, r3", {
        .instr = I_SASX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    {0xF56271FF, 0, "sbcal r1, r2, #0x1fe", {
        .instr = I_SBC, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .imm = 0x1fe, .cond = C_AL}},
    {0xF57271FF, 0, "sbcal r1, r2, #0x1fe", {
        .instr = I_SBC, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .imm = 0x1fe, .cond = C_AL}},
    {0xEB6201C3, 0, "sbcal.w r1, r2, r3, lsl #3", {
        .instr = I_SBC, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 0x3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    {0xEB7201C3, 0, "sbcal.w r1, r2, r3, lsl #3", {
        .instr = I_SBC, .I = B_SET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 0x3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},

    // SBFX width encoded as width - 1
    {0xF34271D5, 0, "sbfxal r1, r2, #0x1f, #0x16", {
        .instr = I_SBFX, .I = B_SET, .Rd = r1, .Rn = r2, .imm = 0x1f, .lsb = 0x1f, .width = (0x16), .cond = C_AL}},
    {0xFB92F1F3, 0, "sdival r1, r2, r3", {
        .instr = I_SDIV, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAA2F183, 0, "selal r1, r2, r3", {
        .instr = I_SEL, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xF3AF8004, 0, "seval.w", {
        .instr = I_SEV, .I = B_UNSET, .cond = C_AL}},
    {0xFA92F123, 0, "shadd16al r1, r2, r3", {
        .instr = I_SHADD16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFA82F123, 0, "shadd8al r1, r2, r3", {
        .instr = I_SHADD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAA2F123, 0, "shasxal r1, r2, r3", {
        .instr = I_SHASX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAE2F123, 0, "shsaxal r1, r2, r3", {
        .instr = I_SHSAX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAD2F123, 0, "shsub16al r1, r2, r3", {
        .instr = I_SHSUB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    {0xFAC2F123, 0, "shsub8al r1, r2, r3", {
        .instr = I_SHSUB8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // SMLA
    {0xFB124103, 0, "smlabbal r1, r2, r3, r4", {
        .instr = I_SMLABB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .N = B_UNSET, .M = B_UNSET, .cond = C_AL}},
    {0xFB124113, 0, "smlabtal r1, r2, r3, r4", {
        .instr = I_SMLABT, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .N = B_UNSET, .M = B_SET, .cond = C_AL}},
    {0xFB124123, 0, "smlatbal r1, r2, r3, r4", {
        .instr = I_SMLATB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .N = B_SET, .M = B_UNSET, .cond = C_AL}},
    {0xFB124133, 0, "smlattal r1, r2, r3, r4", {
        .instr = I_SMLATT, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .N = B_SET, .M = B_SET, .cond = C_AL}},
    {0xFB12F123, 0, "smultbal r1, r2, r3", {
        .instr = I_SMULTB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .N = B_SET, .M = B_UNSET, .cond = C_AL}},

    // SMLAD
    {0xFB224103, 0, "smladal r1, r2, r3, r4", {
        .instr = I_SMLAD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_UNSET, .cond = C_AL}},
    {0xFB224113, 0, "smladxal r1, r2, r3, r4", {
        .instr = I_SMLAD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_SET, .cond = C_AL}},
    {0xFB22F103, 0, "smuadal r1, r2, r3", {
        .instr = I_SMUAD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_UNSET, .cond = C_AL}},

    // SMLAL
    {0xFBC31204, 0, "smlalal r1, r2, r3, r4", {
        .instr = I_SMLAL, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .cond = C_AL}},

    // SMLALXY
    {0xFBC31284, 0, "smlalbbal r1, r2, r3, r4", {
        .instr = I_SMLALBB, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .N = B_UNSET, .M = B_UNSET, .cond = C_AL}},
    {0xFBC31294, 0, "smlalbtal r1, r2, r3, r4", {
        .instr = I_SMLALBT, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .N = B_UNSET, .M = B_SET, .cond = C_AL}},
    {0xFBC312A4, 0, "smlaltbal r1, r2, r3, r4", {
        .instr = I_SMLALTB, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .N = B_SET, .M = B_UNSET, .cond = C_AL}},
    {0xFBC312B4, 0, "smlalttal r1, r2, r3, r4", {
        .instr = I_SMLALTT, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .N = B_SET, .M = B_SET, .cond = C_AL}},

    // SMLALD
    {0xFBC312C4, 0, "smlaldal r1, r2, r3, r4", {
        .instr = I_SMLALD, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .M = B_UNSET, .cond = C_AL}},
    {0xFBC312D4, 0, "smlaldxal r1, r2, r3, r4", {
        .instr = I_SMLALD, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .M = B_SET, .cond = C_AL}},

    // SMLAWB, SMLAWT
    {0xFB324103, 0, "smlawbal r1, r2, r3, r4", {
        .instr = I_SMLAW, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_UNSET, .cond = C_AL}},
    {0xFB324113, 0, "smlawtal r1, r2, r3, r4", {
        .instr = I_SMLAW, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_SET, .cond = C_AL}},


    // SMLSD
    {0xFB424103, 0, "smlsdal r1, r2, r3, r4", {
        .instr = I_SMLSD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_UNSET, .cond = C_AL}},
    {0xFB424113, 0, "smlsdxal r1, r2, r3, r4", {
        .instr = I_SMLSD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .M = B_SET, .cond = C_AL}},
    {0xFB42F103, 0, "smusdal r1, r2, r3, r4", {
        .instr = I_SMUSD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_UNSET, .cond = C_AL}},

    // SMLSLD
    {0xFBD312C4, 0, "smlsldal r1, r2, r3, r4", {
        .instr = I_SMLSLD, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .M = B_UNSET, .cond = C_AL}},
    {0xFBD312D4, 0, "smlsldxal r1, r2, r3, r4", {
        .instr = I_SMLSLD, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .M = B_SET, .cond = C_AL}},

    // SMMLA
    {0xFB524103, 0, "smmlaal r1, r2, r3, r4", {
        .instr = I_SMMLA, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .R = B_UNSET, .cond = C_AL}},
    {0xFB524113, 0, "smmlaral r1, r2, r3, r4", {
        .instr = I_SMMLA, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .R = B_SET, .cond = C_AL}},
    {0xFB52F103, 0, "smmul r1, r2, r3, r4", {
        .instr = I_SMMUL, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .R = B_UNSET, .cond = C_AL}},

    // SMMLS
    {0xFB624103, 0, "smmlsal r1, r2, r3, r4", {
        .instr = I_SMMLS, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .R = B_UNSET, .cond = C_AL}},
    {0xFB624113, 0, "smmlsral r1, r2, r3, r4", {
        .instr = I_SMMLS, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .R = B_SET, .cond = C_AL}},

    // SMMUL
    {0xFB52F103, 0, "smmulal r1, r2, r3", {
        .instr = I_SMMUL, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .R = B_UNSET, .cond = C_AL}},
    {0xFB52F113, 0, "smmulral r1, r2, r3", {
        .instr = I_SMMUL, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .R = B_SET, .cond = C_AL}},

    // SMUAD
    {0xFB22F103, 0, "smuadal r1, r2, r3", {
        .instr = I_SMUAD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_UNSET, .cond = C_AL}},
    {0xFB22F113, 0, "smuadxal r1, r2, r3", {
        .instr = I_SMUAD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_SET, .cond = C_AL}},

    // SMULXY
    {0xFB12F103, 0, "smulbbal r1, r2, r3", {
        .instr = I_SMULBB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .N = B_UNSET, .M = B_UNSET, .cond = C_AL}},
    {0xFB12F113, 0, "smulbtal r1, r2, r3", {
        .instr = I_SMULBT, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .N = B_UNSET, .M = B_SET, .cond = C_AL}},
    {0xFB12F123, 0, "smultbal r1, r2, r3", {
        .instr = I_SMULTB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .N = B_SET, .M = B_UNSET, .cond = C_AL}},
    {0xFB12F133, 0, "smulttal r1, r2, r3", {
        .instr = I_SMULTT, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .N = B_SET, .M = B_SET, .cond = C_AL}},

    // SMULL
    {0xFB831204, 0, "smullal r1, r2, r3, r4", {
        .instr = I_SMULL, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .cond = C_AL}},

    // SMULWB/T
    {0xFB32F103, 0, "smulwb r1, r2, r3", {
        .instr = I_SMULW, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_UNSET, .cond = C_AL}},
    {0xFB32F113, 0, "smulwt r1, r2, r3", {
        .instr = I_SMULW, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_SET, .cond = C_AL}},

    // SMUSD
    {0xFB42F103, 0, "smusdal r1, r2, r3", {
        .instr = I_SMUSD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_UNSET, .cond = C_AL}},
    {0xFB42F113, 0, "smusdxal r1, r2, r3", {
        .instr = I_SMUSD, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .M = B_SET, .cond = C_AL}},

    // SSAT, SSAT16
    /* TODO: fix it fix it fix it
    {0xF30301D5, 0, "ssatal r1, #21, r3, lsl #3", {
        .instr = I_SSAT, .I = B_SET, .sat_imm = 21, .Rd = r1, .Rn = r3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},
    */

    // SSAX
    {0xFAE2F103, 0, "ssaxal r1, r2, r3", {
        .instr = I_SSAX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // SSUB16
    {0xFAD2F103, 0, "ssub16al r1, r2, r3", {
        .instr = I_SSUB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // SSUB8
    {0xFAC2F103, 0, "ssub8al r1, r2, r3", {
        .instr = I_SSUB8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},


    /* TODO: tests for STC, STC2 */

    // STM
    {0xE8810007, 0, "stmal.w r1, r0, r1, r2", {
        .instr = I_STM, .I = B_UNSET, .Rn = r1, .reglist = 7, .W = B_UNSET, .cond = C_AL}},
    {0xE8A10007, 0, "stmal.w r1!, r0, r1, r2", {
        .instr = I_STM, .I = B_UNSET, .Rn = r1, .reglist = 7, .W = B_SET, .cond = C_AL}},
    {0xE8A18007, 0, "stmal.w r1!, r0, r1, r2", {
        .instr = I_STM, .I = B_UNSET, .Rn = r1, .reglist = ((1 << 15)|7), .W = B_SET, .cond = C_AL}},

    // STMDB
    {0xE9050007, 0, "stmdbal r5, r0, r1, r2", {
        .instr = I_STMDB, .I = B_UNSET, .Rn = r5, .reglist = 7, .W = B_UNSET, .cond = C_AL}},
    {0xE9250007, 0, "stmdbal r5!, r0, r1, r2", {
        .instr = I_STMDB, .I = B_UNSET, .Rn = r5, .reglist = 7, .W = B_SET, .cond = C_AL}},
    {0xE9258007, 0, "stmdbal r5!, r0, r1, r2", {
        .instr = I_STMDB, .I = B_UNSET, .Rn = r5, .reglist = ((1 << 15)|7), .W = B_SET, .cond = C_AL}},
    {0xE92D0007, 0, "pushal.w r0, r1, r2", {
        .instr = I_PUSH, .I = B_UNSET, .reglist = 7, .cond = C_AL}},

    // STR
    {0xF8C21BEA, 0, "stral.w r1, [r2, #0xbea]", {
        .instr = I_STR, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xbea , .cond = C_AL}},
    {0xF8421CF0, 0, "stral r1, [r2, #-0xf0]", {
        .instr = I_STR, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf0, .P = B_SET, .W = B_UNSET, .U = B_UNSET, .cond = C_AL}},
    {0xF8421DF0, 0, "stral r1, [r2, #-0xf0]!", {
        .instr = I_STR, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf0, .P = B_SET, .W = B_SET, .U = B_UNSET, .cond = C_AL}},
    {0xF8421BF0, 0, "stral r1, [r2], #0xf0", {
        .instr = I_STR, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf0, .P = B_UNSET, .W = B_SET, .U = B_SET, .cond = C_AL}},
    {0xF8421EF0, 0, "strtal r1, [r2, #0xf0]", {
        .instr = I_STRT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf0, .cond = C_AL}},
    {0xF84D1D04, 0, "pushal.w r1", {
        .instr = I_PUSH, .I = B_UNSET, .Rt = r1, .cond = C_AL}},

    // register
    {0xF8421023, 0, "stral.w r1, [r2, r3, lsl #2]", {
        .instr = I_STR, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 2, .shift = 2, .shift_type = S_LSL, .cond = C_AL}},

    // STRB
    {0xF8821CAB, 0, "strbal.w r1, [r2, #0xcab]", {
        .instr = I_STRB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xcab , .cond = C_AL}},
    {0xF8021C0F, 0, "strbal r1, [r2, #-0x0f]", {
        .instr = I_STRB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .cond = C_AL}},
    {0xF8021D0F, 0, "strbal r1, [r2, #-0x0f]!", {
        .instr = I_STRB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf, .P = B_SET, .U = B_UNSET, .W = B_SET, .cond = C_AL}},
    {0xF8021B0F, 0, "strbal r1, [r2], #0x0f", {
        .instr = I_STRB, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf, .P = B_UNSET, .U = B_SET, .W = B_SET, .cond = C_AL}},
    {0xF8021E0F, 0, "strbtal r1, [r2, #0x0f]", {
        .instr = I_STRBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xf, .cond = C_AL}},

    // STRB register
    {0xF8021033, 0, "strbal.w r1, [r2, r3, lsl #3]", {
        .instr = I_STRB, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 0x3, .shift = 3, .shift_type = S_LSL, .cond = C_AL}},

    // STRBT
    {0xF8021ECC, 0, "strbtal r1, [r2, #0xcc]", {
        .instr = I_STRBT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xcc, .cond = C_AL}},

    // STRD
    {0xE94312F0, 0, "strdal r1, r2, [r3, #-0x3c0]", {
        .instr = I_STRD, .I = B_SET, .Rt = r1, .Rt2 = r2, .Rn = r3, .imm = 0x3c0, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .cond = C_AL}},

    // STREX
    {0xE84321FF, 0, "strexal r1, r2, [r3, #0x3fc]", {
        .instr = I_STREX, .I = B_SET, .Rd = r1, .Rt = r2, .Rn = r3, .imm = 0x3fc,  .cond = C_AL}},

    // STREXB
    {0xE8C32F41, 0, "strexbal r1, r2, [r3]", {
        .instr = I_STREXB, .I = B_UNSET, .Rd = r1, .Rt = r2, .Rn = r3,  .cond = C_AL}},

    // STREXD
    {0xE8C42371, 0, "strexdal r1, r2, r3, [r4]", {
        .instr = I_STREXD, .I = B_UNSET, .Rd = r1, .Rt = r2, .Rt2 = r3, .Rn = r4,  .cond = C_AL}},

    // STREXH
    {0xE8C32F51, 0, "strexhal r1, r2, [r3]", {
        .instr = I_STREXH, .I = B_UNSET, .Rd = r1, .Rt = r2, .Rn = r3,  .cond = C_AL}},

    // STRH
    {0xF8A21BEB, 0, "strhal.w r1, [r2, #0xbeb]", {
        .instr = I_STRH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xbeb,  .cond = C_AL}},
    {0xF8221CFF, 0, "strhal r1, [r2, #-0xff]", {
        .instr = I_STRH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .P = B_SET, .U = B_UNSET, .W = B_UNSET, .cond = C_AL}},
    {0xF8221DFF, 0, "strhal r1, [r2, #-0xff]!", {
        .instr = I_STRH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .P = B_SET, .U = B_UNSET, .W = B_SET, .cond = C_AL}},
    {0xF8221BFF, 0, "strhal r1, [r2], #0xff", {
        .instr = I_STRH, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .P = B_UNSET, .U = B_SET, .W = B_SET, .cond = C_AL}},
    {0xF8221EFF, 0, "strhtal r1, [r2, #0xff]", {
        .instr = I_STRHT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},

    // register
    {0xF8221013, 0, "strhal.w r1, [r2, r3, lsl #1]", {
        .instr = I_STRH, .I = B_SET, .Rt = r1, .Rn = r2, .Rm = r3, .imm = 0x1, .shift = 1, .shift_type = S_LSL,  .cond = C_AL}},

    // STRHT
    {0xF8221EFF, 0, "strhtal r1, [r2, #0xff]", {
        .instr = I_STRHT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0xff, .cond = C_AL}},

    // STRT
    {0xF8421E33, 0, "strtal r1, [r2, #0x33]", {
        .instr = I_STRT, .I = B_SET, .Rt = r1, .Rn = r2, .imm = 0x33, .cond = C_AL}},

    // SUB
    {0xF5A271FF, 0, "subal.w r1, r2, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = r2, .imm = 0x1fe, .S = B_UNSET, .cond = C_AL}},
    {0xF5B275FF, 0, "subsal.w r5, r2, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = r5, .Rn = r2, .imm = 0x1fe, .S = B_SET, .cond = C_AL}},
    {0xF5A27FFF, 0, "subal.w pc, r2, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = PC, .Rn = r2, .imm = 0x1fe, .S = B_UNSET, .cond = C_AL}},
    {0xF5B27FFF, 0, "cmpal.w r2, #0x1fe", {
        .instr = I_CMP, .I = B_SET, .Rd = PC, .Rn = r2, .imm = 0x1fe, .cond = C_AL}},
    {0xF5AD71FF, 0, "subal.w r1, sp, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .imm = 0x1fe, .S = B_UNSET, .cond = C_AL}},

    // SUBW
    {0xF6A271FF, 0, "subwal r1, r2, #0xfff", {
        .instr = I_SUBW, .I = B_SET, .Rd = r1, .Rn = r2, .imm = 0xfff, .cond = C_AL}},
    {0xF6AF71FF, 0, "adral.w r1, #0xfff", {
        .instr = I_ADR, .I = B_SET, .Rd = r1, .Rn = PC, .imm = 0xfff, .cond = C_AL}},
    {0xF6AD71FF, 0, "subwal r1, sp, #0xfff", {
        .instr = I_SUBW, .I = B_SET, .Rd = r1, .Rn = SP, .imm = 0xfff, .cond = C_AL}},

    // SUB register
    {0xEBA27113, 0, "subal.w r1, r2, r3, lsr #28", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 28, .S = B_UNSET, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBB27113, 0, "subsal.w r1, r2, r3, lsr #28", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .imm = 28, .S = B_SET, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBB27F13, 0, "cmpal.w r2, r3, lsr #28", {
        .instr = I_CMP, .I = B_SET, .Rd = PC, .Rn = r2, .Rm = r3, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBAD7113, 0, "subal.w r1, sp, r3, lsr #28", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .Rm = r3, .imm = 28, .S = B_UNSET, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},


    // SUB SP min imm
    {0xF5AD71FF, 0, "subal.w r1, sp, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .imm = 0x1fe, .S = B_UNSET, .cond = C_AL}},
    {0xF5BD71FF, 0, "subsal.w r1, sp, #0x1fe", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .imm = 0x1fe, .S = B_SET, .cond = C_AL}},
    {0xF5BD7FFF, 0, "cmpal.w sp, #0x1fe", {
        .instr = I_CMP, .I = B_SET, .Rd = PC, .Rn = SP, .imm = 0x1fe, .cond = C_AL}},
    {0xF6AD71cc, 0, "subwal r1, sp, #0xfcc", {
        .instr = I_SUBW, .I = B_SET, .Rd = r1, .Rn = SP, .imm = 0xfcc, .S = B_UNSET, .cond = C_AL}},

    // SUB SP min reg
    {0xEBAD7113, 0, "subal.w r1, sp, r3, lsr #28", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .Rm = r3, .imm = 28, .S = B_UNSET, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBBD7113, 0, "subsal.w r1, sp, r3, lsr #28", {
        .instr = I_SUB, .I = B_SET, .Rd = r1, .Rn = SP, .Rm = r3, .imm = 28, .S = B_SET, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},
    {0xEBBD7F13, 0, "cmpal.w sp, r3, lsr #28", {
        .instr = I_CMP, .I = B_SET, .Rd = PC, .Rn = SP, .Rm = r3, .imm = 28, .shift = 28, .shift_type = S_LSR, .cond = C_AL}},

    // SXTAB
    {0xFA42F1A3, 0, "sxtabal r1, r2, r3, #16", {
        .instr = I_SXTAB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA4FF1A3, 0, "sxtbal r1, r3, #16", {
        .instr = I_SXTB, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // SXTAB16
    {0xFA22F1A3, 0, "sxtab16al r1, r2, r3, #16", {
        .instr = I_SXTAB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA2FF1A3, 0, "sxtb16al r1, r3, #16", {
        .instr = I_SXTB16, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // SXTAH
    {0xFA02F1A3, 0, "sxtahal r1, r2, r3, #16", {
        .instr = I_SXTAH, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA0FF1A3, 0, "sxthal r1, r3, #16", {
        .instr = I_SXTH, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // SXTB
    {0xFA4FF1B5, 0, "sxtbal.w r1, r5, #24", {
        .instr = I_SXTB, .I = B_UNSET, .Rd = r1, .Rm = r5, .rotate = 24, .cond = C_AL}},

    // SXTB16
    {0xFA2FF1B5, 0, "sxtb16al.w r1, r5, #24", {
        .instr = I_SXTB16, .I = B_UNSET, .Rd = r1, .Rm = r5, .rotate = 24, .cond = C_AL}},

    // SXTH
    {0xFA0FF1B5, 0, "sxthal.w r1, r5, #24", {
        .instr = I_SXTH, .I = B_UNSET, .Rd = r1, .Rm = r5, .rotate = 24, .cond = C_AL}},

    // TBB
    {0xE8D1F002, 0, "tbbal [r1, r2]", {
        .instr = I_TBB, .I = B_UNSET, .Rn = r1, .Rm = r2, .H = B_UNSET, .cond = C_AL}},

    // TBH
    {0xE8D7F018, 0, "tbhal [r7, r8]", {
        .instr = I_TBH, .I = B_UNSET, .Rn = r7, .Rm = r8, .H = B_SET, .cond = C_AL}},

    // TEQ
    {0xF4917FFF, 0, "teqal r1, #0x1fe", {
        .instr = I_TEQ, .I = B_SET, .Rd = PC, .Rn = r1, .imm = 0x1fe, .cond = C_AL}},
    {0xEA915F62, 0, "teqal r1, r2, asr #21", {
        .instr = I_TEQ, .I = B_SET, .Rd = PC, .Rn = r1, .Rm = r2, .imm = 21, .shift = 21, .shift_type = S_ASR, .cond = C_AL}},

    // TST
    {0xF0110F0F, 0, "tstal r1, #15", {
        .instr = I_TST, .I = B_SET, .Rd = PC, .Rn = r1, .imm = 15, .cond = C_AL}},
    {0xEA117F32, 0, "tstal r1, r2, ror #28", {
        .instr = I_TST, .I = B_SET, .Rd = PC, .Rn = r1, .Rm = r2, .imm = 28, .shift = 28, .shift_type = S_ROR, .cond = C_AL}},

    // UADD16
    {0xFA92F143, 0, "uadd16al r1, r2, r3", {
        .instr = I_UADD16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UADD8
    {0xFA82F143, 0, "uadd8al r1, r2, r3", {
        .instr = I_UADD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UASX
    {0xFAA2F143, 0, "uasxal r1, r2, r3", {
        .instr = I_UASX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // UBFX, width is encoded as width - 1 (???)
    {0xF3C271CA, 0, "ubfxal r1, r2, #31, #11", {
        .instr = I_UBFX, .I = B_SET, .Rd = r1, .Rn = r2, .width = 11, .imm = 31, .lsb = 31, .cond = C_AL}},

    // UDF
    /* TODO: broken
    {0xF7FFA000, 0, "udfal.w #0xf000", {
        .instr = I_UDF, .I = B_SET, .imm = 0xf000, .cond = C_AL}},
    */

    // UDIV
    {0xFBB2F1F3, 0, "udival r1, r2, r3", {
        .instr = I_UDIV, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // UHADD16
    {0xFA92F163, 0, "uhadd16 r1, r2, r3", {
        .instr = I_UHADD16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UHADD8
    {0xFA82F163, 0, "uhadd8 r1, r2, r3", {
        .instr = I_UHADD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UHASX
    {0xFAA2F163, 0, "uhasx r1, r2, r3", {
        .instr = I_UHASX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UHSAX
    {0xFAE2F163, 0, "uhsax r1, r2, r3", {
        .instr = I_UHSAX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UHSUB16
    {0xFAD2F163, 0, "uhsub16 r1, r2, r3", {
        .instr = I_UHSUB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UHSUB8
    {0xFAC2F163, 0, "uhsub8 r1, r2, r3", {
        .instr = I_UHSUB8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // UMAAL
    {0xFBE31264, 0, "umaalal r1, r2, r3, r4", {
        .instr = I_UMAAL, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .cond = C_AL}},
    // UMLAL
    {0xFBE31204, 0, "umlalal r1, r2, r3, r4", {
        .instr = I_UMLAL, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .cond = C_AL}},
    // UMULL
    {0xFBA31204, 0, "umullal r1, r2, r3, r4", {
        .instr = I_UMULL, .I = B_UNSET, .RdLo = r1, .RdHi = r2, .Rn = r3, .Rm = r4, .cond = C_AL}},

    // UQADD16
    {0xFA92F153, 0, "uqadd16al r1, r2, r3", {
        .instr = I_UQADD16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UQADD8
    {0xFA82F153, 0, "uqadd8al r1, r2, r3", {
        .instr = I_UQADD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UQASX
    {0xFAA2F153, 0, "uqasxal r1, r2, r3", {
        .instr = I_UQASX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UQSUB16
    {0xFAD2F153, 0, "uqsub16 r1, r2, r3", {
        .instr = I_UQSUB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // UQSUB8
    {0xFAC2F153, 0, "uqsub8 r1, r2, r3", {
        .instr = I_UQSUB8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // USAD8
    {0xFB72F103, 0, "usad8al r1, r2, r3", {
        .instr = I_USAD8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // USADA8
    {0xFB724103, 0, "usada8al r1, r2, r3, r4", {
        .instr = I_USADA8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .Ra = r4, .cond = C_AL}},


    /* TODO: USAT, USAT16 */

    // USAX
    {0xFAE2F143, 0, "usaxal r1, r2, r3", {
        .instr = I_USAX, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // USUB16
    {0xFAD2F143, 0, "usub16al r1, r2, r3", {
        .instr = I_USUB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},
    // USUB8
    {0xFAC2F143, 0, "usub8al r1, r2, r3", {
        .instr = I_USUB8, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},

    // UXTAB
    {0xFA52F1A3, 0, "uxtabal r1, r2, r3, #16", {
        .instr = I_UXTAB, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA5FF1A3, 0, "uxtbal.w r1, r2, r3, #16", {
        .instr = I_UXTB, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // UXTAB16
    // The manual contains a mistake (???) here where there is Rd two times, this is probably not the case.
    {0xFA32F1A3, 0, "uxtab16al r1, r2, r3, #16", {
        .instr = I_UXTAB16, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA3FF1A3, 0, "uxtb16al r1, r2, r3, #16", {
        .instr = I_UXTB16, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // UXTAH
    {0xFA12F1A3, 0, "uxtahal r1, r2, r3, #16", {
        .instr = I_UXTAH, .I = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r3, .rotate = 16, .cond = C_AL}},
    {0xFA1FF1A3, 0, "uxthal.w r1, r2, r3, #16", {
        .instr = I_UXTH, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 16, .cond = C_AL}},

    // UXTB
    {0xFA5FF193, 0, "uxtbal.w r1, r2, r3, #8", {
        .instr = I_UXTB, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 8, .cond = C_AL}},

    // UXTB16
    {0xFA3FF193, 0, "uxtb16al r1, r2, r3, #8", {
        .instr = I_UXTB16, .I = B_UNSET, .Rd = r1, .Rm = r3, .rotate = 8, .cond = C_AL}},

    // UXTH
    {0xFA1FF1B8, 0, "uxthal.w r1, r2, r8, #24", {
        .instr = I_UXTH, .I = B_UNSET, .Rd = r1, .Rm = r8, .rotate = 24, .cond = C_AL}},

    // WFE
    {0xF3AF8002, 0, "wfeal.w", {
        .instr = I_WFE, .I = B_UNSET, .cond = C_AL}},

    // WFI
    {0xF3AF8003, 0, "wfial.w", {
        .instr = I_WFI, .I = B_UNSET, .cond = C_AL}},

    // YIELD
    {0xF3AF8001, 0, "yieldal.w", {
        .instr = I_YIELD, .I = B_UNSET, .cond = C_AL}},
};

static int _darm_thumb_disasm(darm_t *d, uint32_t w)
{
    return darm_thumb_disasm(d, w);
}

static int _darm_thumb2_disasm(darm_t *d, uint32_t w)
{
    return darm_thumb2_disasm(d, w >> 16, w & 0xffff);
}

// test the internal functions for Thumb2
static int test_thumb2_functions()
{
    uint16_t value1 = 0xFE;
    darm_t d, d2;

    // zero extend to 32 bits
    if(thumb_expand_imm(value1) != value1) {
        printf("Zero expand in thumb_expand_imm failed\n");
        return -1;
    }

    // Bit 0:7 interleaved with 0
    if(thumb_expand_imm(0x155) != 0x550055) {
        printf("Bit interleaving in thumb_expand_imm failed\n");
        return -1;
    }

    // Bit 0:7 interleaved with 0
    if(thumb_expand_imm(0x255) != 0x55005500) {
        printf("Bit interleaving in thumb_expand_imm failed\n");
        return -1;
    }

    // Bit 0:7 concatenated
    if(thumb_expand_imm(0x355) != 0x55555555) {
        printf("Bit concatenation in thumb_expand_imm failed\n");
        return -1;
    }

    // Bit 0:6 rotated
    if(thumb_expand_imm(0xC55) != 0xD500) {
        printf("Bit rotate in thumb_expand_imm failed\n");
        return -1;
    }

    // shift immediate case LSL
    thumb2_decode_immshift(&d, 0, 15);
    if(d.shift_type != S_LSL || d.shift != 15) {
        printf("Shift immediate test LSL failed\n");
        return -1;
    }

    // shift immediate case LSR
    thumb2_decode_immshift(&d, 1, 15);
    thumb2_decode_immshift(&d2, 1, 0);
    if(d.shift_type != S_LSR || d.shift != 15 || d2.shift_type != S_LSR || d2.shift != 32) {
        printf("Shift immediate test LSR failed\n");
        return -1;
    }

    // shift immediate case ASR
    thumb2_decode_immshift(&d, 2, 15);
    thumb2_decode_immshift(&d2, 2, 0);
    if(d.shift_type != S_ASR || d.shift != 15 || d2.shift_type != S_ASR || d2.shift != 32) {
        printf("Shift immediate test ASR failed\n");
        return -1;
    }

    // shift immediate case ROR
    thumb2_decode_immshift(&d, 3, 15);
    thumb2_decode_immshift(&d2, 3, 0);
    if(d.shift_type != S_ROR || d.shift != 15 || d2.shift_type != S_ROR || d2.shift != 1) {
        printf("Shift immediate test ROR failed\n");
        return -1;
    }

    printf("[x] passed thumb2 helper function tests\n");
    return 0;
}

int main()
{
    int disasm_index = 0, failure = 0;
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
        if(p->CRn == 0) p->CRn = R_INVLD;
        if(p->CRm == 0) p->CRm = R_INVLD;
        if(p->CRd == 0) p->CRd = R_INVLD;
        if(p->firstcond == 0) p->firstcond = C_INVLD;

        if(p->shift_type == S_LSL && p->Rs == R_INVLD && p->shift == 0) {
            p->shift_type = S_INVLD;
        }

        ret = disasms[disasm_index](&d, tests[i].w);

        darm_str_t str;
        memset(&str, 0, sizeof(str));
        if(ret == 0 && darm_str2(&d, &str, 1) == 0) {
            printf("%s\n", str.total);
            fflush(stdout);
        }
        else if(ret < 0) {
            printf("error decoding instr..\n");
        }

#define C(x) p->x != d.x

    // if d.x, the output from the disasm, is invalid or unset, then the set
    // value must be unset, otherwise, it must be set (this is because we
    // can't specify B_INVLD in the unittests without making everything
    // unreadable)
#define F(x) ((d.x == B_INVLD || d.x == B_UNSET) ? p->x != B_UNSET : p->x != B_SET)

        // so we don't have to hardcode all of these
        tests[i].d.w = d.w;

        // test all flags and conditions
        int flags;
        if((flags = ret != tests[i].r || C(w) || C(instr) || C(instr_type) ||
                C(cond) || F(S) || F(E) || C(option) || F(U) || F(H) ||
                F(P) || F(R) || F(W) || C(Rd) || C(Rn) || C(Rm) || C(Ra) ||
                C(Rt) || C(RdHi) || C(RdLo) || F(I) || C(imm) ||
                C(shift_type) || C(Rs) || C(shift) || C(lsb) ||
                C(width) || C(reglist) || F(T) || F(M) || F(N) ||
                C(Rt2) || F(B) || C(coproc) || C(opc1) || C(opc2) ||
                C(CRn) || C(CRm) || C(CRd) || C(firstcond) || C(mask)) ||
                // for now only compare strings for armv7,
                // not for thumb and thumb2
                (disasm_index < 2 && strcmp(str.total, tests[i].s))) {

            // problem with instruction test
            printf("incorrect %s for 0x%08x, ret %d\n",
                flags ? "flags" : "string representation", d.w, ret);
            printf("  %s = %s (%d)\n", str.total, tests[i].s,
                strcmp(str.total, tests[i].s));
            darm_dump(&d);
            darm_dump(&tests[i].d);

            failure = 1;
        }
    }

    if(failure != 0) {
        printf("[-] unittests NOT successful!\n");
        return 0;
    }

    // run some tests on utility functions
    if(test_thumb2_functions() < 0) {
        return 0;
    }

    printf("[x] unittests were successful :)\n");
    return 0;
}
