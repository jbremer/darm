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
#include <string.h>
#include <stdint.h>
#include "darm.h"

#define APPEND(out, ptr) \
    do { \
        const char *s = ptr; \
        char *d = out; \
        if(s != NULL) while (*s != 0) *d++ = *s++; \
    } while (0);


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
            APPEND(args[arg++], armv7_register_by_index(d->Rd));
            continue;

        case 'n':
            if(d->Rn == R_INVLD) break;
            APPEND(args[arg++], armv7_register_by_index(d->Rn));
            arg++;
            continue;

        case 'm':
            if(d->Rm == R_INVLD) break;
            APPEND(args[arg++], armv7_register_by_index(d->Rm));
            arg++;
            continue;

        case 'a':
            if(d->Ra == R_INVLD) break;
            APPEND(args[arg++], armv7_register_by_index(d->Ra));
            arg++;
            continue;

        case 't':
            if(d->Rt == R_INVLD) break;
            APPEND(args[arg++], armv7_register_by_index(d->Rt));
            arg++;
            continue;

        default:
            return -1;
        }

        if(ptrs[++idx] == NULL || idx == 3) return -1;
        off--;
    }

    *mnemonic = *args[0] = *args[1] = *args[2] = *args[3] = 0;

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
