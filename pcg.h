#ifndef pcg_guard
#define pcg_guard

#include "lexical.h"
#include "pm0vm.h"

typedef struct {
   int kind;   
   char name[12];
   int val;
   int level;
   int addr;
} symbol;

int getIndex(char *lexeme);
instruction **pcg(TokenNode *token, int flag);
void addInst(instruction **inst, int op, int r, int l, int m);
instruction factor(TokenNode **token, instruction **inst, int *flag);
void expression(TokenNode **token, instruction **inst, int *flag);
void term(TokenNode **token, instruction **inst, int *flag);

#endif