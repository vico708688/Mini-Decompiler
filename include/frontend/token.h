#ifndef H_TOKEN
#define H_TOKEN

#include <stdbool.h>

// /!\ carefull with the order -> mnemonic first
typedef enum Token_t
{
    // classical mnemonics
    MN_MOV,
    MN_ADD,
    MN_SUB,
    MN_MUL,
    MN_MOD,
    MN_AND,
    MN_OR,
    MN_XOR,
    MN_SHL,
    MN_SHR,

    // special mnemonics
    MN_JIF,
    MN_JA,
    MN_JE,
    MN_JL,
    MN_JMP,
    MN_CMP,
    MN_MOVB,
    MN_CALL,
    
    // end of the mnemonic list
    TOKEN_MAX,
    
    // special char
	TK_COMMA,
    TK_PIPE,
    TK_EQUAL,
    TK_JUMP_ARROW,
    TK_LOGICAL_AND,
	TK_L_PARENTH,
	TK_R_PARENTH,
	TK_L_SQ_BRACKET,
	TK_R_SQ_BRACKET,

    // functions
    FUNC_PUTCHAR,
    FUNC_READ,

    // operands
	TK_INT_NUMBER,
    TK_REGISTER,
    TK_MEMORY,
    TK_FLAG,
	TK_LABEL,

} Token_t;

union TokenValue {
	char* value_str;
	char value_chr;
	int value_int;
};

typedef struct {
	enum Token_t kind;
	union TokenValue value;
    bool ownstr;
	int line;
	int column;
} Token;

typedef struct {
	Token* tokens;
    int nb_tokens;
	int token_index;
} TokenList;

char* print_token(Token* token);
void error(char* error_str, Token* token);

#endif