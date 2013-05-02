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
#include <ctype.h>
#include <stdint.h>
#include "darm.h"

#define APPEND(out, ptr) \
    do { \
        const char *p = ptr; \
        if(p != NULL) while (*p != 0) *out++ = *p++; \
    } while (0);

int utoa(unsigned int value, char *out, int base)
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

int darm_str(const darm_t *d, darm_str_t *str)
{
    if(d->instr == I_INVLD || d->instr >= ARRAYSIZE(darm_mnemonics)) {
        return -1;
    }

    // the format string index
    uint32_t idx = 0;

    // the offset in the format string
    uint32_t off = 0;

    // argument index
    uint32_t arg = 0;

    // pointers to the arguments
    char *args[] = {str->arg[0], str->arg[1], str->arg[2], str->arg[3]};

    // ptr to the output mnemonic
    char *mnemonic = str->mnemonic;
    APPEND(mnemonic, darm_mnemonic_name(d->instr));

    char *shift = str->shift;

    const char **ptrs = armv7_format_strings[d->instr];
    if(ptrs[0] == NULL) return -1;

    for (char ch; (ch = ptrs[idx][off]) != 0; off++) {
        switch (ch) {
        case 's':
            if(d->S == B_SET) {
                *mnemonic++ = 'S';
            }
            continue;

        case 'c':
            APPEND(mnemonic, darm_condition_name(d->cond, 1));
            continue;

        case 'd':
            if(d->Rd == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->Rd));
            arg++;
            continue;

        case 'n':
            if(d->Rn == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->Rn));
            arg++;
            continue;

        case 'm':
            if(d->Rm == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->Rm));
            arg++;
            continue;

        case 'a':
            if(d->Ra == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->Ra));
            arg++;
            continue;

        case 't':
            if(d->Rt == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->Rt));
            arg++;
            continue;

        case '2':
            // first check if Rt2 is actually set
            if(d->Rt2 != R_INVLD) {
                APPEND(args[arg], darm_register_name(d->Rt2));
                arg++;
                continue;
            }
            // for some instructions, Rt2 = Rt + 1
            else if(d->Rt != R_INVLD) {
                APPEND(args[arg], darm_register_name(d->Rt + 1));
                arg++;
                continue;
            }
            break;

        case 'h':
            if(d->RdHi == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->RdHi));
            arg++;
            continue;

        case 'l':
            if(d->RdLo == R_INVLD) break;
            APPEND(args[arg], darm_register_name(d->RdLo));
            arg++;
            continue;

        case 'i':
            // check if an immediate has been set
            if(d->I != B_SET) break;

            *args[arg]++ = '#';
            if(d->imm > 0x1000) {
                *args[arg]++ = '0';
                *args[arg]++ = 'x';
                args[arg] += utoa(d->imm, args[arg], 16);
            }
            else {
                args[arg] += utoa(d->imm, args[arg], 10);
            }
            arg++;
            continue;

        case 'S':
            // is there even a shift?
            if(d->shift_type == S_INVLD) continue;

            if(d->P == B_SET) {
                // we're still inside the memory address
                shift = args[arg] - 1;
                *shift++ = ',';
                *shift++ = ' ';
            }

            if(d->Rs == R_INVLD) {
                const char *type; uint32_t imm;
                if(darm_immshift_decode(d, &type, &imm) == 0) {
                    APPEND(shift, type);
                    *shift++ = ' ';
                    *shift++ = '#';
                    shift += utoa(imm, shift, 10);
                }
                else if(d->P == B_SET) {
                    // we're still in the memory address, but there was
                    // no shift, so we have to revert the shift pointer so
                    // it will write a closing bracket again
                    shift -= 2;
                }
            }
            else {
                APPEND(shift, darm_shift_type_name(d->shift_type));
                *shift++ = ' ';
                APPEND(shift, darm_register_name(d->Rs));
            }

            if(d->P == B_SET) {
                // close the memory address
                *shift++ = ']';

                // reset shift
                args[arg] = shift;
                shift = str->shift;
            }
            continue;

        case '!':
            if(d->W == B_SET) {
                *args[arg-1]++ = '!';
            }
            continue;

        case 'e':
            args[arg] += utoa(d->E, args[arg], 10);
            continue;

        case 'x':
            if(d->M == B_SET) {
                *mnemonic++ = 'x';
            }
            continue;

        case 'X':
            // if the flags are not set, then this instruction doesn't take
            // the (B|T)(B|T) postfix
            if(d->N == B_INVLD || d->M == B_INVLD) break;

            *mnemonic++ = d->N == B_SET ? 'T' : 'B';
            *mnemonic++ = d->M == B_SET ? 'T' : 'B';
            continue;

        case 'R':
            if(d->R == B_SET) {
                *mnemonic++ = 'R';
            }
            continue;

        case 'T':
            APPEND(mnemonic, d->T == B_SET ? "TB" : "BT");
            continue;

        case 'r':
            if(d->reglist != 0) {
                args[arg] += darm_reglist(d->reglist, args[arg]);
            }
            else {
                *args[arg]++ = '{';
                APPEND(args[arg], darm_register_name(d->Rt));
                *args[arg]++ = '}';
            }
            continue;

        case 'L':
            *args[arg]++ = '#';
            args[arg] += utoa(d->lsb, args[arg], 10);
            arg++;
            continue;

        case 'w':
            *args[arg]++ = '#';
            args[arg] += utoa(d->width, args[arg], 10);
            arg++;
            continue;

        case 'o':
            *args[arg]++ = '#';
            args[arg] += utoa(d->option, args[arg], 10);
            arg++;
            continue;

        case 'B':
            *args[arg]++ = '[';
            APPEND(args[arg], darm_register_name(d->Rn));

            // if post-indexed or the index is not even set, then we close
            // the memory address
            if(d->P != B_SET) {
                *args[arg++]++ = ']';
            }
            else {
                *args[arg]++ = ',';
                *args[arg]++ = ' ';
            }
            continue;

        case 'O':
            // if the Rm operand is set, then this is about the Rm operand,
            // otherwise it's about the immediate
            if(d->Rm != R_INVLD) {

                // negative offset
                if(d->U == B_UNSET) {
                    *args[arg]++ = '-';
                }

                APPEND(args[arg], darm_register_name(d->Rm));

                // if post-indexed this was a stand-alone operator one
                if(d->P == B_UNSET) {
                    arg++;
                }
            }
            // if there's an immediate, append it
            else if(d->imm != 0) {
                // negative offset?
                APPEND(args[arg], d->U == B_UNSET ? "#-0x" : "#0x");
                args[arg] += utoa(d->imm, args[arg], 16);
            }
            else {
                // there's no immediate, so we have to remove the ", " which
                // was introduced by the base register of the memory address
                args[arg] -= 2;
            }

            // if pre-indexed, close the memory address, but don't increase
            // arg so we can alter it in the shift handler
            if(d->P == B_SET) {
                *args[arg]++ = ']';
            }
            continue;

        case 'b':
            // BLX first checks for branch and only then for the conditional
            // version which takes the Rm as operand, so let's see if the
            // branch stuff has been initialized yet
            if(d->instr == I_BLX && d->H == B_INVLD) break;

            APPEND(args[arg], "#0x");
            args[arg] += utoa(d->imm, args[arg], 16);
            continue;

        case 'M':
            *args[arg]++ = '[';
            APPEND(args[arg], darm_register_name(d->Rn));
            *args[arg]++ = ',';
            *args[arg]++ = ' ';

            // if the Rm operand is defined, then we use that optionally with
            // a shift, otherwise there might be an immediate value as offset
            if(d->Rm != R_INVLD) {
                APPEND(args[arg], darm_register_name(d->Rm));

                const char *type; uint32_t imm;
                if(darm_immshift_decode(d, &type, &imm) == 0) {
                    *args[arg]++ = ',';
                    *args[arg]++ = ' ';
                    APPEND(args[arg], type);
                    *args[arg]++ = ' ';
                    *args[arg]++ = '#';
                    args[arg] += utoa(imm, args[arg], 10);
                }
            }
            else {
                // negative offset?
                APPEND(args[arg], d->U == B_UNSET ? "#-0x" : "#0x");
                args[arg] += utoa(d->imm, args[arg], 16);
            }

            *args[arg]++ = ']';
            continue;

        case 'A':
            if(d->rotate != 0) {
                APPEND(args[arg], "ROR #");
                args[arg] += utoa(d->rotate, args[arg], 10);
            }
            continue;

        default:
            return -1;
        }

        if(ptrs[++idx] == NULL || idx == 3) return -1;
        off--;
    }

    *mnemonic = *args[0] = *args[1] = *args[2] = *args[3] = *shift = 0;

    char *instr = str->instr;
    APPEND(instr, str->mnemonic);

    for (int i = 0; i < 4 && args[i] != str->arg[i]; i++) {
        if(i != 0) *instr++ = ',';
        *instr++ = ' ';
        APPEND(instr, str->arg[i]);
    }

    if(shift != str->shift) {
        *instr++ = ',';
        *instr++ = ' ';
        APPEND(instr, str->shift);
    }

    *instr = 0;
    return 0;
}

int darm_str2(const darm_t *d, darm_str_t *str, int lowercase)
{
    if(darm_str(d, str) < 0) {
        return -1;
    }

    if(lowercase != 0) {
        // just lowercase the entire object, including null-bytes
        char *buf = (char *) str;
        for (uint32_t i = 0; i < sizeof(darm_str_t); i++) {
            buf[i] = tolower(buf[i]);
        }
    }
    return 0;
}
