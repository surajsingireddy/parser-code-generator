#include <stdio.h>
#include <stdlib.h>
#include "pcg.h"
#include "lexical.h"

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
			printf("then  expected."); 
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
		case lessym: return 1;
		case gtrsym: return 1;
		case leqsym: return 1;
		case geqsym: return 1;
		case eqsym: return 1;
		case neqsym: return 1;
		default: return 0;
	}
}

void factor(TokenNode *token) {
	if (type(token, identsym) || type(token, numbersym)) next(&token);
	else if (type(token, lparentsym)) {
		next(&token);
		expression(token);
		if (!type(token, rparentsym)) err(0);
		next(&token);
	} else err(0);
}

void term(TokenNode *token) {
	factor(token);
	while (type(token, multsym) || type(token, slashsym)) {
		next(&token);
		factor(token);
	}
}

void expression(TokenNode *token) {
	if (type(token, plussym) || type(token, minussym)) next(&token);
	term(token);
	while (type(token, plussym) || type(token, minussym)) {
		next(&token);
		term(token);
	}
}

void condition(TokenNode *token) {
	if (type(token, oddsym)) {
		next(&token);
		expression(token);
	} else {
		expression(token);
		if (!isRelation(token)) err(0);
		next(&token);
		expression(token);
	}
}

void statement(TokenNode *token) {
	if (type(token, identsym)) {
		if (!nexttype(token, becomessym)) err(0);
		next(&token);
		expression(token);
	} else if (type(token, callsym)) {
		if (!nexttype(token, identsym)) err(0);
		next(&token);
	} else if (type(token, beginsym)) {
		next(&token);
		statement(token);
		while (type(token, semicolonsym)) {
			next(&token);
			statement(token);
		}

		if (!type(token, endsym)) err(0);

		next(&token);
	} else if (type(token, ifsym)) {
		next(&token);
		condition(token);
		if (!type(token, thensym)) err(0);
		next(&token);
		statement(token);
	} else if (type(token, whilesym)) {
		next(&token);
		condition(token);
		if (!type(token, dosym)) err(0);
		next(&token);
		statement(token);
	}
}

void block(TokenNode *token) {
	if (type(token, constsym)) {
		do {
			if (!nexttype(token, identsym)) err(0);
			if (!nexttype(token, eqsym)) err(0);
			if (!nexttype(token, numbersym)) err(0);
		} while (nexttype(token, commasym));

		if (!type(token, semicolonsym)) err(0);

		next(&token);
	}

	if (type(token, numbersym)) {
		do {
			if (!nexttype(token, identsym)) err(0);
		} while (nexttype(token, commasym));

		if (!type(token, semicolonsym)) err(0);

		next(&token);
	}

	while (type(token, procsym)) {
		if (!nexttype(token, identsym)) err(0);
		if (!nexttype(token, semicolonsym)) err(0);
		next(&token);
		block(token);
		if (!nexttype(token, semicolonsym)) err(0);
		next(&token);
	}

	statement(token);
}

void program(TokenNode *token) {
	block(token);
	if (type(token, periodsym)) err(9);
}

void pcg(TokenNode *token, int flag) {
	program(token);
}