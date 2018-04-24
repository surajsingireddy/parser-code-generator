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

int nexttype(TokenNode *token, token_type t) {
	next(&token);
	return type(token, t);
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

void factor(TokenNode *token, instruction **inst) {
	int i;
	if (type(token, identsym)) {
		addInst(inst, LOD, 2, 0, symbol_table[getIndex(token->toke->lexeme)]->addr);
		next(&token);
	} else if (type(token, numbersym)) {
		addInst(inst, LIT, 2, 0, atoi(token->toke->lexeme));
		next(&token);
	} else if (type(token, lparentsym)) {
		next(&token);
		expression(token, inst);
		addInst(inst, LIT, 2, 0, 0);
		addInst(inst, ADD, 2, 0, 2);
		if (!type(token, rparentsym)) err(22);
		next(&token);
	} else err(24);
}

void term(TokenNode *token, instruction **inst) {
	factor(token, inst);
	addInst(inst, LIT, 1, 0, 0);
	addInst(inst, ADD, 1, 1, 2);
	while (type(token, multsym) || type(token, slashsym)) {
		if (type(token, multsym)) {
			next(&token);
			factor(token, inst);
			addInst(inst, MUL, 1, 1, 2);
		} else {
			next(&token);
			factor(token, inst);
			addInst(inst, DIV, 1, 1, 2);
}	}	}

void expression(TokenNode *token, instruction **inst) {
	int neg = 0;
	if (type(token, plussym)) {
		next(&token);
	} else if (type(token, minussym)) {
		neg = 1;
		next(&token);
	}

	term(token, inst);

	addInst(inst, LIT, 0, 0, 0);
	addInst(inst, ADD, 0, 0, 1);
	if (neg == 1) {
		addInst(inst, NEG, 0, 0, 0);
	}

	while (type(token, plussym) || type(token, minussym)) {
		if (type(token, plussym)) {
			next(&token);
			term(token, inst);
			addInst(inst, ADD, 0, 0, 1);
		} else {
			next(&token);
			term(token, inst);
			addInst(inst, SUB, 0, 0, 1);
}	}	}

void condition(TokenNode *token, instruction **inst) {
	int rel;
	if (type(token, oddsym)) {
		next(&token);
		expression(token, inst);
		addInst(inst, ODD, 0, 0, 0);
	} else {
		expression(token, inst);
		addInst(inst, LIT, 1, 0, 0);
		addInst(inst, ADD, 1, 0, 1);
		rel = isRelation(token);
		if (rel == 0) err(20);
		next(&token);
		expression(token, inst);
		addInst(inst, rel, 0, 0, 1);
	}
}

void statement(TokenNode *token, instruction **inst) {
	int ifpc, thenpc, i;
	if (type(token, identsym)) {
		i = getIndex(token->toke->lexeme);
		if (!nexttype(token, becomessym)) err(3);
		next(&token);
		expression(token, inst);
		addInst(inst, STO, 0, 0, symbol_table[i]->addr);
	} else if (type(token, callsym)) {
		if (!nexttype(token, identsym)) err(14);
		next(&token);
	} else if (type(token, beginsym)) {
		do {
			next(&token);
			statement(token, inst);
		} while (type(token, semicolonsym));

		if (!type(token, endsym)) err(0);

		next(&token);
	} else if (type(token, ifsym)) {
		next(&token);
		condition(token, inst);
		thenpc = idx;
		addInst(inst, JPC, 0, 0, 0);
		if (!type(token, thensym)) err(16);
		next(&token);
		statement(token, inst);
		inst[thenpc]->m = idx;
	} else if (type(token, whilesym)) {
		next(&token);
		ifpc = idx;
		condition(token, inst);
		thenpc = idx;
		addInst(inst, JPC, 0, 0, 0);
		if (!type(token, dosym)) err(18);
		next(&token);
		statement(token, inst);
		addInst(inst, JMP, 0, 0, ifpc);
		inst[thenpc]->m = idx;
	} else if (type(token, readsym)) {
		addInst(inst, SIO, 0, 0, 2);
		next(&token);
	} else if (type(token, writesym)) {
		addInst(inst, SIO, 0, 0, 1);
		next(&token);
	}
}

void block(TokenNode *token, instruction **inst) {
	int i;
	if (type(token, constsym)) {
		do {
			symbol_table[s] = (symbol *)malloc(sizeof(symbol));
			if (!nexttype(token, identsym)) err(0);
			strcpy(symbol_table[s]->name, token->toke->lexeme);
			if (!nexttype(token, eqsym)) err(0);
			if (!nexttype(token, numbersym)) err(0);
			symbol_table[s]->val = atoi(token->toke->lexeme);
			symbol_table[s]->kind = 1;
			symbol_table[s]->level = 0;
			symbol_table[s++]->addr = s+4;
		} while (nexttype(token, commasym));

		if (!type(token, semicolonsym)) err(0);

		next(&token);
	}

	if (type(token, varsym)) {
		do {
			if (!nexttype(token, identsym)) err(0);
			symbol_table[s] = (symbol *)malloc(sizeof(symbol));
			symbol_table[s]->val = 0;
			symbol_table[s]->kind = 2;
			symbol_table[s]->level = 0;
			symbol_table[s++]->addr = s+4;
		} while (nexttype(token, commasym));

		if (!type(token, semicolonsym)) err(0);

		next(&token);
	}

	while (type(token, procsym)) {
		if (!nexttype(token, identsym)) err(0);
		if (!nexttype(token, semicolonsym)) err(0);
		next(&token);
		block(token, inst);
		if (!nexttype(token, semicolonsym)) err(0);
		next(&token);
	}

	addInst(inst, 6, 0, 0, s+4);

	for (i=0; i<s; i++) {
		if (symbol_table[i]->kind == 1) {
			addInst(inst, 1, 0, 0, symbol_table[i]->val);
			addInst(inst, 4, 0, 0, symbol_table[i]->addr);
		}
	}

	statement(token, inst);
}

void addInst(instruction **inst, int op, int r, int l, int m) {
	inst[idx] = (instruction *)malloc(sizeof(instruction));
	inst[idx]->op = op;
	inst[idx]->r = r;
	inst[idx]->l = l;
	inst[idx++]->m = m;
}

int getIndex(char *lexeme) {
	int i;
	for(i=0; i<s; i++) {
		if (strcmp(lexeme, symbol_table[i]->name) == 0) return i;
	}

	return -1;
}

void program(TokenNode *token, instruction **inst) {
	block(token, inst);
	if (type(token, periodsym)) err(9);
	addInst(inst, SIO, 0, 0, 3);
}

instruction **pcg(TokenNode *token, int flag) {
	instruction *inst[MAX_STACK_HEIGHT] = {};
	program(token, inst);
	return inst;
}