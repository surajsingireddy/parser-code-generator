#ifndef guard
#define guard

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

instruction **pcg(lexemes, flags&2)

#endif