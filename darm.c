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
    if(d->instr == I_INVLD || d->instr >= ARRAYSIZE(armv7_mnemonics)) {
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
    APPEND(mnemonic, armv7_mnemonic_by_index(d->instr));

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
            APPEND(mnemonic, armv7_condition_info(d->cond, NULL, NULL, 1));
            continue;

        case 'd':
            if(d->Rd == R_INVLD) break;
            APPEND(args[arg], armv7_register_by_index(d->Rd));
            arg++;
            continue;

        case 'n':
            if(d->Rn == R_INVLD) break;
            APPEND(args[arg], armv7_register_by_index(d->Rn));
            arg++;
            continue;

        case 'm':
            if(d->Rm == R_INVLD) break;
            APPEND(args[arg], armv7_register_by_index(d->Rm));
            arg++;
            continue;

        case 'a':
            if(d->Ra == R_INVLD) break;
            APPEND(args[arg], armv7_register_by_index(d->Ra));
            arg++;
            continue;

        case 't':
            if(d->Rt == R_INVLD) break;
            APPEND(args[arg], armv7_register_by_index(d->Rt));
            arg++;
            continue;

        case 'i':
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
            if(d->shift_is_reg == 0) {
                const char *type; uint32_t imm;
                if(armv7_immshift_decode(d, &type, &imm) == 0) {
                    APPEND(shift, type);
                    *shift++ = ' ';
                    *shift++ = '#';
                    shift += utoa(imm, shift, 10);
                }
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
