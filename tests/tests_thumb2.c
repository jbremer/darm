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
        .instr = I_POP, .reglist = 0x4AA9, .cond = C_AL}},
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
        .instr = I_PLD, .I = B_SET, .Rn = r2, .U = B_UNSET, .P = B_SET, .W = B_UNSET, .imm = 0xff, .cond = C_AL}},

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
    {0xEA4F7102, 0, "lsl.w r1, r2, #0x1c", {
        .instr = I_LSL, .I = B_SET, .S = B_UNSET, .Rd = r1, .Rm = r2, .imm = 0x1c, .shift = 0x1c, .shift_type = S_LSL, .cond = C_AL}},
    {0xEA5F0102, 0, "movs.w r1, r2", {
        .instr = I_MOV, .I = B_SET, .S = B_SET, .Rd = r1, .Rm = r2, .cond = C_AL}},
    {0xFA12F103, 0, "lsls.w r1, r2, r3", {
        .instr = I_LSL, .I = B_UNSET, .S = B_SET, .Rd = r1, .Rn = r2, .Rm = r3, .cond = C_AL}},


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
		F(J1) || F(J2)
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

