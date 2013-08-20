#include <stdio.h>
#include <string.h>
#include "../darm.h"

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
    {0xe7c8411f, 0, "bfc r4, #2, #6", {
        .instr = I_BFC, .instr_type = T_ARM_BITS, .cond = C_AL, .Rd = 4,
        .lsb = 2, .width = 6}},
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
        .Rn = SP, .W = 1, .reglist = b11110110}},
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
        .Rn = 2, .Rd = 1, .rotate = b1000, .Rm = 4}},
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
        int flags;
        if((flags = ret != tests[i].r || C(w) || C(instr) || C(instr_type) ||
                C(cond) || F(S) || F(E) || C(option) || F(U) || F(H) ||
                F(P) || F(R) || F(W) || C(Rd) || C(Rn) || C(Rm) || C(Ra) ||
                C(Rt) || C(RdHi) || C(RdLo) || F(I) || C(imm) ||
                C(shift_type) || C(Rs) || C(shift) || C(lsb) ||
                C(width) || C(reglist) || F(T) || F(M) || F(N) ||
                C(Rt2) || F(B) || C(coproc) || C(opc1) || C(opc2) ||
                C(CRn) || C(CRm) || C(CRd) || C(firstcond) || C(mask)) ||
                strcmp(str.total, tests[i].s)) {
            // leave ugly code
            printf("incorrect %s for 0x%08x, ret %d\n",
                flags ? "flags" : "string representation", d.w, ret);
            printf("  %s = %s (%d)\n", str.total, tests[i].s,
                strcmp(str.total, tests[i].s));
            darm_dump(&d);
            darm_dump(&tests[i].d);
            failure = 1;
        }
    }
    if(failure == 0) {
        printf("[x] unittests were successful\n");
    }
    return failure;
}
