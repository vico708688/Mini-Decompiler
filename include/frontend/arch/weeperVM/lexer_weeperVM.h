#ifndef H_LEXER_WEEPERVM
#define H_LEXER_WEEPERVM

#include "frontend/arch/weeperVM/token_weeperVM.h"

#include <stdbool.h>
#include <regex.h>

typedef struct
{
    regex_t integer;
    regex_t charac;
} regexList;

void init_regexes(regexList* regexes);
void free_regexes(regexList* regexes);
void free_tokens_weeperVM(TokenList* tokenList);
TokenList *init_token_list(char** text);

void addToken(TokenList *tokenList, enum TokenKind kind, union TokenValue value, bool ownstr);
bool canAddKeywordToken(TokenList *tokenList, union TokenValue value);

bool isAtEnd(char** text);
bool isAtNewLine(char** text);

bool isType(char** text, int* lenToken, regex_t* regex);

int getInteger(char** text, int lenToken);
char* getChar(char** text, int lenToken);

void advance(char** text, int nb, int* nb_instructions);

TokenList* lexer_weeperVM(char** text, int* nb_instructions);


#endif