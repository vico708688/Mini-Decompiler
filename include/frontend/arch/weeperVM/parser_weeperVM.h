#ifndef H_PARSER_WEEPERVM
#define H_PARSER_WEEPERVM

#include "frontend/arch/weeperVM/token_weeperVM.h"
#include "frontend/arch/weeperVM/ASM_weeperVM.h"

typedef ASMInstruction (*ParseFunc)(TokenList*);
typedef struct Parse_DispatchTable
{
    enum TokenKind tokenType;
    ParseFunc func;
} Parse_DispatchTable;

typedef ASMInstruction (*ParseJmpFunc)(TokenList*);
typedef struct Parse_jmp_DispatchTable
{
    enum TokenKind tokenType;
    ParseJmpFunc func;
} Parse_jmp_DispatchTable;

void free_asm_weeperVM(ASM* program);
void free_asm_instruction(ASMInstruction* instruction);
void free_asm_operand(ASMOperand* operand);

ASM* parser_weeperVM(TokenList* tokenList, int nb_instructions);

Token show_next_token(TokenList* tokenList);
Token accept_token(TokenList* tokenList);
Token expect_token(TokenList* tokenList, enum TokenKind tokenType);

int find_instruction_offset(ASM* program, int offset);

// ------------------------------------------- Asm_WeeperVM parsing ----------------------------------------------
ASM* parse_asm(TokenList* tokenList, int nb_instructions);

// ------------------------------------------- Instruction parsing ----------------------------------------------
ASMInstruction parse_instruction(TokenList* tokenList, int offset);
ParseFunc find_parsing_op_instruction(enum TokenKind tokenType, Parse_DispatchTable* table);
ParseJmpFunc find_parsing_jmp_instruction(enum TokenKind tokenType, Parse_jmp_DispatchTable* table);

ASMOperand* parse_operand(TokenList* tokenList);
int parse_imm(TokenList* tokenList);

ASMInstruction parse_op(TokenList* tokenList);
ASMInstruction parse_jmp(TokenList* tokenList);
ASMInstruction parse_call(TokenList* tokenList);

#endif