#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include <string.h>
#include "../darm.h"
#include "../thumb2.h"
#include <ctype.h>
#include <stdint.h>


#define SIGN_EXTEND32(v, len) ((v << (32 - len)) >> (32 - len))


int main(int argc, char *argv[]) {


   uint16_t expand;
   int extend;
   int len;

   if (argc == 2) {
     sscanf(argv[1], "%i", &expand);

     printf("%i\n", thumb_expand_imm(expand));
  }

  if (argc == 3) {
     sscanf(argv[1], "%i", &extend);
     sscanf(argv[2], "%i", &len);
     printf("extend %i -> %i, %i\n", SIGN_EXTEND32(extend,len), extend, len);
  }

}
