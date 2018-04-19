#include <string.h>
#include "lexical.h"

void main(int argc, char **argv) {
	char *filename = argv[1];
	int i = 1;
	char flags[3] = {0, 0, 0}

	while (++i < argc && argv[i][0] == '-') {
		if (argv[i][1] == 'l') flags[0] = 1;
		else if (argv[i][1] == 'a') flags[1] = 1;
		else if (argv[i][1] == 'v') flags[2] = 1;
	}

	lex(filename, flags[0]);
}