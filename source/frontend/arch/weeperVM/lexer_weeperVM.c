#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "frontend/arch/weeperVM/token_weeperVM.h"
#include "frontend/arch/weeperVM/lexer_weeperVM.h"
#include "utils/utils.h"

int column = 1;
int line = 1;

void free_tokens_weeperVM(TokenList* tokenList)
{
	for (int i = 0; i < tokenList->nb_tokens; i++)
	{
		if (tokenList->tokens[i].ownstr)
		{
            free(tokenList->tokens[i].value.value_str);
        }
	}
	free(tokenList->tokens);
	free(tokenList);
}

void free_regexes(regexList* regexes)
{
	regfree(&(regexes->integer));
	regfree(&(regexes->charac));
}

void init_regexes(regexList* regexes)
{
	if(regcomp(&(regexes->integer), "^[0-9]+", REG_EXTENDED))
	{
		perror("Error regcomp integer\n");
		exit(1);
	}

	if(regcomp(&(regexes->charac), "^[[:alpha:]][a-zA-Z_0-9]*", REG_EXTENDED))
	{
		perror("Error regcomp character\n");
		exit(1);
	}
}

char* print_token(Token* token)
{
    char* str = malloc(16 * sizeof(char));

    if (token->ownstr) {
        str = token->value.value_str;
    }
    else if (token->kind == TK_R_SQ_BRACKET || token->kind == TK_L_SQ_BRACKET || token->kind == TK_COMMA || token->kind == TK_PIPE || token->kind == TK_JUMP_ARROW || token->kind == TK_LOGICAL_AND || token->kind == TK_L_PARENTH || token->kind == TK_R_PARENTH) {
        sprintf(str, "%s", (char[]){token->value.value_chr, '\0'});
    }
    else if (token->kind == TK_IMMEDIATE) {
        sprintf(str, "%d", token->value.value_int);
    }

    return str;
}

void error(char* error_str, Token* token)
{
    fprintf(stderr, "Error at line %d, column %d with token '%s' : %s\n", token->line, token->column, print_token(token), error_str);
    exit(1);
}

void addToken(TokenList *tokenList, enum TokenKind kind, union TokenValue value, bool ownstr)
{
	Token token = { .kind = kind, .value = value, .line = line, .column = column, .ownstr = ownstr };
	
	if (tokenList->token_index >= tokenList->nb_tokens)
	{
		Token *tmp = NULL;
		tokenList->nb_tokens *= 2;
		tmp = realloc(tokenList->tokens, tokenList->nb_tokens * sizeof(Token));
		if (tmp == NULL)
		{
				free_tokens_weeperVM(tokenList);
				perror("Error realloc\n");
				exit(1);
		}
		tokenList->tokens = tmp;
		
		// after a realloc, initialization of the new structs !
		for (int i = tokenList->token_index; i < tokenList->nb_tokens; i++)
		{
			tokenList->tokens[i].ownstr = false;
		}
	}
	tokenList->tokens[tokenList->token_index] = token;
	(tokenList->token_index)++;
}

// TODO: dispatch table
bool canAddKeywordToken(TokenList *tokenList, union TokenValue value)
{
	if (strcmp(value.value_str, "mov") == 0)
	{
		addToken(tokenList, MN_MOV, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "add") == 0)
	{
		addToken(tokenList, MN_ADD, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "sub") == 0)
	{
		addToken(tokenList, MN_SUB, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mul") == 0)
	{
		addToken(tokenList, MN_MUL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mod") == 0)
	{
		addToken(tokenList, MN_REM, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "and") == 0)
	{
		addToken(tokenList, MN_AND, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "or") == 0)
	{
		addToken(tokenList, MN_OR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "xor") == 0)
	{
		addToken(tokenList, MN_XOR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "shl") == 0)
	{
		addToken(tokenList, MN_SHL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "shr") == 0)
	{
		addToken(tokenList, MN_SHR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "ja") == 0)
	{
		addToken(tokenList, MN_JA, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "je") == 0)
	{
		addToken(tokenList, MN_JE, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "jl") == 0)
	{
		addToken(tokenList, MN_JL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "jmp") == 0)
	{
		addToken(tokenList, MN_JMP, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "jif") == 0)
	{
		addToken(tokenList, MN_JNZ, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "cmp") == 0)
	{
		addToken(tokenList, MN_CMP, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "movb") == 0)
	{
		addToken(tokenList, MN_MOVB, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "call") == 0)
	{
		addToken(tokenList, MN_CALL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "reg") == 0)
	{
		addToken(tokenList, TK_REGISTER, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "mem") == 0)
	{
		addToken(tokenList, TK_MEMORY, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "flags") == 0)
	{
		addToken(tokenList, TK_FLAG, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "putchar") == 0)
	{
		addToken(tokenList, FUNC_PUTCHAR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "read") == 0)
	{
		addToken(tokenList, FUNC_READ, value, true);
		return true;
	}
	return false;
}

bool isAtEnd(char** text)
{
	return **text == '\0';
}

bool isAtNewLine(char** text)
{
	return **text == '\n';
}

bool isType(char** text, int* lenToken, regex_t* regex)
{
	regmatch_t pmatch[1];
	
	if (regexec(regex, *text, 1, pmatch, 0) == 0)
	{
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

int getInteger(char** text, int lenToken)
{
	char* string = extract_sub_string(text, lenToken);
	
	int number = atoi(string);
	
	free(string);
	return number;
}

char* getChar(char** text, int lenToken)
{
	char* word = extract_sub_string(text, lenToken);

	return word;
}

void advance(char** text, int nb, int* nb_instructions)
{
	column += nb;
	if (isAtNewLine(text)) {
		(*nb_instructions)++;
		line++;
		column = 0;
	}
	(*text) += nb; /* /!\ Ordre des opérateurs : *text++ != *(text++) */
}

TokenList* init_token_list(char** text)
{
	TokenList *tokenList = malloc(sizeof(TokenList));

	if (tokenList == NULL)
	{
		printf("Error tokenList malloc.\n");
		exit(1);
	}
	
	tokenList->nb_tokens = 256;
	tokenList->token_index = 0;

	if ((tokenList->tokens = calloc(tokenList->nb_tokens, sizeof(Token))) == NULL)
	{
		perror("Error calloc\n");
		exit(1);
	}

	return tokenList;
}

TokenList* lexer_weeperVM(char** text, int* nb_instructions)
{
	int lenToken = 0;

	*nb_instructions = 1;
	regexList regexes;
	init_regexes(&regexes);

	TokenList *tokenList = init_token_list(text);

	/* Analyse lexicale */
	while (!isAtEnd(text))
	{
		if (**text == ' ' || **text == '\n' || **text == '\t')
		{
			advance(text, 1, nb_instructions);
			continue;
		}

		union TokenValue value = { 0 };
		switch (**text)
		{
			case ',':
				value.value_chr = **text;
				addToken(tokenList, TK_COMMA, value, false);
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
					addToken(tokenList, TK_IMMEDIATE, value, false);
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

	// printf("Liste des tokens (%d tokens):\n\n", tokenList->token_index);
	// for (int i = 0; i < tokenList->token_index; i++) {
	// 	Token token = tokenList->tokens[i];
	// 	printf("#%d, TYPE: %d, VALUE: %s, LINE: %d, COLUMN: %d\n", i, token.kind, print_token(&token), token.line, token.column);
		
	// }
	// printf("\n");
	// exit(1);

	free_regexes(&regexes);

	return tokenList;
}