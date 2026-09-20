#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "frontend/arch/weeperVM/parser_weeperVM.h"
#include "frontend/arch/weeperVM/token_weeperVM.h"

ASMOpcode token_to_opcode(TokenKind token)
{
    switch (token)
    {
    case MN_MOV:
        return OPCODE_MOV;
    case MN_ADD:
        return OPCODE_ADD;
    case MN_SUB:
        return OPCODE_SUB;
    case MN_MUL:
        return OPCODE_MUL;
    case MN_REM:
        return OPCODE_REM;
    case MN_AND:
        return OPCODE_AND;
    case MN_OR:
        return OPCODE_OR;
    case MN_XOR:
        return OPCODE_XOR;
    case MN_SHL:
        return OPCODE_SHL;
    case MN_SHR:
        return OPCODE_SHR;
    case MN_JNZ:
        return OPCODE_JNZ;
    case MN_JA:
        return OPCODE_JA;
    case MN_JE:
        return OPCODE_JE;
    case MN_JL:
        return OPCODE_JL;
    case MN_MOVB:
        return OPCODE_MOVB;
    case MN_CMP:
        return OPCODE_CMP;
    case MN_JMP:
        return OPCODE_JMP;
    case MN_CALL:
        return OPCODE_CALL;

    default:
        fprintf(stderr, "\e[31m Error\e[0m: During parsing, the token number %d is not a valid opcode.\n", token);
        exit(1);
        break;
    }
}

void free_asm_weeperVM(ASM *program)
{
    for (int i = 0; i < program->nb_instructions; i++)
    {
        ASMInstruction *instruction = &program->instructions[i];
        free_asm_instruction(instruction);
    }
    free(program->instructions);
    free(program);
}

void free_asm_instruction(ASMInstruction *instruction)
{
    if (instruction == NULL)
        return;

    switch (instruction->opcode)
    {
    case OPCODE_CALL:
        break;

    default:
        free_asm_operand(instruction->operation.arg0);
        free_asm_operand(instruction->operation.arg1);
        break;
    }
}

void free_asm_operand(ASMOperand *operand)
{
    if (operand == NULL)
        return;

    switch (operand->kind)
    {
    case OPE_MEM:
    case OPE_REG:
        free_asm_operand(operand->index);
        break;

    case OPE_IMM:
    case OPE_FLAG:
        break;

    default:
        break;
    }

    free(operand);
}

Token show_next_token(TokenList *tokenList)
{
    return tokenList->tokens[tokenList->token_index];
}

Token accept_token(TokenList *tokenList)
{
    tokenList->token_index++;
    if (tokenList->token_index - 1 < tokenList->nb_tokens)
    {
        return tokenList->tokens[tokenList->token_index - 1];
    }

    fprintf(stderr, "\e[31m Error\e[0m: wrong token indice (%d > %d)\n", tokenList->token_index - 1, tokenList->nb_tokens);
    exit(1);
}

Token expect_token(TokenList *tokenList, enum TokenKind tokenType)
{
    Token token = show_next_token(tokenList);
    if (token.kind != tokenType)
    {
        error("Syntax error", &token);
    }
    return accept_token(tokenList);
}

ASM *parser_weeperVM(TokenList *tokenList, int nb_instructions)
{
    tokenList->token_index = 0;

    ASM *program = parse_asm(tokenList, nb_instructions);

    return program;
}

int find_instruction_offset(ASM *program, int offset)
{
    for (int idx = 0; idx < program->nb_instructions; idx++)
    {
        if (program->instructions[idx].offset == offset)
        {
            return idx;
        }
    }

    return -1;
}

// ------------------------------------------- Asm parsing ----------------------------------------------
ASM *parse_asm(TokenList *tokenList, int nb_instructions)
{
    ASM *program = malloc(sizeof(ASM));
    if (program == NULL)
    {
        perror("\e[31m Error\e[0m instruction malloc.\n");
        exit(EXIT_FAILURE);
    }

    program->instr_idx = 0;
    program->nb_instructions = nb_instructions;
    program->instructions = calloc(program->nb_instructions, sizeof(ASMInstruction));

    for (int offset = 0; offset < program->nb_instructions; offset++)
    {
        program->instructions[program->instr_idx++] = parse_instruction(tokenList, offset);
    }

    return program;
}

// ------------------------------------------- Instruction parsing ----------------------------------------------
ParseFunc find_parsing_op_instruction(enum TokenKind tokenType, Parse_DispatchTable *table)
{
    for (int i = 0; i < MN_COUNT; i++)
    {
        if (table[i].tokenType == tokenType)
        {
            return table[i].func;
        }
    }

    return NULL;
}

ParseJmpFunc find_parsing_jmp_instruction(enum TokenKind tokenType, Parse_jmp_DispatchTable *table)
{
    for (int i = 0; i < MN_COUNT; i++)
    {
        if (table[i].tokenType == tokenType)
        {
            return table[i].func;
        }
    }

    return NULL;
}

ASMInstruction parse_instruction(TokenList *tokenList, int offset)
{
    ASMInstruction instruction;

    Token nextToken = show_next_token(tokenList);

    // keep in the parser or move in the lifter ?
    Parse_DispatchTable parser_op_table[MN_COUNT] = {
        {MN_MOV, parse_op},
        {MN_MOVB, parse_op},
        {MN_CMP, parse_op},
        {MN_ADD, parse_op},
        {MN_SUB, parse_op},
        {MN_XOR, parse_op},
        {MN_AND, parse_op},
        {MN_SHL, parse_op},
        {MN_SHR, parse_op},
        {MN_OR, parse_op},
        {MN_REM, parse_op},
    };

    Parse_jmp_DispatchTable parser_jmp_table[MN_COUNT] = {
        {MN_JNZ, parse_jmp},
        {MN_JA, parse_jmp},
        {MN_JE, parse_jmp},
        {MN_JL, parse_jmp},
        {MN_JMP, parse_jmp},
    };

    ParseFunc parser_op = find_parsing_op_instruction(nextToken.kind, parser_op_table);

    if (parser_op == NULL)
    {
        ParseJmpFunc parser_jmp = find_parsing_jmp_instruction(nextToken.kind, parser_jmp_table);

        if (parser_jmp != NULL)
        {
            instruction = parser_jmp(tokenList);
        }
        else
        {
            if (nextToken.kind == MN_CALL)
            {
                instruction = parse_call(tokenList);
            }
            else
            {
                error("Could not find a function to parse this mnemonic, add it in parse_instruction", &nextToken);
                exit(1);
            }
        }
    }
    else
    {
        instruction = parser_op(tokenList);
    }

    instruction.offset = offset;

    return instruction;
}

ASMOperand *parse_operand(TokenList *tokenList)
{
    ASMOperand *operand = malloc(sizeof(ASMOperand));
    if (operand == NULL)
    {
        fprintf(stderr, "malloc() Operand failed\n");
        exit(1);
    }

    Token nextToken = show_next_token(tokenList);

    if (nextToken.kind == TK_IMMEDIATE)
    {
        operand->kind = OPE_IMM;

        operand->value = parse_imm(tokenList);
    }
    else if (nextToken.kind == TK_REGISTER)
    {
        operand->kind = OPE_REG;
        accept_token(tokenList);

        expect_token(tokenList, TK_L_SQ_BRACKET);

        operand->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
    }
    else if (nextToken.kind == TK_MEMORY)
    {
        operand->kind = OPE_MEM;
        accept_token(tokenList);

        expect_token(tokenList, TK_L_SQ_BRACKET);

        operand->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
    }
    else if (nextToken.kind == TK_FLAG)
    {
        operand->kind = OPE_FLAG;
        accept_token(tokenList);

        operand->index = NULL;
    }
    else
    {
        error("wrong operand", &nextToken);
    }

    return operand;
}

int parse_imm(TokenList *tokenList)
{
    int value;

    Token nextToken = show_next_token(tokenList);

    if (nextToken.kind == TK_IMMEDIATE)
    {
        value = nextToken.value.value_int;
        accept_token(tokenList);
    }
    else
    {
        error("parsing immediate", &nextToken);
    }

    return value;
}

ASMInstruction parse_op(TokenList *tokenList)
{
    ASMInstruction instruction;

    Token nextToken = accept_token(tokenList);
    enum TokenKind mnemonic = nextToken.kind;

    instruction.opcode = token_to_opcode(mnemonic);

    instruction.operation.arg0 = parse_operand(tokenList);
    expect_token(tokenList, TK_COMMA);
    instruction.operation.arg1 = parse_operand(tokenList);

    return instruction;
}

ASMInstruction parse_jmp(TokenList *tokenList)
{
    ASMInstruction instruction;

    Token nextToken = accept_token(tokenList);
    enum TokenKind mnemonic = nextToken.kind;

    instruction.opcode = mnemonic;
    ASMOperand *jmp_addr = parse_operand(tokenList);

    if (mnemonic == MN_JNZ)
    {
        expect_token(tokenList, TK_COMMA);
        instruction.operation.arg0 = parse_operand(tokenList);
        instruction.operation.arg1 = jmp_addr;
    }
    else
    {
        instruction.operation.arg0 = jmp_addr;
        instruction.operation.arg1 = NULL;
    }

    return instruction;
}

ASMInstruction parse_call(TokenList *tokenList)
{
    ASMInstruction instruction = {0};

    expect_token(tokenList, MN_CALL);

    Token nextToken = show_next_token(tokenList);
    enum TokenKind func = nextToken.kind;

    instruction.opcode = token_to_opcode(MN_CALL);

    switch (func)
    {
    case FUNC_PUTCHAR:
        instruction.call_name = CALL_PUTCHAR;
        expect_token(tokenList, FUNC_PUTCHAR);
        expect_token(tokenList, TK_L_PARENTH);
        expect_token(tokenList, TK_R_PARENTH);
        break;

    case FUNC_READ:
        instruction.call_name = CALL_READ;
        expect_token(tokenList, FUNC_READ);
        expect_token(tokenList, TK_L_PARENTH);
        expect_token(tokenList, TK_R_PARENTH);
        break;

    default:
        fprintf(stderr, "Unrecognized function '%s'\n", nextToken.value.value_str);
        exit(1);
        break;
    }

    return instruction;
}