#ifndef pm0vm_guard
#define pm0vm_guard

// definitions
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define INSTRUCTION_REGISTERS 8

typedef enum {
	LIT = 1, RTN, LOD, STO, CAL, INC, JMP, JPC, SIO, NEG, ADD, 
	SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
} opcode;

typedef struct instruction {
	int op; // opcode
	int r;	// register
	int l;	// L : lexicographical level or register
	int m;	// M : number, program address, data address, register
} instruction;

void vm(instruction **inst, int flag);

#endif