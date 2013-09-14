#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../darm.h"
#include "../thumb2.h"

#define SIGN_EXTEND32(v, len) (((int32_t)(v) << (32 - len)) >> (32 - len))

int main(int argc, char *argv[])
{
    int expand, extend, len;

    if(argc == 2) {
        sscanf(argv[1], "%i", &expand);
        printf("%i\n", thumb_expand_imm(expand));
    }

    if(argc == 3) {
        sscanf(argv[1], "%i", &extend);
        sscanf(argv[2], "%i", &len);
        printf("extend %i -> %i, %i\n",
            SIGN_EXTEND32(extend, len), extend, len);
    }
}
