#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"
#include "token.h"

char* read_asm_file(char* path) {
    FILE* fp;
    int ret;
    struct stat statbuf;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen() failed, path: %s\n", path);
        exit(1);
    }
    
    if (stat(path, &statbuf) == -1) {
        fprintf(stderr, "fstat() failed\n");
        exit(1);
    }

    int buffer_len = statbuf.st_size;
    char* buffer = malloc(buffer_len + 1);

    if (buffer == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(1);
    }

    ret = fread(buffer, 1, buffer_len, fp);
    if (ret != buffer_len) {
        fprintf(stderr, "fread() failed, ret: %d\n", ret);
        fclose(fp);
        exit(1);
    }

    buffer[buffer_len] = '\0';

    fclose(fp);

    return buffer;
}

void freeTokens(TokenList *tokenList) {
	for (int i = 0; i < tokenList->size; i++) {
		if (tokenList->tokens[i].ownstr) {
            free(tokenList->tokens[i].value.value_str);
        }
	}
	free(tokenList->tokens);
}

void freeRegexes(regexList* regexes) {
	regfree(&(regexes->integer));
	regfree(&(regexes->charac));
}

void initRegexes(regexList* regexes) {
	if(regcomp(&(regexes->integer), "^[0-9]+", REG_EXTENDED)) {
		perror("Error regcomp integer\n");
		exit(1);
	}

	if(regcomp(&(regexes->charac), "^[[:alpha:]][a-zA-Z_0-9]*", REG_EXTENDED)) {
		perror("Error regcomp character\n");
		exit(1);
	}
}

TokenList *initTokenList(char** text) {
	/* Allocation de 256 tokens puis realloc */
	TokenList *tokenList = malloc(sizeof(TokenList));

	if (tokenList == NULL)
	{
		printf("Error tokenList malloc.\n");
		exit(1);
	}
	
	tokenList->size = 256;
	tokenList->indexToken = 0;

	if ((tokenList->tokens = calloc(tokenList->size, sizeof(Token))) == NULL) {
		perror("Error calloc\n");
		exit(1);
	}

	return tokenList;
}

char* extractSubString(char** string, int len) {
	char* subString = malloc((len + 1) * sizeof(char));
	if (subString == NULL)
	{
		perror("Error malloc\n");
		exit(1);
	}
	
	strncpy(subString, *string, len);

	subString[len] = '\0';

	return subString;
}