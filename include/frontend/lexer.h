#ifndef H_LEXER
#define H_LEXER

#include "frontend/token.h"

#include <stdbool.h>
#include <regex.h>

void addToken(TokenList *tokenList, enum Token_t kind, union TokenValue value, bool ownstr);
bool canAddKeywordToken(TokenList *tokenList, union TokenValue value);

bool isAtEnd(char** text);
bool isAtNewLine(char** text);

bool isType(char** text, int* lenToken, regex_t* regex);

int getInteger(char** text, int lenToken);
char* getChar(char** text, int lenToken);

void advance(char** text, int nb, int* nb_instructions);

TokenList* lexer(char** text, int* nb_instructions);


#endif