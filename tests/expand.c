#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include <string.h>
#include "../darm.h"
#include "../thumb2.h"
#include <ctype.h>
#include <stdint.h>


int main(int argc, char *argv[]) {


   uint16_t expand;

   if (argc > 1) {
     sscanf(argv[1], "%i", &expand);

     printf("%i\n", thumb_expand_imm(expand));
  }

}
