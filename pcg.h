#ifndef pcg_guard
#define pcg_guard

#include "lexical.h"

typedef struct {
   int kind;   
   char name[12];
   int val;
   int level;
   int addr;
} symbol;

typedef struct instruction {
	int op; // opcode
	int r;	// register
	int l;	// L : lexicographical level or register
	int m;	// M : number, program address, data address, register
} instruction;

void pcg(TokenNode *token, int flag);
void factor(TokenNode *token);
void expression(TokenNode *token);
void term(TokenNode *token);

#endif