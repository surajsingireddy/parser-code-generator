#include <stdlib.h>
#include <stdio.h>
#include "lexical.h"
#include "pcg.h"

void main(int argc, char **argv) {
	instruction **inst;
	char *filename = argv[1];
	int i = 1;
	int flags = 0;

	while (++i < argc && argv[i][0] == '-') {
		if (argv[i][1] == 'l') flags |= 1;
		else if (argv[i][1] == 'a') flags |= 2;
		else if (argv[i][1] == 'v') flags |= 4;
	}

	TokenNode *lexemes = lex(filename, flags&1);

	inst = pcg(lexemes, flags&2);

	vm(inst, flags&4);

	printf("\nFinished execution. Exiting...\n");
}