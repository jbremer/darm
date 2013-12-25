/*
Copyright (c) 2013, Jurriaan Bremer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the darm developer(s) nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdint.h>
#include "darm.h"
#include "darm-instr.h"
#include "darm-internal.h"
#include "darm-tables.h"

#if DARM_DBGLVL >= 1
#define DPRINT(fmt, ...) \
    fprintf(stderr, "%s:%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DPRINT(fmt, ...) (void)0
#endif

#define ROR(val, rotate) (((val) >> (rotate)) | ((val) << (32 - (rotate))))

// The upper four bits define the rotation value, but we have to multiply the
// rotation value by two, so instead of right shifting by eight, we do a
// right shift of seven, effectively avoiding the left shift of one.
#define ARMExpandImm(imm12) ROR((imm12) & 0xff, ((imm12) >> 7) & 0x1e)

static const char *g_darm_registers[]  = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
    "r11", "r12", "SP", "LR", "PC",

    "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7", "cr8",
    "cr9", "cr10", "cr11", "cr12", "cr13", "cr14", "cr15",
};

static const char *g_darm_conditionals[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "al", "al",
};

static const char *g_darm_option[] = {
    NULL, NULL, "oshst", "osh", NULL, NULL, "nshst", "nsh",
    NULL, NULL, "ishst", "ish", NULL, NULL, "st", "sy",
};

static const char *g_darm_coproc[] = {
    "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8",
    "p9", "p10", "p11", "p12", "p13", "p14", "p15",
};

static inline uint32_t _extract_field(uint32_t insn,
    uint32_t idx, uint32_t bits)
{
    return (insn >> idx) & ((1 << bits) - 1);
}

static inline void _darm_init(darm_t *d, uint32_t insn)
{
    d->insn = insn;
    d->imm = 0;
}

// Disassembles any instruction according to the state machine and lookup
// table. The input instruction can be either 16 or 32 bit, depending on
// the target state machine (which will have been created accordingly.)
static int _darm_disassemble(darm_t *d, uint32_t insn,
    const uint8_t *sm, const uint16_t *lut, const uint8_t *fmt)
{
    _darm_init(d, insn);

    uint32_t off = 0, value;
    while (1) {
        switch ((darm_sm_opcode_t) sm[off++]) {
        case SM_HLT:
            return -1;

        case SM_STEP:
            value = (insn >> sm[off]) & 1;
            off = lut[sm[off+1] + sm[off+2]*256 + value];
            break;

        case SM_CMP4:
            value = _extract_field(insn, sm[off], 4) == sm[off+1];
            off = lut[sm[off+2] + sm[off+3]*256 + value];
            break;

        case SM_TBL5:
            value = _extract_field(insn, sm[off], 5);
            off = lut[sm[off+1] + sm[off+2]*256 + value];
            break;

        case SM_RETN:
            return 0;

        case SM_INSTR:
            d->instr = sm[off] + sm[off+1]*256;
            off += 2;
            break;

        case SM_EXTR:
            *(uint32_t *)((char *) d + sm[off]) =
                _extract_field(insn, sm[off+1], sm[off+2]);
            off += 3;
            break;

        case SM_EXTR2:
            *(uint32_t *)((char *) d + sm[off]) =
                _extract_field(insn, sm[off+1], sm[off+2]) + sm[off+3];
            off += 4;
            break;

        case SM_IMM:
            d->imm |= _extract_field(insn, 0, sm[off++]);
            break;

        case SM_IMM2:
            d->imm |= _extract_field(insn, sm[off+1], sm[off]) << sm[off+2];
            off += 3;
            break;

        case SM_SIGN:
            // See the VariableSignExtend section at
            // http://graphics.stanford.edu/~seander/bithacks.html
            value = sizeof(uint32_t) * 8 - sm[off++];
            d->imm = (int32_t)(d->imm << value) >> value;
            break;

        case SM_STR:
            d->format = &fmt[sm[off] + sm[off+1]*256];
            off += 2;
            break;

        case SM_ARMExpandImm:
            d->imm = ARMExpandImm(d->imm);
            break;
        }
    }
    return 0;
}

int darm_armv7(darm_t *d, uint32_t insn)
{
    return _darm_disassemble(d, insn, g_armv7_sm, g_armv7_lut, g_armv7_fmt);
}

int darm_thumb(darm_t *d, uint16_t w, uint16_t w2)
{
    return _darm_disassemble(d, (w << 16) | w2,
        g_thumb_sm, g_thumb_lut, g_thumb_fmt);
}

#define APPEND(out, ptr) \
    do { \
        const char *p = ptr; \
        if(p != NULL) while (*p != 0) *out++ = *p++; \
    } while (0);

#define APPEND_REGISTER(reg) \
    if(reg < R_BASE || reg >= R_REGCNT) return -1; \
    APPEND(out, g_darm_registers[reg]);

static int _utoa(unsigned int value, char *out, int base)
{
    char buf[30]; unsigned int i, counter = 0;

    if(value == 0) {
        buf[counter++] = '0';
    }

    for (; value != 0; value /= base) {
        buf[counter++] = "0123456789abcdef"[value % base];
    }

    for (i = 0; i < counter; i++) {
        out[i] = buf[counter - i - 1];
    }

    return counter;
}

static int _append_imm(char *out, uint32_t imm)
{
    const char *base = out;
    if(imm > 0x400) {
        *out++ = '0';
        *out++ = 'x';
        out += _utoa(imm, out, 16);
    }
    else {
        out += _utoa(imm, out, 10);
    }
    return out - base;
}

int darm_reglist(uint16_t reglist, char *out)
{
    char *base = out;

    *out++ = '{';

    // Support for empty register lists.
    if(reglist == 0) out++;

    while (reglist != 0) {
        // Count trailing zeros.
        int32_t reg, start = __builtin_ctz(reglist);

        APPEND(out, g_darm_registers[start]);

        for (reg = start; reg == __builtin_ctz(reglist); reg++) {
            reglist &= ~(1 << reg);
        }

        // If reg is not start + 1, then we have a series of consecutive
        // registers.
        if(reg != start + 1) {
            // If reg is start + 2, then we have two consecutive registers,
            // but we prefer the notation {r0,r1} over {r0-r1} in that case.
            *out++ = reg == start + 2 ? ',' : '-';
            APPEND(out, g_darm_registers[reg-1]);
        }
        *out++ = ',';
    }

    out[-1] = '}', *out = 0;
    return out - base;
}

int darm_string(const darm_t *d, char *out)
{
    const uint8_t *fmt = d->format; uint32_t value; darm_reg_t reg;
    int first = 1;

    if(d->instr == I_INVLD || d->instr >= I_INSTRCNT) {
        DPRINT("Invalid instr: %d [%d, %d]", d->instr, I_INVLD, I_INSTRCNT);
        return -1;
    }
    APPEND(out, g_darm_instr[d->instr - 1]);

    while (1) {
        switch ((darm_string_opcode_t) *fmt) {
        case STR_RETN: case STR_S: case STR_cond:
            break;

        default:
            if(first != 0) {
                first = 0;
            }
            else {
                *out++ = ',';
            }
            *out++ = ' ';
        }

        switch ((darm_string_opcode_t) *fmt++) {
        case STR_RETN:
            *out = 0;
            return 0;

        case STR_S:
            if(d->S == B_INVLD) {
                DPRINT("S flag has not been set");
                return -1;
            }
            if(d->S == B_SET) {
                *out++ = 's';
            }
            break;

        case STR_cond:
            if(d->cond < C_BASE || d->cond > C_AL) {
                DPRINT("Invalid conditional state: %d", d->cond);
                return -1;
            }
            if(d->cond != C_AL) {
                APPEND(out, g_darm_conditionals[d->cond]);
            }
            break;

        case STR_REG:
            reg = *(darm_reg_t *)((char *) d + *fmt++);
            APPEND_REGISTER(reg);
            break;

        case STR_REG_CONST:
            reg = *fmt++;
            APPEND_REGISTER(reg);
            break;

        case STR_SHIFT:
            break;

        case STR_IMM:
            out += _append_imm(out, d->imm);
            break;

        case STR_INT:
            value = *(uint32_t *)((char *) d + *fmt++);
            out += _append_imm(out, value);
            break;

        case STR_REGLIST:
            out += darm_reglist(d->register_list, out);
            break;

        case STR_OPTION:
            if(d->option < O_BASE || d->option >= O_OPTIONCNT) {
                DPRINT("Invalid option value: %d", d->option);
                return -1;
            }

            if(g_darm_option[d->option] == NULL) {
                out += _append_imm(out, d->option);
            }
            else {
                APPEND(out, g_darm_option[d->option]);
            }
            break;

        case STR_LABEL:
            APPEND(out, "pc");
            if((int32_t) d->imm < 0) {
                *out++ = '-';
                out += _append_imm(out, -d->imm);
            }
            else {
                *out++ = '+';
                out += _append_imm(out, d->imm);
            }
            break;

        case STR_EXCL:
            if(d->W == B_INVLD) {
                DPRINT("W flag has not been set");
                return -1;
            }

            if(d->W == B_SET) {
                *out++ = '!';
            }
            break;

        case STR_COPROC:
            if(d->coproc < P_BASE || d->coproc >= P_PROCCNT) {
                DPRINT("Invalid coproc value: %d", d->coproc);
                return -1;
            }

            APPEND(out, g_darm_coproc[d->coproc]);
            break;
        }
    }
}
