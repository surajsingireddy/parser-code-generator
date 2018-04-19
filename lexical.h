#ifndef guard
#define guard

#include <stdbool.h>

typedef enum {
	nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, 
	oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, 
	rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym,
	ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, 
	writesym, readsym, elsesym
} token_type;

typedef struct Token {
	token_type type;
	char *lexeme;
} Token;

typedef struct TokenNode {
	Token *toke;
	struct TokenNode *next;
} TokenNode;

char *getSym(token_type t);
TokenNode *createNode(Token *toke);
TokenNode *addNode(TokenNode *head, Token *toke);
void printList(TokenNode *head, bool internal);
int readFile(char *filename, char *contents);
int isLetter(char c);
int isDigit(char c);
void error(int e);
Token *analyzeOther(int *counter, char *contents, Token *toke);
Token *analyzeNumber(int *counter, char *contents, Token *toke);
Token *analyzeIdentifier(int *counter, char *contents, Token *toke);
int checkOther(char c);
Token *nextToken(int *counter, char *contents);
TokenNode *analyze(int fileSize, char *contents);
int lex(char* filename, bool output);

#endif