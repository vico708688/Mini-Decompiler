#include <stdio.h>
#include <stdlib.h>

#include "token.h"

char* print_token(Token* token)
{
    char* str = malloc(16 * sizeof(char));

    if (token->ownstr) {
        str = token->value.value_str;
    }
    else if (token->kind == TK_R_SQ_BRACKET || token->kind == TK_L_SQ_BRACKET || token->kind == TK_COMMA || token->kind == TK_PIPE || token->kind == TK_JUMP_ARROW || token->kind == TK_LOGICAL_AND || token->kind == TK_L_PARENTH || token->kind == TK_R_PARENTH) {
        sprintf(str, "%s", (char[]){token->value.value_chr, '\0'});
    }
    else if (token->kind == TK_INT_NUMBER) {
        sprintf(str, "%d", token->value.value_int);
    }

    return str;
}

void error(char* error_str, Token* token)
{
    fprintf(stderr, "Error at line %d, column %d with token '%s' : %s\n", token->line, token->column, print_token(token), error_str);
    exit(1);
}