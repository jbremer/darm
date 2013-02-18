#include <stdio.h>
#include "darm.h"

int main(int argc, char *argv[])
{
    int ret; darm_t d;

    uint32_t w = 0xe0a10082;

    ret = armv7_disassemble(&d, w);

    printf("ret: %d, d->instr: %d, Rd: %d, Rn: %d, Rm: %d\n",
        ret, d.instr, d.Rd, d.Rn, d.Rm);
}
