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

    {0xF55152AA, 0, "adc r2, r1 #0xdaa", {
        .instr = I_ADC, .I = B_SET, .imm = (0x1540), .S = 1, .Rn = 1, .Rd = 2, .cond = C_AL}},
    {0xF14102CC, 0, "adc r2, r1 #0xcc", {
        .instr = I_ADC, .I = B_SET, .imm = (0xCC), .S = 0, .Rn = 1, .Rd = 2, .cond = C_AL}},
    {0xEB4211F3, 0, "adc.w r1, r2, r3 ror #7", {
        .instr = I_ADC, .I = B_SET, .imm = 0x7, .S = 0, .Rd = 1, .Rn = 2, .Rm = 3, .cond = C_AL, .shift_type = S_ROR, .shift = 0x7}},
    {0xF51251AF, 0, "adds.w r1, r2 #0xdaf", {
        .instr = I_ADD, .I = B_SET, .imm = 5600, .S = 1, .Rd = 1, .Rn = 2, .cond = C_AL}},
    // exception in ADD function -> CMN
    {0xF5125FAF, 0, "cmn r2 #0xdaf", {
        .instr = I_CMN, .I = B_SET, .imm = 5600, .S = 0, .Rd = PC, .Rn = 2, .cond = C_AL}},
    // exception in ADD function -> ADD (SP plus imm)
    {0xF51D51AF, 0, "adds.w r1, SP #0xdaf", {
        .instr = I_ADD, .I = B_SET, .imm = 5600, .S = 1, .Rd = 1, .Rn = SP, .cond = C_AL}},
    // exception in ADD (SP plus imm) function -> CMN
    {0xF51D5FAF, 0, "cmn SP #0xdaf", {
        .instr = I_CMN, .I = B_SET, .imm = 5600, .S = 0, .Rd = PC, .Rn = SP, .cond = C_AL}},
    {0xF60271AB, 0, "addw r1, r2, #0xfab", {
        .instr = I_ADDW, .I = B_SET, .imm = 0xFAB, .Rd = r1, .Rn = r2, .cond = C_AL}},
    {0xF60F71AC, 0, "adr r1 #0xfac", {
        .instr = I_ADR, .I = B_SET, .imm = 0xFAC, .Rd = r1, .Rn = PC, .cond = C_AL}},



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
                C(CRn) || C(CRm) || C(CRd) /*|| C(firstcond)*/ || C(mask)

                ) {


	    print_failure("");
            printf("Testing instruction: %s\n", thumb2_tests[i].s);

            darm_dump(&d);
            darm_dump(&thumb2_tests[i].d);
	    return 1;


        } else {
	   print_success("");
           printf("Testing instruction: %s\n", thumb2_tests[i].s);
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

