#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

int column = 1;
int line = 1;

void addToken(TokenList *tokenList, enum Token_t kind, union TokenValue value, bool ownstr) {
	Token token = { .kind = kind, .value = value, .line = line, .column = column, .ownstr = ownstr };
	
	if (tokenList->indexToken >= tokenList->size) {
		Token *tmp = NULL;
		tokenList->size *= 2;
		tmp = realloc(tokenList->tokens, tokenList->size * sizeof(Token));
		if (tmp == NULL) {
				freeTokens(tokenList);
				perror("Error realloc\n");
				exit(1);
		}
		tokenList->tokens = tmp;
	}
	tokenList->tokens[tokenList->indexToken] = token; /* tester s'il reste de la place dans la liste */
	(tokenList->indexToken)++;
}

// dispatch table
bool canAddKeywordToken(TokenList *tokenList, union TokenValue value)
{
	if (strcmp(value.value_str, "mov") == 0) {
		addToken(tokenList, MN_MOV, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "add") == 0) {
		addToken(tokenList, MN_ADD, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "sub") == 0) {
		addToken(tokenList, MN_SUB, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mul") == 0) {
		addToken(tokenList, MN_MUL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mod") == 0) {
		addToken(tokenList, MN_MOD, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "and") == 0) {
		addToken(tokenList, MN_AND, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "or") == 0) {
		addToken(tokenList, MN_OR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "xor") == 0) {
		addToken(tokenList, MN_XOR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "shl") == 0) {
		addToken(tokenList, MN_SHL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "shr") == 0) {
		addToken(tokenList, MN_SHR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "ja") == 0) {
		addToken(tokenList, MN_JA, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "je") == 0) {
		addToken(tokenList, MN_JE, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "jl") == 0) {
		addToken(tokenList, MN_JL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "jif") == 0) {
		addToken(tokenList, MN_JIF, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "cmp") == 0) {
		addToken(tokenList, MN_CMP, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "movb") == 0) {
		addToken(tokenList, MN_MOVB, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "call") == 0) {
		addToken(tokenList, MN_CALL, value, true);
		return true;
	}
	
	else if (strcmp(value.value_str, "reg") == 0) {
		addToken(tokenList, TK_REGISTER, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mem") == 0) {
		addToken(tokenList, TK_MEMORY, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "flags") == 0) {
		addToken(tokenList, TK_FLAG, value, true);
		return true;
	}
	
	else if (strcmp(value.value_str, "putchar") == 0) {
		addToken(tokenList, FUNC_PUTCHAR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "read") == 0) {
		addToken(tokenList, FUNC_READ, value, true);
		return true;
	}
	return false;
}

bool isAtEnd(char** text) {
	return **text == '\0';
}

bool isAtNewLine(char** text) {
	return **text == '\n';
}

bool isType(char** text, int* lenToken, regex_t* regex) {
	regmatch_t pmatch[1];
	
	if (regexec(regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

int getInteger(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);
	
	int number = atoi(string);
	
	free(string);
	return number;
}

char* getChar(char** text, int lenToken) {
	char* word = extractSubString(text, lenToken);

	return word;
}

void advance(char** text, int nb, int* nb_instructions) {
	column += nb;
	if (isAtNewLine(text)) {
		(*nb_instructions)++;
		line++;
		column = 0;
	}
	(*text) += nb; /* /!\ Ordre des opérateurs : *text++ != *(text++) */
}

TokenList lexer(char** text, int* nb_instructions) {
	int lenToken = 0;
	*nb_instructions = 0;
	regexList regexes;
	initRegexes(&regexes);

	TokenList *tokenList = initTokenList(text);

	/* Analyse lexicale */
	while (!isAtEnd(text)) {
		if (**text == ' ' || **text == '\n' || **text == '\t') {
			advance(text, 1, nb_instructions);
			continue;
		}

		union TokenValue value = { 0 };
		switch (**text) {
			case '|':
				value.value_chr = **text;
				addToken(tokenList, TK_PIPE, value, false);
				advance(text, 1, nb_instructions);
				break;
			case ',':
				value.value_chr = **text;
				addToken(tokenList, TK_COMMA, value, false);
				advance(text, 1, nb_instructions);
				break;
			case '>':
				value.value_chr = **text;
				addToken(tokenList, TK_JUMP_ARROW, value, false);
				advance(text, 1, nb_instructions);
				break;
			case '&':
				value.value_chr = **text;
				addToken(tokenList, TK_LOGICAL_AND, value, false);
				advance(text, 1, nb_instructions);
				break;
			case '(':
				value.value_chr = **text;
				addToken(tokenList, TK_L_PARENTH, value, false);
				advance(text, 1, nb_instructions);
				break;
			case ')':
				value.value_chr = **text;
				addToken(tokenList, TK_R_PARENTH, value, false);
				advance(text, 1, nb_instructions);
				break;
			case '[':
				value.value_chr = **text;
				addToken(tokenList, TK_L_SQ_BRACKET, value, false);
				advance(text, 1, nb_instructions);
				break;
			case ']':
				value.value_chr = **text;
				addToken(tokenList, TK_R_SQ_BRACKET, value, false);
				advance(text, 1, nb_instructions);
				break;

			/* NUMBERS + KEYWORDS */
			default:
			{
				/* INTEGERS */
				if (isType(text, &lenToken, &(regexes.integer)))
				{
					value.value_int = getInteger(text, lenToken);
					addToken(tokenList, TK_INT_NUMBER, value, false);
					advance(text, lenToken, nb_instructions);
				}
				/* KEYWORDS */
				else if (isType(text, &lenToken, &(regexes.charac)))
				{
					value.value_str = getChar(text, lenToken);
					if (!canAddKeywordToken(tokenList, value))
					{
						fprintf(stderr, "Unrecognized token: %s\n", value.value_str);
						exit(1);
					}
					advance(text, lenToken, nb_instructions);
				}
				else
				{
					printf("Unknown caracter: %c, at line %d, column %d.\n", **text, line, column);
					advance(text, 1, nb_instructions);
				}
				break;
			}
		}
	}

	// printf("Liste des tokens (%d tokens):\n\n", tokenList->indexToken);
	// for (int i = 0; i < tokenList->indexToken; i++) {
	// 	Token token = tokenList->tokens[i];
	// 	printf("#%d, TYPE: %d, VALUE: %s, LINE: %d, COLUMN: %d\n", i, token.kind, print_token(&token), token.line, token.column);
		
	// }
	// printf("\n");
	// exit(1);

	freeRegexes(&regexes);

	return *tokenList;
}