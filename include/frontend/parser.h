#ifndef H_PARSER
#define H_PARSER

#include "frontend/token.h"
#include "frontend/ASM.h"

typedef Instruction (*ParseFunc)(TokenList*);
typedef struct Parse_DispatchTable
{
    enum Token_t tokenType;
    ParseFunc func;
} Parse_DispatchTable;

typedef Instruction (*ParseJmpFunc)(TokenList*, int);
typedef struct Parse_jmp_DispatchTable
{
    enum Token_t tokenType;
    ParseJmpFunc func;
} Parse_jmp_DispatchTable;

Asm* parser(TokenList* tokenList, int nb_instructions);

Token show_next_token(TokenList* tokenList);
Token accept_token(TokenList* tokenList);
Token expect_token(TokenList* tokenList, enum Token_t tokenType);

int find_instruction_offset(Asm* program, int offset);

// ------------------------------------------- Asm parsing ----------------------------------------------
Asm* parse_asm(TokenList* tokenList, int nb_instructions);

// ------------------------------------------- Instruction parsing ----------------------------------------------
Instruction parse_instruction(TokenList* tokenList, int offset);
ParseFunc find_parsing_instruction(enum Token_t tokenType, Parse_DispatchTable* table);
ParseJmpFunc find_parsing_jmp_instruction(enum Token_t tokenType, Parse_jmp_DispatchTable* table);

ASMOperand* parse_operand(TokenList* tokenList);
ASMOperation* parse_operation(TokenList* tokenList);
ASMInt* parse_int(TokenList* tokenList);
ASMDst* parse_dst(TokenList* tokenList);
ASMSrc* parse_src(TokenList* tokenList);

Instruction parse_mov(TokenList* tokenList);
Instruction parse_movb(TokenList* tokenList);
Instruction parse_op(TokenList* tokenList);
Instruction parse_cmp(TokenList* tokenList);
Instruction parse_call(TokenList* tokenList);
Instruction parse_jmp(TokenList* tokenList, int offset);

#endif