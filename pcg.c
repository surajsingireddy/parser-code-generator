#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcg.h"
#include "lexical.h"
#include "pm0vm.h"

#define MAX_SYMBOL_TABLE_SIZE (2<<6)-1

symbol *symbol_table[MAX_SYMBOL_TABLE_SIZE];
int s = 0, idx = 0;

void err(int e) {
	switch (e) {
		case 0:
			printf("BOI");
			exit(e);
		case 1:  
			printf("Use = instead of :=."); 
			exit(e);
		case 2:  
			printf("= must be followed by a number."); 
			exit(e);
		case 3:  
			printf("Identifier must be followed by =."); 
			exit(e);
		case 4:  
			printf("const, var, procedure must be followed by identifier."); 
			exit(e);
		case 5:  
			printf("Semicolon or comma missing."); 
			exit(e);
		case 6:  
			printf("Incorrect symbol after procedure declaration."); 
			exit(e);
		case 7:  
			printf("Statement expected."); 
			exit(e);
		case 8:  
			printf("Incorrect symbol after statement part in block."); 
			exit(e);
		case 9:  
			printf("Period expected."); 
			exit(e);
		case 10:  
			printf("Semicolon between statements missing."); 
			exit(e); 
		case 11:  
			printf("Undeclared identifier."); 
			exit(e); 
		case 12:  
			printf("Assignment to constant or procedure is not allowed."); 
			exit(e); 
		case 13:  
			printf("Assignment operator expected."); 
			exit(e); 
		case 14:  
			printf("call must be followed by an identifier."); 
			exit(e); 
		case 15:  
			printf("Call of a constant or variable is meaningless."); 
			exit(e); 
		case 16:  
			printf("then expected."); 
			exit(e); 
		case 17:  
			printf("Semicolon or } expected."); 
			exit(e); 
		case 18:  
			printf("do expected."); 
			exit(e); 
		case 19:  
			printf("Incorrect symbol following statement."); 
			exit(e);
		case 20:  
			printf("Relational operator expected."); 
			exit(e); 
		case 21:  
			printf("Expression must not contain a procedure identifier."); 
			exit(e); 
		case 22:  
			printf("Right parenthesis missing."); 
			exit(e); 
		case 23:  
			printf("The preceding factor cannot begin with this symbol."); 
			exit(e); 
		case 24:  
			printf("An expression cannot begin with this symbol."); 
			exit(e); 
		case 25:  
			printf("This number is too large."); 
			exit(e); 
		case 26:
			printf("end expected.");
			exit(e);
	}
}

void next(TokenNode **token) {
	*token = (*token)->next;
	if ((*token) == NULL) err(0);
	else if ((*token)->toke == NULL) err(0);
}

int type(TokenNode *token, token_type t) {
	return (t == token->toke->type) ? 1 : 0;
}

int nexttype(TokenNode **token, token_type t) {
	next(token);
	return type(*token, t);
}

int isRelation(TokenNode *token) {
	switch (token->toke->type) {
		case lessym: return LSS;
		case gtrsym: return GTR;
		case leqsym: return LEQ;
		case geqsym: return GEQ;
		case eqsym: return EQL;
		case neqsym: return NEQ;
		default: return 0;
	}
}

instruction factor(TokenNode **token, instruction **inst, int *flag) {
	instruction in;
	if (type(*token, identsym)) {
		in = (instruction) {LOD, (*flag), 0, symbol_table[getIndex((*token)->toke->lexeme)]->addr};
		next(token);
		return in;
	} else if (type(*token, numbersym)) {
		in = (instruction) {LIT, (*flag), 0, atoi((*token)->toke->lexeme)};
		next(token);
		return in;
	} else if (type(*token, lparentsym)) {
		next(token);
		expression(token, inst, flag);
		if (!type(*token, rparentsym)) err(22);
		next(token);
		return (instruction) {0, 0, 0, 0};
	} else err(24);
}

void term(TokenNode **token, instruction **inst, int *flag) {
	instruction in = factor(token, inst, flag);
	instruction in2;

	if (!(type(*token, multsym) || type(*token, slashsym))) {
		addInst(inst, in.op, in.r, in.l, in.m);
		(*flag) = 1;
		return;
	}

	while (type(*token, multsym) || type(*token, slashsym)) {
		if (type(*token, multsym)) {
			next(token);
			in2 = factor(token, inst, flag);
			addInst(inst, in.op, *flag, in.l, in.m);
			(*flag) = 1;
			addInst(inst, in2.op, *flag, in2.l, in2.m);
			addInst(inst, MUL, 0, 0, 1);
		} else {
			next(token);
			in2 = factor(token, inst, flag);
			addInst(inst, in.op, (*flag), in.l, in.m);
			(*flag) = 1;
			addInst(inst, in2.op, (*flag), in2.l, in2.m);
			addInst(inst, DIV, 0, 0, 1);
		}
	}
}

void expression(TokenNode **token, instruction **inst, int *flag) {
	int neg = 0;
	if (type(*token, plussym)) {
		next(token);
	} else if (type(*token, minussym)) {
		neg = 1;
		next(token);
	}

	term(token, inst, flag);

	if (neg == 1) {
		addInst(inst, NEG, 0, 0, 0);
	}

	while (type(*token, plussym) || type(*token, minussym)) {
		if (type(*token, plussym)) {
			next(token);
			term(token, inst, flag);
			addInst(inst, ADD, 0, 0, 1);
		} else {
			next(token);
			term(token, inst, flag);
			addInst(inst, SUB, 0, 0, 1);
		}
	}
}

void expressionWrapper(TokenNode **token, instruction **inst) {
	int flag = 0;
	expression(token, inst, &flag);
}

void condition(TokenNode **token, instruction **inst) {
	int rel;
	if (type(*token, oddsym)) {
		next(token);
		expressionWrapper(token, inst);
		addInst(inst, ODD, 0, 0, 0);
	} else {
		expressionWrapper(token, inst);
		addInst(inst, LIT, 2, 0, 0);
		addInst(inst, ADD, 2, 0, 2);
		rel = isRelation(*token);
		if (rel == 0) err(20);
		next(token);
		expressionWrapper(token, inst);
		addInst(inst, rel, 0, 2, 0);
	}
}

void statement(TokenNode **token, instruction **inst) {
	int ifpc, thenpc, i;
	if (type(*token, identsym)) {
		i = getIndex((*token)->toke->lexeme);
		if (i == -1) err(11);
		if (!nexttype(token, becomessym)) err(3);
		next(token);
		expressionWrapper(token, inst);
		addInst(inst, STO, 0, 0, symbol_table[i]->addr);
	} else if (type(*token, callsym)) {
		if (!nexttype(token, identsym)) err(14);
		next(token);
	} else if (type(*token, beginsym)) {
		do {
			next(token);
			statement(token, inst);
		} while (type(*token, semicolonsym));

		if (!type(*token, endsym)) err(26);

		next(token);
	} else if (type(*token, ifsym)) {
		next(token);
		condition(token, inst);
		thenpc = idx;
		addInst(inst, JPC, 0, 0, 0);
		if (!type(*token, thensym)) err(16);
		next(token);
		statement(token, inst);
		inst[thenpc]->m = idx;
	} else if (type(*token, whilesym)) {
		next(token);
		ifpc = idx;
		condition(token, inst);
		thenpc = idx;
		addInst(inst, JPC, 0, 0, 0);
		if (!type(*token, dosym)) err(18);
		next(token);
		statement(token, inst);
		addInst(inst, JMP, 0, 0, ifpc);
		inst[thenpc]->m = idx;
	} else if (type(*token, readsym)) {
		addInst(inst, SIO, 0, 0, 2);
		next(token);
	} else if (type(*token, writesym)) {
		next(token);
		addInst(inst, LOD, 0, 0, symbol_table[getIndex((*token)->toke->lexeme)]->addr);
		addInst(inst, SIO, 0, 0, 1);
		next(token);
	}
}

void block(TokenNode **token, instruction **inst) {
	int i;
	if (type(*token, constsym)) {
		do {
			symbol_table[s] = (symbol *)malloc(sizeof(symbol));
			if (!nexttype(token, identsym)) err(4);
			strcpy(symbol_table[s]->name, (*token)->toke->lexeme);
			if (!nexttype(token, eqsym)) err(3);
			if (!nexttype(token, numbersym)) err(2);
			symbol_table[s]->val = atoi((*token)->toke->lexeme);
			symbol_table[s]->kind = 1;
			symbol_table[s]->level = 0;
			symbol_table[s++]->addr = s+4;
		} while (nexttype(token, commasym));

		if (!type(*token, semicolonsym)) err(5);

		next(token);
	}

	if (type(*token, varsym)) {
		do {
			if (!nexttype(token, identsym)) err(4);
			symbol_table[s] = (symbol *)malloc(sizeof(symbol));
			strcpy(symbol_table[s]->name, (*token)->toke->lexeme);
			symbol_table[s]->val = 0;
			symbol_table[s]->kind = 2;
			symbol_table[s]->level = 0;
			symbol_table[s++]->addr = s+4;
		} while (nexttype(token, commasym));

		if (!type(*token, semicolonsym)) err(5);

		next(token);
	}

	while (type(*token, procsym)) {
		if (!nexttype(token, identsym)) err(4);
		if (!nexttype(token, semicolonsym)) err(5);
		next(token);
		block(token, inst);
		if (!nexttype(token, semicolonsym)) err(5);
		next(token);
	}

	addInst(inst, INC, 0, 0, s+4);

	for (i=0; i<s; i++) {
		if (symbol_table[i]->kind == 1) {
			addInst(inst, LIT, 0, 0, symbol_table[i]->val);
			addInst(inst, STO, 0, 0, symbol_table[i]->addr);
		}
	}

	statement(token, inst);
}

void addInst(instruction **inst, int op, int r, int l, int m) {
	// printf("%d %d %d %d %d\n", idx, op, r, l, m);
	inst[idx] = (instruction *)malloc(sizeof(instruction));
	inst[idx]->op = op;
	inst[idx]->r = r;
	inst[idx]->l = l;
	inst[idx++]->m = m;
	// printf("%d %d %d %d %d\n", idx-1, inst[idx-1]->op, inst[idx-1]->r, inst[idx-1]->l, inst[idx-1]->m);
}

int getIndex(char *lexeme) {
	int i;
	for(i=0; i<s; i++) {
		if (strcmp(lexeme, symbol_table[i]->name) == 0) return i;
	}

	err(0);
}

void program(TokenNode **token, instruction **inst) {
	block(token, inst);
	if (!type(*token, periodsym)) err(9);
	addInst(inst, SIO, 0, 0, 3);
}

instruction **pcg(TokenNode *token, int flag) {
	int i;
	instruction **inst = (instruction **)malloc(sizeof(instruction *) * MAX_STACK_HEIGHT);
	program(&token, inst);

	printf("Code is syntactically correct. Assembly code generated successfully.\n");

	if (flag == 0) return inst;

	printf("-------------------------------------------\n");
	printf("GENERATED INTERMEDIATE CODE:\n");

	for(i=0; i<MAX_STACK_HEIGHT; i++) {
		printf("%d %d %d %d %d\n", i, inst[i]->op, inst[i]->r, inst[i]->l, inst[i]->m);
		if (inst[i]->op == SIO && inst[i]->m == 3) break;
	}

	return inst;
}