#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../darm.h"

int main(int argc, char *argv[]) {
	struct stat st;
	uint16_t w;
	uint16_t w2;
	FILE *f;
	int pos;
	char linebuf[128];
	darm_str_t ds;

	(void)linebuf; // unused

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file.bin>\n", argv[0]);
		return -1;
	}

	if (stat(argv[1], &st) < 0) {
		perror("stat");
		return -1;
	}

	if ((f = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		return -1;
	}

	pos = 0;

	while(1) {
		darm_t d; int ret;

		if (fread(&w, sizeof(uint16_t), 1, f) < 1) {
			break;
		}  
		if (fread(&w2, sizeof(uint16_t), 1, f) < 1) {
			break;
		}  

        ret = darm_thumb2_disasm(&d, w, w2);

		if (ret == 2) {
			// 32-bit Thumb2 instruction
			darm_str(&d, &ds);
			printf("%06x: [T2][%04x%04x] %s %x %x %x\n", pos, w, w2, ds.total, d.Rd, d.Rn, d.Rm);
			pos+=4;
		} else if (ret == 1) {
			// 16-bit Thumb instruction
			darm_str(&d, &ds);
			printf("%06x: [%04x] %s\n", pos, w, ds.total);
			fseek(f, -2, SEEK_CUR); // rewind 2 bytes
			pos+=2;
		} else {
			printf("%06x: ERR\n", pos);
			pos+=4;
		}

	}

	return 0;
}
