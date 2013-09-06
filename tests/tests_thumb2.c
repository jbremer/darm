#include <stdio.h>
#include <string.h>
#include "../darm.h"
#include "../thumb2.h"
#include "tests.h"

struct {
    uint32_t w;
    int r;
    const char *s;
    darm_t d;
} thumb2_tests[] = {

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
        .instr = I_TST, .I = B_SET, .S = B_UNSET, .imm = 0x5, .Rn = r2, .Rm = r3, .shift = 5, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA5F11A2, 0, "asrsal.w r1, r2, #6", {
        .instr = I_ASR, .I = B_SET, .S = B_SET, .imm = 0x6, .Rd = r1, .Rm = r2, .shift = 6, .shift_type = S_ASR, .cond = C_AL}},
    {0xFA42F104, 0, "asral.w r1, r2, r4", {
        .instr = I_ASR, .I = B_UNSET, .S = B_UNSET, .Rd = r1, .Rn = r2, .Rm = r4, .cond = C_AL}},

    /* Branch */
    {0xF43F8801, 0, "beq.w #-266238", {
        .instr = I_B, .I = B_SET, .S = B_SET, .imm = 0xFFFBF002, .J1 = B_UNSET, .J2 = B_SET,  .cond = C_EQ}},
    {0xF30AAFF6, 0, "bgt.w #0xcafec", {
        .instr = I_B, .I = B_SET, .S = B_UNSET, .imm = 0xCAFEC, .J1 = B_SET, .J2 = B_SET,  .cond = C_GT}},
    {0xF0AF975D, 0, "bal.w #0xcafeba", {
        .instr = I_B, .I = B_SET, .S = B_UNSET, .imm = 0xCAFEBA, .J1 = B_UNSET, .J2 = B_UNSET,  .cond = C_AL}},
    {0xF55FB6BA, 0, "bal.w #-6947468", {
        .instr = I_B, .I = B_SET, .S = B_SET, .imm = 0xFF95FD74, .J1 = B_SET, .J2 = B_UNSET,  .cond = C_AL}},
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
        .instr = I_BL, .I = B_SET, .S = B_UNSET, .imm = 0xdeadbe, .J1 = B_UNSET, .J2 = B_UNSET, .cond = C_AL}},
    {0xF5EAFEDF, 0, "blal #-2183746", {
        .instr = I_BL, .I = B_SET, .S = B_SET, .imm = 0xffdeadbe, .J1 = B_SET, .J2 = B_SET, .cond = C_AL}},
    {0xF00FC78E, 0, "blxal #0xc0ff1c", {
        .instr = I_BLX, .I = B_SET, .S = B_UNSET, .imm = 0xc0ff1c, .J1 = B_UNSET, .J2 = B_UNSET, .H = B_SET, .cond = C_AL}},
    {0xF40FEF8E, 0, "blxal #-4128996", {
        .instr = I_BLX, .I = B_SET, .S = B_SET, .imm = 0xffc0ff1c, .J1 = B_SET, .J2 = B_SET, .H = B_SET, .cond = C_AL}},
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
        .instr = I_MOV, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},
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
        .instr = I_MOV, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xEA5F0102, 0, "movsal.w r1, r2", {
        .instr = I_MOV, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},

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

    /* TODO: uncomment this when STR has been fixed
    {0xF84D9D04, 0, "pushal.w r9", {
        .instr = I_PUSH, .I = B_UNSET, .Rt = r9, .cond = C_AL}},
    */

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

    {0xF34271D5, 0, "sbfxal r1, r2, #0x1f, #0x15", {
        .instr = I_SBFX, .I = B_SET, .Rd = r1, .Rn = r2, .imm = 0x1f, .lsb = 0x1f, .width = 0x15, .cond = C_AL}},
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




};


int test_thumb2_instructions() {
    for (uint32_t i = 0; i < ARRAYSIZE(thumb2_tests); i++) {
        darm_t d; int ret = 0;
        char buf[100];

        // update the registers in the tests in order not to be 0, but R_INVLD
        // instead

        darm_t *p = &thumb2_tests[i].d;

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

	if ((ret = darm_thumb2_disasm(&d, (thumb2_tests[i].w & 0xFFFF0000) >> 16, thumb2_tests[i].w & 0xFFFF)) == -1) {
            sprintf(buf,"Test failed on instruction %i:%s", i, thumb2_tests[i].s);
            print_failure(buf);
	    return 1;
        }

        // so we don't have to hardcode all of these
        d.w = thumb2_tests[i].d.w;

        int flags; ret = 0;
        if(C(w) || C(instr) || C(instr_type) ||
                C(cond) || F(S) || F(E) || C(option) || F(U) || F(H) ||
                F(P) || F(R) || F(W) || C(Rd) || C(Rn) || C(Rm) || C(Ra) ||
                C(Rt) || C(RdHi) || C(RdLo) || F(I) || C(imm) || 
                C(shift_type) || C(Rs) || C(shift) || C(lsb) ||
                C(width) || C(reglist) || F(T) || F(M) || F(N) ||
                C(Rt2) || F(B) || C(coproc) || C(opc1) || C(opc2) ||
                C(CRn) || C(CRm) || C(CRd) /*|| C(firstcond)*/ || C(mask) ||
		F(J1) || F(J2) || C(sat_imm)
                ) {


	    print_failure("");
            printf("%s\n", thumb2_tests[i].s);

            darm_dump(&d);
            darm_dump(&thumb2_tests[i].d);
	    return 1;


        } else {
	   print_success("");
           printf("%s\n", thumb2_tests[i].s);
	}
    }


}



// Test the internal functions of thumb2
int test_thumb2_functions() {

    int failure = 0;

    uint16_t value1 = 0xFE;
    darm_t d, d2;

    // Zero extend to 32 bits
    if (thumb_expand_imm(value1) != value1) {
        print_failure("Zero expand in thumb_expand_imm failed\n");
        failure = 1;
    }

    // Bit 0:7 interleaved with 0
    if (thumb_expand_imm(0x155) != 0x550055) {
        print_failure("Bit interleaving in thumb_expand_imm failed\n");
        failure = 1;
    }

    // Bit 0:7 interleaved with 0
    if (thumb_expand_imm(0x255) != 0x55005500) {
        print_failure("Bit interleaving in thumb_expand_imm failed\n");
        failure = 1;
    }

    // Bit 0:7 concatenated
    if (thumb_expand_imm(0x355) != 0x55555555) {
        print_failure("Bit concatenation in thumb_expand_imm failed\n");
        failure = 1;
    }

    // Bit 0:6 rotated
    if (thumb_expand_imm(0xC55) != 0xD500) {
        print_failure("Bit rotate in thumb_expand_imm failed\n");
        failure = 1;
    }

    // shift immediate case LSL
    thumb2_decode_immshift(&d, 0, 15); 
    if (d.shift_type != S_LSL || d.shift != 15) {
        print_failure("Shift immediate test LSL failed\n");
        failure = 1;
    }

    // shift immediate case LSR
    thumb2_decode_immshift(&d, 1, 15);
    thumb2_decode_immshift(&d2, 1, 0);
    if (d.shift_type != S_LSR || d.shift != 15 || d2.shift_type != S_LSR || d2.shift != 32) {
        print_failure("Shift immediate test LSR failed\n");
        failure = 1;
    }

    // shift immediate case ASR
    thumb2_decode_immshift(&d, 2, 15);
    thumb2_decode_immshift(&d2, 2, 0);
    if (d.shift_type != S_ASR || d.shift != 15 || d2.shift_type != S_ASR || d2.shift != 32) {
        print_failure("Shift immediate test ASR failed\n");
        failure = 1;
    }

    // shift immediate case ROR
    thumb2_decode_immshift(&d, 3, 15);
    thumb2_decode_immshift(&d2, 3, 0);
    if (d.shift_type != S_ROR || d.shift != 15 || d2.shift_type != S_ROR || d2.shift != 1) {
        print_failure("Shift immediate test ROR failed\n");
        failure = 1;
    }


    if (failure == 0) 
        print_success("Passed thumb2 helper function tests\n");
    
    return failure;
}

