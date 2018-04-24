/*
* COP3402 - Spring 2018
* System Software Assignment 2
* Submitted by: Suraj Singireddy (su365398), Gavin Knopp (ga803888)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical.h"

#define MAX_SIZE 5000
#define MAX_IDENT 11
#define INTERNAL 0
#define SYMBOLIC 1

char *getSym(token_type t) {
	switch (t) {
		case nulsym: return "nulsym";
		case identsym: return "identsym";
		case numbersym: return "numbersym";
		case plussym: return "plussym";
		case minussym: return "minussym";
		case multsym: return "multsym";
		case slashsym: return "slashsym";
		case oddsym: return "oddsym";
		case eqsym: return "eqsym";
		case neqsym: return "neqsym";
		case lessym: return "lessym";
		case leqsym: return "leqsym";
		case gtrsym: return "gtrsym";
		case geqsym: return "geqsym";
		case lparentsym: return "lparentsym";
		case rparentsym: return "rparentsym";
		case commasym: return "commasym";
		case semicolonsym: return "semicolonsym";
		case periodsym: return "periodsym";
		case becomessym: return "becomessym";
		case beginsym: return "beginsym";
		case endsym: return "endsym";
		case ifsym: return "ifsym";
		case thensym: return "thensym";
		case whilesym: return "whilesym";
		case dosym: return "dosym";
		case callsym: return "callsym";
		case constsym: return "constsym";
		case varsym: return "varsym";
		case procsym: return "procsym";
		case writesym: return "writesym";
		case readsym: return "readsym";
		case elsesym: return "elsesym";
	}
}

// start linked list functions

TokenNode *createNode(Token *toke) {
	TokenNode *node = (TokenNode *)malloc(sizeof(TokenNode));
	
	node->next = NULL;
	if(toke == NULL)
		node->toke = NULL;
	else
		node->toke = toke;

	return node;
}

// Adds node to tail, I dont think we need to add any to the head
TokenNode *addNode(TokenNode *head, Token *toke) {
	TokenNode *temp = head;

	if(head == NULL){
		head = createNode(toke);
		return head;
	}

	while(temp->next != NULL)
		temp = temp->next;

	temp->next = createNode(toke);
	return head;
}

void printList(TokenNode *head, int repr) {
	TokenNode *temp = head;

	if (repr) printf("Symbolic Representation:\n");
	else printf("Internal Representation:\n");

	while(temp != NULL) {
		if (repr) printf("%s ", getSym(temp->toke->type));
		else printf("%d ", temp->toke->type);
		if(temp->toke->type == identsym || temp->toke->type == numbersym) printf("%s ", temp->toke->lexeme);
		printf(" ");
		temp = temp->next;
	}
	printf("\n\n");
}

// end linked list functions

int readFile(char *filename, char *contents) {
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("File not found.");
		exit(1);
	}

	int i = 0;
	char c;

	while (!feof(file)) {
		c = fgetc(file);
		if (c == '\t') continue;
		contents[i++] = c;
	}
	contents[i] = '\0';
	fclose(file);
	return i;
}

int isLetter(char c) {
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

int isDigit(char c) {
	return ((c >= '0') && (c <= '9'));
}

void error(int e) {
	printf("Error: ");
	switch(e) {
		case 1:
			printf("Variable that does not start with letter.");
			break;
		case 2:
			printf("Number too long.");
			break;
		case 3:
			printf("Name too long.");
			break;
		case 4:
			printf("Invalid symbol.");
			break;
	}
	exit(e);
}

Token *analyzeOther(int *counter, char *contents, Token *toke) {
	toke->lexeme = (char *)malloc(2*sizeof(char));
	switch (contents[*counter]) {
		case '+':
			toke->type = plussym;
			strcpy(toke->lexeme, "+");
			return toke;
		case '-':
			toke->type = minussym;
			strcpy(toke->lexeme, "-");
			return toke;
		case '*':
			toke->type = multsym;
			strcpy(toke->lexeme, "*");
			return toke;
		case '/':
			toke->type = slashsym;
			strcpy(toke->lexeme, "/");
			return toke;
		case '=':
			toke->type = eqsym;
			strcpy(toke->lexeme, "=");
			return toke;
		case '<':
			toke->type = lessym;
			free(toke->lexeme);
			return toke;
		case '>':
			toke->type = gtrsym;
			free(toke->lexeme);
			return toke;
		case '(':
			toke->type = lparentsym;
			strcpy(toke->lexeme, "(");
			return toke;
		case ')':
			toke->type = rparentsym;
			strcpy(toke->lexeme, ")");
			return toke;
		case ',':
			toke->type = commasym;
			strcpy(toke->lexeme, ",");
			return toke;
		case ';':
			toke->type = semicolonsym;
			strcpy(toke->lexeme, ";");
			return toke;
		case '.':
			toke->type = periodsym;
			strcpy(toke->lexeme, ".");
			return toke;
		case ':':
			toke->type = becomessym;
			free(toke->lexeme);
			toke->lexeme = malloc(3*sizeof(char));
			strcpy(toke->lexeme, ":=");
			*counter += 1;
			return toke;
		case '#':
			return NULL;	// dont know what to do here but passes test
		default:
			// printf("ASCII: %i\n", contents[*counter]); // This line was so useful
			error(4);
	}

	return toke;
}

Token *analyzeNumber(int *counter, char *contents, Token *toke) {
	int i;
	
	for(i=1; i<5; i++) {
		if (!isDigit(contents[*counter+i])) {
			// if its a letter it should throw an error, most likely error(1)
			if(isLetter(contents[*counter+i]))
				error(1);
			break;
		}
	}
	if (isDigit(contents[*counter+i])) error(2);

	toke->lexeme = (char *)malloc((i+1)*sizeof(char));
	strncpy(toke->lexeme, contents+*counter, i);
	toke->lexeme[i] = '\0';
	*counter += i-1;
	return toke;
}

Token *analyzeIdentifier(int *counter, char *contents, Token *toke) {
	char word[MAX_IDENT+2];
	int i;

	for(i=0; i<12; i++) {
		word[i] = contents[*counter+i];
		word[i+1] = '\0';
		if(!isDigit(word[i]) && !isLetter(word[i])){
			word[i] = '\0';
			toke->lexeme = (char *)malloc((strlen(word)+1)*sizeof(char));
			strcpy(toke->lexeme, word);
			*counter += i-1;
			break;
		} else if(i == 11) {
			error(3);
		}
	}

	if (strcmp(word, "if") == 0) {
		toke->lexeme = (char *)malloc(3*sizeof(char));
		strcpy(toke->lexeme, "if");
		toke->type = ifsym; 
		return toke;
	} else if (strcmp(word, "do") == 0) {
		toke->lexeme = (char *)malloc(3*sizeof(char));
		strcpy(toke->lexeme, "do");
		toke->type = dosym;
		return toke;
	} else if (strcmp(word, "var") == 0) {
		toke->lexeme = (char *)malloc(4*sizeof(char));
		strcpy(toke->lexeme, "var");
		toke->type = varsym;
		return toke;
	} else if (strcmp(word, "end") == 0) {
		toke->lexeme = (char *)malloc(4*sizeof(char));
		strcpy(toke->lexeme, "end");
		toke->type = endsym;
		return toke;
	} else if (strcmp(word, "call") == 0) {
		toke->lexeme = (char *)malloc(5*sizeof(char));
		strcpy(toke->lexeme, "call");
		toke->type = callsym;
		return toke;
	} else if (strcmp(word, "then") == 0) {
		toke->lexeme = (char *)malloc(5*sizeof(char));
		strcpy(toke->lexeme, "then");
		toke->type = thensym;
		return toke;
	} else if (strcmp(word, "else") == 0) {
		toke->lexeme = (char *)malloc(5*sizeof(char));
		strcpy(toke->lexeme, "else");
		toke->type = elsesym;
		return toke;
	} else if (strcmp(word, "read") == 0) {
		toke->lexeme = (char *)malloc(5*sizeof(char));
		strcpy(toke->lexeme, "read");
		toke->type = readsym;
		return toke;
	} else if (strcmp(word, "const") == 0) {
		toke->lexeme = (char *)malloc(6*sizeof(char));
		strcpy(toke->lexeme, "const");
		toke->type = constsym;
		return toke;
	} else if (strcmp(word, "begin") == 0) {
		toke->lexeme = (char *)malloc(6*sizeof(char));
		strcpy(toke->lexeme, "begin");
		toke->type = beginsym;
		return toke;
	} else if (strcmp(word, "while") == 0) {
		toke->lexeme = (char *)malloc(6*sizeof(char));
		strcpy(toke->lexeme, "while");
		toke->type = whilesym;
		return toke;
	} else if (strcmp(word, "write") == 0) {
		toke->lexeme = (char *)malloc(6*sizeof(char));
		strcpy(toke->lexeme, "write");
		toke->type = writesym;
		return toke;
	} else if (strcmp(word, "procedure") == 0) {
		toke->lexeme = (char *)malloc(10*sizeof(char));
		strcpy(toke->lexeme, "procedure");
		toke->type = procsym;
		return toke;
	}

	if(!isDigit(word[i]) && !isLetter(word[i])){
		word[i] = '\0';
		toke->lexeme = (char *)malloc(strlen(word)*sizeof(char));
		strcpy(toke->lexeme, word);
		return toke;
	}

	return toke;
}

int checkOther(char c) {
	return (c == '\n') || (c == ' ') || (c == 13) || (c == -1) || (c == '#');
}


Token *nextToken(int *counter, char *contents) {
	Token *toke = (Token *)malloc(sizeof(Token));
	token_type t;
	char word[MAX_IDENT+1];
	int i;

	if(checkOther(contents[*counter]))
		return NULL;
	if (isLetter(contents[*counter])) {
		toke->type = identsym;
	} else if (isDigit(contents[*counter])) {
		toke->type = numbersym;
	} else {
		toke = analyzeOther(counter, contents, toke);
		if(toke == NULL)
			return toke;
	}

	switch (toke->type) {
		case numbersym:
			toke = analyzeNumber(counter, contents, toke);
			break;

		case identsym:
			toke = analyzeIdentifier(counter, contents, toke);
			break;

		
		case slashsym:
			if (contents[*counter+1] == '*') {
				while (1) {
					while (contents[(*counter)++] != '*');
					if (contents[(*counter)++] == '/') break;
				}
				return NULL;
			} else {
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, "/\0");
			}
			break;
		

		case lessym:
			if (contents[++*counter] == '>') {
				toke->type = neqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, "<>\0");
			} else if (contents[*counter] == '=') {
				toke->type = leqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, "<=\0");
			} else {
				*counter--;
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, "<\0");
			}
			break;

		case gtrsym:
			if (contents[++*counter] == '=') {
				toke->type = geqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, ">=\0");
			} else {
				*counter--;
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, ">\0");
			}
			break;
	}

	return toke;
}

TokenNode *analyze(int fileSize, char *contents, int output) {
	int c = 0;
	int i, j = 0;
	token_type *tokenTypes = (token_type *)malloc(fileSize*sizeof(token_type));
	char **tokens = (char **)malloc(fileSize*sizeof(char*));
	Token *toke = NULL;
	TokenNode *head = NULL;
	
	for(i=0; i<fileSize; i++) {
		toke = nextToken(&i, contents);
		if(toke != NULL) head = addNode(head, toke);
	}

	if (output == 0) return head;

	printf("-------------------------------------------\n");
	printf("LIST OF LEXEMES/TOKENS:\n\n");

	printList(head, INTERNAL);
	printList(head, SYMBOLIC);

	return head;
}

TokenNode *lex(char *filename, int output) {
	char contents[MAX_SIZE] = {};
	int fileSize = readFile(filename, contents);
	TokenNode *head = analyze(fileSize, contents, output);
	return head;
}