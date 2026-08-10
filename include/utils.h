#ifndef H_UTILS
#define H_UTILS

#include <regex.h>
#include "token.h"

typedef struct {
    regex_t integer;
    regex_t charac;
} regexList;

void freeTokens(TokenList* tokenList);
void freeRegexes(regexList* regexes);

void initRegexes(regexList* regexes);
TokenList *initTokenList(char** text);

char* extractSubString(char** string, int len);

char* read_asm_file(char* path);

#endif