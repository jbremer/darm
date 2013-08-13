#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../darm.h"

int main(int argc, char *argv[]) {
	struct stat st;
	uint32_t w;
	FILE *f;
	int pos;
	char linebuf[128];
	darm_str_t ds;

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

		if (fread(&w, sizeof(uint32_t), 1, f) < 1) {
			break;
		}  

        ret = darm_armv7_disasm(&d, w);

		if (ret != 0) {
			printf("%06x: ERR\n", pos);
			pos+=4;
			continue;
		}

		darm_str(&d, &ds);

		printf(
			"%06x: [%08x] %s\n", 
			pos, w, ds.total
		);

		pos+=4;
	}

	return 0;
}
