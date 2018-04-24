#include <stdlib.h>
#include <stdio.h>
#include "lexical.h"
#include "pcg.h"

void main(int argc, char **argv) {
	instruction **inst;
	char *filename = argv[1];
	int i;
	int flags = 0;

	while (++i < argc && argv[i][0] == '-') {
		if (argv[i][1] == 'l') flags |= 1;
		else if (argv[i][1] == 'a') flags |= 2;
		else if (argv[i][1] == 'v') flags |= 4;
	}

	printf("%d\n", flags);

	TokenNode *lexemes = lex(filename, flags&1);

	inst = pcg(lexemes, flags&2);

	for(i=0; i<MAX_STACK_HEIGHT; i++) {
		printf("%d %d %d %d", inst[i]->op, inst[i]->r, inst[i]->l, inst[i]->m);
		if (inst[i]->op == SIO && inst[i]->m == 3) break;
	}
}