/*
Copyright (c) 2013-2014, Jurriaan Bremer
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


static const char *g_darm_registers[]  = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
    "r11", "r12", "sp", "lr", "pc",

    "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7", "cr8",
    "cr9", "cr10", "cr11", "cr12", "cr13", "cr14", "cr15",

    "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "d10", "d11",
    "d12", "d13", "d14", "d15", "d16", "d17", "d18", "d19", "d20", "d21",
    "d22", "d23", "d24", "d25", "d26", "d27", "d28", "d29", "d30", "d31",
};

static const char *g_darm_vfp_reg[] = {
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
    "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
};

static const char *g_darm_simd_reg[] = {
    "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8",
    "q9", "q10", "q11", "q12", "q13", "q14", "q15",
};

static const char *g_darm_conditionals[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "al", "al",
};

static const char *g_darm_shifts[] = {
    "lsl", "lsr", "asr", "ror", "rrx",
};

static const char *g_darm_option[] = {
    NULL, NULL, "oshst", "osh", NULL, NULL, "nshst", "nsh",
    NULL, NULL, "ishst", "ish", NULL, NULL, "st", "sy",
};

static const char *g_darm_coproc[] = {
    "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8",
    "p9", "p10", "p11", "p12", "p13", "p14", "p15",
};

static inline uint32_t _ror(uint32_t value, uint32_t rotate)
{
    return (value >> rotate) | (value << (32 - rotate));
}

// The upper four bits define the rotation value, but we have to multiply the
// rotation value by two, so instead of right shifting by eight, we do a
// right shift of seven, effectively avoiding the left shift of one.
static inline uint32_t _arm_expand_imm(uint32_t value)
{
    return _ror(value & 0xff, (value >> 7) & 0x1e);
}

static inline uint32_t _thumb_expand_imm(uint32_t imm12)
{
    if((imm12 >> 10) == 0) {
        uint32_t value = imm12 & 0xff;
        switch ((imm12 >> 8) & 3) {
        case 0:
            return value;

        case 1:
            return (value << 16) | value;

        case 2:
            return (value << 24) | (value << 8);

        case 3:
            return (value << 24) | (value << 16) | (value << 8) | value;
        }
    }
    return _ror(0x80 | (imm12 & 0x7f), imm12 >> 7);
}

static inline uint64_t _simd_expand_imm(uint32_t op,
    uint32_t cmode, uint32_t imm)
{
    uint32_t testimm = 0; uint64_t result = imm;
    switch (cmode >> 1) {
    case 0:
        result |= result << 32;
        break;

    case 1:
        testimm = 1;
        result <<= 8;
        result |= result << 32;
        break;

    case 2:
        testimm = 1;
        result <<= 16;
        result |= result << 32;
        break;

    case 3:
        testimm = 1;
        result <<= 24;
        result |= result << 32;
        break;

    case 4:
        result |= result << 32;
        result |= result << 16;
        break;

    case 5:
        testimm = 1;
        result <<= 8;
        result |= result << 32;
        result |= result << 16;
        break;

    case 6:
        testimm = 1;
        result <<= cmode & 1 ? 16 : 8;
        result |= result << 32;
        break;

    case 7:
        if((cmode & 1) == 0) {
            if(op == 0) {
                result |= result << 32;
                result |= result << 16;
                result |= result << 8;
            }
            else {
                result  = (uint64_t)((imm >> 0) & 1 ? 0xff : 0) << 0;
                result |= (uint64_t)((imm >> 1) & 1 ? 0xff : 0) << 8;
                result |= (uint64_t)((imm >> 2) & 1 ? 0xff : 0) << 16;
                result |= (uint64_t)((imm >> 3) & 1 ? 0xff : 0) << 24;
                result |= (uint64_t)((imm >> 4) & 1 ? 0xff : 0) << 32;
                result |= (uint64_t)((imm >> 5) & 1 ? 0xff : 0) << 40;
                result |= (uint64_t)((imm >> 6) & 1 ? 0xff : 0) << 48;
                result |= (uint64_t)((imm >> 7) & 1 ? 0xff : 0) << 56;
            }
        }
        else {
            if(op == 0) {
                result = (imm & 0x3f) << 19;
                result |= ((imm >> 6) & 1 ? 0x1f : 0x20) << 25;
                result |= (imm & 0x80) << 24;
                result |= result << 32;
            }
            else {
                // TODO undefined
            }
        }
        break;
    }

    if(testimm != 0 && imm == 0) {
        // TODO unpredictable
    }
    return result;
}

static inline uint32_t _extract_field(uint32_t insn,
    uint32_t idx, uint32_t bits)
{
    return (insn >> idx) & ((1 << bits) - 1);
}

static inline void _darm_init(darm_t *d, uint32_t insn)
{
    d->cond = C_AL;
    d->insn = insn;
    d->imm = 0;
    d->shift_type = 0;
    d->long_dest = B_UNSET;
    d->simd_flt = 0.0;
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

        case SM_EXTR3:
            *(uint32_t *)((char *) d + sm[off]) |=
                _extract_field(insn, sm[off+1], sm[off+2]) << sm[off+3];
            off += 4;
            break;

        case SM_FPREG:
            *(uint32_t *)((char *) d + sm[off]) = FP_BASE +
                _extract_field(insn, sm[off+1], 4) +
                _extract_field(insn, sm[off+2], 1) * 16;
            off += 3;
            break;

        case SM_FPREG2:
            if(d->sz == B_SET) {
                *(uint32_t *)((char *) d + sm[off]) = FP_BASE +
                    _extract_field(insn, sm[off+1], 4) +
                    _extract_field(insn, sm[off+2], 1) * 16;
            }
            else {
                *(uint32_t *)((char *) d + sm[off]) = FP_BASE +
                    _extract_field(insn, sm[off+1], 4) * 2 +
                    _extract_field(insn, sm[off+2], 1);
            }
            off += 3;
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

            // Given sign extension is only requested for branch instructions,
            // and their add (U) flag depends on the sign of this immediate,
            // we make sure the immediate is a positive integer, and the add
            // flag is set to add/sub accordingly.
            if((int32_t) d->imm > 0) {
                d->U = B_SET;
            }
            else {
                d->U = B_UNSET;
                d->imm = -d->imm;
            }
            break;

        case SM_BNXOR:
            value = (insn >> sm[off]) ^ (insn >> 26);
            d->imm |= ((value & 1) == 0) << sm[off+1];
            off += 2;
            break;

        case SM_STR:
            d->format = &fmt[sm[off] + sm[off+1]*256];
            off += 2;
            break;

        case SM_ARMExpandImm:
            d->imm = _arm_expand_imm(d->imm);
            break;

        case SM_ThumbExpandImm:
            d->imm = _thumb_expand_imm(d->imm);
            break;

        case SM_AdvSIMDExpandImm:
            d->simd_imm = _simd_expand_imm(d->op, d->cmode, d->imm);
            break;

        case SM_Rt2fromRt:
            d->Rt2 = d->Rt + 1;
            break;

        case SM_RtReglist:
            d->register_list = 1 << d->Rt;
            break;

        case SM_Assign:
            *(uint32_t *)((char *) d + sm[off]) = sm[off+1];
            off += 2;
            break;

        case SM_Imm7Process:
            d->size = 28 - __builtin_clz(d->imm);
            d->imm ^= 8 << d->size;
            break;

        case SM_Imm7Process2:
            d->size = 28 - __builtin_clz(d->imm);
            static uint32_t lut[4] = {16, 32, 64, 64};
            d->imm = lut[d->size] - (d->imm & 0x3f);
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
    if(reg < R_BASE || reg >= R_REGCNT) { \
        DPRINT("Invalid register value: %d", reg); \
        return -1; \
    } \
    APPEND(out, g_darm_registers[reg]);

static int _utoa(uint64_t value, char *out, int base)
{
    char buf[65]; unsigned int i, counter = 0;

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

#define CHECK_RANGE(field, name, max) \
    if(d->field >= max) { \
        DPRINT("Invalid "name" value: %d", d->field); \
        return -1; \
    }

#define CHECK_FLAG(flag, name) \
    if(d->flag != B_UNSET && d->flag != B_SET) { \
        DPRINT("Invalid "name" value: %d", d->flag); \
        return -1; \
    }

int darm_string2(const darm_t *d, darm_string_t *str)
{
    const uint8_t *fmt = d->format; uint32_t value; darm_reg_t reg;
    int next = 1; char *out;

    if(d->instr <= I_INVLD || d->instr >= I_INSTRCNT) {
        DPRINT("Invalid instr: %d [%d, %d]", d->instr, I_INVLD, I_INSTRCNT);
        return -1;
    }

    out = str->mnemonic, str->argcnt = 0, *str->shift = 0;
    APPEND(out, g_darm_instr[d->instr - 1]);

    while (next) {
        switch ((darm_string_opcode_t) *fmt) {
        case STR_RETN: case STR_S: case STR_cond: case STR_wide: case STR_dt:
        case STR_EXCL: case STR_dt2: case STR_dt2u: case STR_dt2i:
        case STR_dt3: case STR_dt4: case STR_dt5: case STR_size:
            break;

        case STR_SHIFT: case STR_SHIFT2:
            *out = 0, out = str->shift;
            break;

        default:
            *out = 0, out = str->arg[str->argcnt++];
        }

        switch ((darm_string_opcode_t) *fmt++) {
        case STR_RETN:
            *out = next = 0;
            break;

        case STR_S:
            if(d->S != B_UNSET && d->S != B_SET && d->S != B_IT) {
                DPRINT("Invalid update value: %d", d->S);
                return -1;
            }

            if(d->S != B_UNSET) {
                *out++ = 's';
            }
            break;

        case STR_dt:
            CHECK_FLAG(U, "unsigned");
            CHECK_RANGE(size, "size", 4);

            *out++ = '.';
            *out++ = d->U == B_SET ? 'u' : 's';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_dt2:
            CHECK_FLAG(F, "floating point");
            CHECK_RANGE(size, "size", 4);

            *out++ = '.';
            *out++ = d->F == B_SET ? 'f' : 's';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_dt2u:
            CHECK_FLAG(F, "floating point");
            CHECK_RANGE(size, "size", 4);

            *out++ = '.';
            *out++ = d->F == B_SET ? 'f' : 'u';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_dt2i:
            CHECK_FLAG(F, "floating point");
            CHECK_RANGE(size, "size", 4);

            *out++ = '.';
            *out++ = d->F == B_SET ? 'f' : 'i';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_dt3:
            CHECK_FLAG(op, "operation");
            CHECK_RANGE(cmode, "cmode", 16);

            *out++ = '.';
            if(d->cmode < 14) {
                *out++ = 'i';
                out += _append_imm(out,
                    d->cmode == 8 || d->cmode == 9 ? 16 : 32);
            }
            else if(d->cmode == 14) {
                *out++ = 'i';
                out += _append_imm(out, d->op == 0 ? 8 : 64);
            }
            else {
                if(d->op == 0) {
                    APPEND(out, "f32");
                }
                else {
                    // TODO undefined
                }
            }
            break;

        case STR_dt4:
            *out++ = '.', *out++ = 'i';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_dt5:
            *out++ = '.', *out++ = 'f';
            out += _append_imm(out, d->sz == B_SET ? 64 : 32);
            break;

        case STR_size:
            *out++ = '.';
            out += _append_imm(out, 8 << d->size);
            break;

        case STR_cond:
            CHECK_RANGE(cond, "conditional state", C_AL+1);
            if(d->cond != C_AL) {
                APPEND(out, g_darm_conditionals[d->cond]);
            }
            break;

        case STR_REG:
            reg = *(darm_reg_t *)((char *) d + *fmt++);
            APPEND_REGISTER(reg);
            break;

        case STR_SIGNRM:
            if(d->U == B_UNSET) {
                *out++ = '-';
            }

            APPEND_REGISTER(d->Rm);
            break;

        case STR_SHIFT:
            CHECK_RANGE(shift_type, "shift type", S_SHFTTYPCNT);
            if(d->shift_type == S_LSL && d->imm == 0) break;

            if(d->shift_type == S_ROR && d->imm == 0) {
                APPEND(out, g_darm_shifts[S_RRX]);
                break;
            }

            static darm_instr_t shift_lookup[4] = {
                [S_LSL] = I_LSL,
                [S_LSR] = I_LSR,
                [S_ASR] = I_ASR,
                [S_ROR] = I_ROR,
            };

            // Omit the shift type if the instruction itself is the
            // shift type.
            if(shift_lookup[d->shift_type] != d->instr) {
                APPEND(out, g_darm_shifts[d->shift_type]);
                *out++ = ' ';
            }

            if(d->shift_type == S_LSR || d->shift_type == S_ASR) {
                value = d->imm == 32 ? 0 : d->imm;
            }
            else {
                value = d->imm;
            }

            *out++ = '#';
            out += _append_imm(out, value);
            break;

        case STR_SHIFT2:
            APPEND(out, g_darm_shifts[d->shift_type]);
            *out++ = ' ';
            APPEND_REGISTER(d->Rs);
            break;

        case STR_IMM:
            *out++ = '#';
            out += _append_imm(out, d->imm);
            break;

        case STR_IMM2:
            *out++ = '#';
            if(d->U == B_UNSET) {
                *out++ = '-';
            }
            out += _append_imm(out, d->imm);
            break;

        case STR_INT:
            value = *(uint32_t *)((char *) d + *fmt++);
            *out++ = '#';
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
                *out++ = '#';
                out += _append_imm(out, d->option);
            }
            else {
                APPEND(out, g_darm_option[d->option]);
            }
            break;

        case STR_EXCL:
            CHECK_FLAG(W, "write-back");

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

        case STR_ENDIAN:
            CHECK_FLAG(E, "endian");
            APPEND(out, d->E ? "be" : "le");
            break;

        // [Rn]
        case STR_MEM:
            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            *out++ = ']';
            break;

        // [Rn, #+/-imm]
        case STR_MEM2:
            CHECK_FLAG(U, "add");

            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            if(d->imm != 0) {
                *out++ = ',', *out++ = ' ', *out++ = '#';
                if(d->U == B_UNSET) {
                    *out++ = '-';
                }
                out += _append_imm(out, d->imm);
            }
            *out++ = ']';
            break;

        // [Rn, #+/-imm]{!}
        // [Rn], #+/-imm
        case STR_MEM2EX:
            CHECK_FLAG(P, "index");
            CHECK_FLAG(U, "add");
            CHECK_FLAG(W, "write-back");

            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            if(d->P == B_SET) {
                if(d->imm != 0) {
                    *out++ = ',', *out++ = ' ', *out++ = '#';
                    if(d->U == B_UNSET) {
                        *out++ = '-';
                    }
                    out += _append_imm(out, d->imm);
                }
                *out++ = ']';
                if(d->W == B_SET) {
                    *out++ = '!';
                }
            }
            else {
                APPEND(out, "], #");
                if(d->U == B_UNSET) {
                    *out++ = '-';
                }
                out += _append_imm(out, d->imm);
            }
            break;

        // [Rn, +/-Rm]
        case STR_MEM3:
            CHECK_FLAG(U, "add");

            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            *out++ = ',', *out++ = ' ';
            if(d->U == B_UNSET) {
                *out++ = '-';
            }
            APPEND_REGISTER(d->Rm);
            *out++ = ']';
            break;

        // [Rn, +/-Rm]{!}
        // [Rn], +/-Rm
        case STR_MEM3EX:
            CHECK_FLAG(P, "index");
            CHECK_FLAG(U, "add");
            CHECK_FLAG(W, "write-back");

            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            if(d->P == B_SET) {
                *out++ = ',', *out++ = ' ';
                if(d->U == B_UNSET) {
                    *out++ = '-';
                }
                APPEND_REGISTER(d->Rm);
                *out++ = ']';
                if(d->W == B_SET) {
                    *out++ = '!';
                }
            }
            else {
                *out++ = ']', *out++ = ' ';
                if(d->U == B_UNSET) {
                    *out++ = '-';
                }
                APPEND_REGISTER(d->Rm);
            }
            break;

        // [Rn, +/-Rm, lsl #13]
        case STR_MEM4:
            CHECK_FLAG(U, "add");

            *out++ = '[';
            APPEND_REGISTER(d->Rn);
            *out++ = ',', *out++ = ' ';

            if(d->U == B_UNSET) {
                *out++ = '-';
            }
            APPEND_REGISTER(d->Rm);

            if(d->shift_type != S_LSL || d->imm != 0) {
                *out++ = ',', *out++ = ' ';

                APPEND(out, g_darm_shifts[d->shift_type]);
                *out++ = ' ';

                if(d->shift_type == S_LSR || d->shift_type == S_ASR) {
                    value = d->imm == 32 ? 0 : d->imm;
                }
                else {
                    value = d->imm;
                }

                *out++ = '#';
                out += _append_imm(out, value);
            }

            *out++ = ']';
            break;

        case STR_wide:
            *out++ = '.', *out++ = 'w';
            break;

        case STR_rotate:
            if(d->rotate == 0) break;
            APPEND(out, "ror #");
            out += _append_imm(out, d->rotate << 3);
            break;

        case STR_Vd:
            CHECK_FLAG(Q, "register size");
            CHECK_FLAG(long_dest, "long destination register");

            if(d->Q == B_SET || d->long_dest == B_SET) {
                APPEND(out, g_darm_simd_reg[(d->Vd - FP_BASE) >> 1]);
            }
            else {
                APPEND(out, g_darm_registers[d->Vd]);
            }
            break;

        case STR_Vd2:
            CHECK_FLAG(sz, "register size");

            if(d->sz == B_SET) {
                APPEND(out, g_darm_registers[d->Vd]);
            }
            else {
                APPEND(out, g_darm_vfp_reg[d->Vd - FP_BASE]);
            }
            break;

        case STR_Vn: case STR_Vm:
            CHECK_FLAG(Q, "register size");

            value = fmt[-1] == STR_Vn ? d->Vn : d->Vm;

            APPEND(out, d->Q == B_SET ?
                g_darm_simd_reg[(value - FP_BASE) >> 1] :
                g_darm_registers[value]);
            break;

        case STR_Vn2: case STR_Vm2:
            CHECK_FLAG(sz, "register size");

            value = fmt[-1] == STR_Vn2 ? d->Vn : d->Vm;

            APPEND(out, d->sz == B_SET ?
                g_darm_registers[value] :
                g_darm_vfp_reg[value - FP_BASE]);
            break;

        case STR_FPREG_S:
            value = *(uint32_t *)((char *) d + *fmt++);
            APPEND(out, g_darm_vfp_reg[value - FP_BASE]);
            break;

        case STR_FPREG_D:
            value = *(uint32_t *)((char *) d + *fmt++);
            APPEND(out, g_darm_registers[value]);
            break;

        case STR_FPREG_Q:
            value = *(uint32_t *)((char *) d + *fmt++);
            APPEND(out, g_darm_simd_reg[(value - FP_BASE) >> 1]);
            break;

        case STR_SIMDIMM:
            *out++ = '#', *out++ = '0', *out++ = 'x';
            out += _utoa(d->simd_imm, out, 16);
            break;

        case STR_FPSCR:
            APPEND(out, "fpscr");
            break;

        case STR_SIMDLIST:
            // TODO
            break;

        case STR_SIMDFLT:
            *out++ = '#';
            // TODO precision
            out += sprintf(out, "%f", d->simd_flt);
            break;
        }
    }

    *out = 0, out = str->total;
    APPEND(out, str->mnemonic);
    for (uint32_t idx = 0; idx < str->argcnt; idx++) {
        if(idx != 0) *out++ = ',';
        *out++ = ' ';
        APPEND(out, str->arg[idx]);
    }
    if(str->shift[0] != 0) {
        *out++ = ',', *out++ = ' ';
        APPEND(out, str->shift);
    }
    *out = 0;
    return 0;
}

int darm_string(const darm_t *d, char *out)
{
    darm_string_t str;
    if(darm_string2(d, &str) == 0) {
        APPEND(out, str.total);
        return *out = 0;
    }
    return -1;
}
