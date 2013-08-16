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


The idea of elfdarm is to be able to correctly disassemble any ARMv7 ELF
binary without caring too much about messed up ELF header files.

*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include "darm.h"
#include "elfdarm.h"

// TODO add an ignore switch
#define MAXFILESIZE (16 * 1024 * 1024)

// check whether an offset + size fall within the acceptable range
#define CHK(offset, size, x ) \
    if((offset) >= g_len || (size) >= g_len || \
            ((offset) + (size)) >= g_len) { \
        fprintf(stderr, "[-] Invalid offset for %s..\n", x); \
        return -1; \
    }

static const uint8_t *g_buf;
static uint32_t g_len;

static int parse_code_section(uint32_t vaddr, uint32_t offset, uint32_t size)
{
    // TODO improve this, big time :')

    const uint32_t *code = (const uint32_t *) &g_buf[offset];

    darm_t d; darm_str_t str;
    for (uint32_t idx = 0; idx < size / sizeof(uint32_t);
            idx++, vaddr += sizeof(uint32_t)) {

        if(darm_armv7_disasm(&d, code[idx]) < 0) {
            printf("#%06x %08x (..)\n", vaddr, code[idx]);
            continue;
        }

        if(darm_str2(&d, &str, 1) < 0) {
            printf("#%06x %08x (..)\n", vaddr, code[idx]);
            continue;
        }

        printf("#%06x %08x %s\n", vaddr, code[idx], str.total);
    }
    return 0;
}

static int parse_program_header(const elf32_pheader_t *phdr)
{
    // let's see if we're interested in this section - is it executable?
    if((phdr->p_flags & PF_X) == 0) return 0;

    CHK(phdr->p_offset, phdr->p_filesz, "Code Section");

    printf("offset: 0x%08x, filesz: 0x%08x, vaddr: 0x%08x\n",
        phdr->p_offset, phdr->p_filesz, phdr->p_vaddr);

    parse_code_section(phdr->p_vaddr, phdr->p_offset, phdr->p_filesz);
    return 0;
}

static int parse_elf_header(const uint8_t *buf)
{
    CHK(0, sizeof(elf32_header_t), "ELF Header");
    elf32_header_t *hdr = (elf32_header_t *) buf;

    uint32_t ph_off = hdr->e_phoff;
    for (uint32_t idx = 0; idx < hdr->e_phnum;
            idx++, ph_off += sizeof(elf32_pheader_t)) {
        CHK(ph_off, sizeof(elf32_pheader_t), "ELF Program Header");

        parse_program_header((elf32_pheader_t *) &buf[ph_off]);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr,
            "elfdarm - Utility for dumping ARMv7 ELF files   "
                                        "(C) Jurriaan Bremer, 2013\n"
            "\n"
            "Usage: %s <binfile>\n", argv[0]
        );
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL) {
        fprintf(stderr, "[-] Error opening input file!\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    g_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if(g_len == 0) {
        fprintf(stderr, "[-] Empty input file!\n");
        goto error;
    }

    if(g_len > MAXFILESIZE) {
        fprintf(stderr, "[-] File too big!\n");
        fprintf(stderr, "[x] Add -f to force loading anyway.\n");
        goto error;
    }

    uint8_t *buf = malloc(g_len);
    if(buf == NULL) {
        fprintf(stderr, "[-] Error allocating memory!\n");
        goto error;
    }

    fread(buf, 1, g_len, fp);
    fclose(fp);

    parse_elf_header(g_buf = buf);

error:
    fclose(fp);
    return 1;
}
