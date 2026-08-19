#include "frontend/parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils/utils.h"
#include "frontend/token.h"

// TODO: delete
// #include "pretty_printer.h"

Token show_next_token(TokenList* tokenList)
{
    return tokenList->tokens[tokenList->token_index];
}

Token accept_token(TokenList* tokenList)
{
    tokenList->token_index++;
    if (tokenList->token_index - 1 < tokenList->nb_tokens)
    {
        return tokenList->tokens[tokenList->token_index - 1];
    }
    
    fprintf(stderr, "Error token indice (%d > %d)\n", tokenList->token_index-1, tokenList->nb_tokens);
    exit(1);
}

Token expect_token(TokenList* tokenList, enum Token_t tokenType)
{
    Token token = show_next_token(tokenList);
    if (token.kind != tokenType) {
        error("Syntax error", &token);
    }
    return accept_token(tokenList);
}

Asm* parser(TokenList* tokenList, int nb_instructions)
{
    tokenList->token_index = 0;

    Asm *program = parse_asm(tokenList, nb_instructions);

    return program;
}

int find_instruction_offset(Asm* program, int offset)
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
Asm* parse_asm(TokenList* tokenList, int nb_instructions)
{
    Asm *program = malloc(sizeof(Asm));
    if (program == NULL)
    {
        perror("Error instruction malloc.\n");
        exit(EXIT_FAILURE);
    }

    program->instr_idx = 0;
    program->nb_instructions = nb_instructions;
    program->instructions = malloc(program->nb_instructions * sizeof(Instruction));

    for (int offset = 0; offset < program->nb_instructions; offset++)
    {
        program->instructions[program->instr_idx++] = parse_instruction(tokenList, offset);
    }
    
    program->instr_idx = 0;
    
    // resolve unresolved jumps
    for (int offset = 0; offset < program->nb_instructions; offset++)
    {
        Instruction instr = program->instructions[offset];
        InstructionKind kind = instr.kind;
        
        if (kind == INSTR_JMP)
        {
            int jump_addr = instr.jump->true_branch->offset;
            int instr_idx = find_instruction_offset(program, jump_addr);

            if (instr_idx != -1)
            {
                instr.jump->jump_resolved = true;

                int offset_true_branch = instr.jump->true_branch->offset;
                free(instr.jump->true_branch);
                instr.jump->true_branch = &(program->instructions[offset_true_branch]);
                
                if (instr.jump->condition->kind != COND_TRUE)
                {
                    int offset_false_branch = instr.jump->false_branch->offset;
                    free(instr.jump->false_branch);
                    instr.jump->false_branch = &(program->instructions[offset_false_branch]);
                }

                // TODO: fuse cmp in jmp condition
            }
            else
            {
                fprintf(stderr, "[-] Unable to resolve the jump address, TODO :)\n");
                exit(1);
            }
        }
    }

    return program;
}

// ------------------------------------------- Instruction parsing ----------------------------------------------
ParseFunc find_parsing_instruction(enum Token_t tokenType, Parse_DispatchTable* table)
{
    for (int i = 0; i < TOKEN_MAX; i++)
    {
        if (table[i].tokenType == tokenType)
        {
            return table[i].func;
        }
    }

    return NULL;
}

ParseJmpFunc find_parsing_jmp_instruction(enum Token_t tokenType, Parse_jmp_DispatchTable* table)
{
    for (int i = 0; i < TOKEN_MAX; i++)
    {
        if (table[i].tokenType == tokenType)
        {
            return table[i].func;
        }
    }

    return NULL;
}

Instruction parse_instruction(TokenList* tokenList, int offset)
{
    // printf("parsing instruction\n");
    Instruction instruction;
    
    Token nextToken = show_next_token(tokenList);
    
    Parse_DispatchTable parser_table[TOKEN_MAX] = {
        {MN_MOV, parse_mov},
        {MN_MOVB, parse_movb},
        {MN_CMP, parse_cmp},

        {MN_ADD, parse_op},
        {MN_SUB, parse_op},
        {MN_XOR, parse_op},
        {MN_AND, parse_op},
        {MN_SHL, parse_op},
        {MN_SHR, parse_op},
        {MN_OR, parse_op},
        {MN_MOD, parse_op},
    };

    Parse_jmp_DispatchTable parser_jmp_table[TOKEN_MAX] = {
        {MN_JIF, parse_jmp},
        {MN_JA, parse_jmp},
        {MN_JE, parse_jmp},
        {MN_JL, parse_jmp},
        {MN_JMP, parse_jmp},
    };
    
    ParseFunc parse = find_parsing_instruction(nextToken.kind, parser_table);
    
    if (parse == NULL)
    {
        ParseJmpFunc parser_jmp = find_parsing_jmp_instruction(nextToken.kind, parser_jmp_table);
        
        if (parser_jmp != NULL)
        {
            instruction = parser_jmp(tokenList, offset);
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
        instruction = parse(tokenList);
    }
    
    instruction.offset = offset;

    return instruction;
}

ASMOperand* parse_operand(TokenList* tokenList)
{
    // printf("parsing operand\n");
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (operand == NULL)
    {
        fprintf(stderr, "malloc() ASMOperand failed\n");
        exit(1);
    }
    
    Token nextToken = show_next_token(tokenList);
    
    if (nextToken.kind == TK_INT_NUMBER)
    {
        // printf("start int\n");
        operand->kind = OPERAND_INT;

        operand->integer = parse_int(tokenList);
        // printf("end int\n");
    }
    else if (nextToken.kind == TK_REGISTER)
    {
        // printf("start reg\n");
        operand->kind = OPERAND_REG;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        operand->reg = malloc(sizeof(ASMReg));

        operand->reg->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end reg\n");
    }
    else if (nextToken.kind == TK_MEMORY)
    {
        // printf("start mem\n");
        operand->kind = OPERAND_MEM;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        operand->mem = malloc(sizeof(ASMMem));

        operand->mem->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end mem\n");
    }
    else
    {
        error("wrong operand", &nextToken);
    }
    
    return operand;
}

ASMFlag* parse_flag(TokenList* tokenList)
{
    ASMFlag* flag = malloc(sizeof(ASMFlag));
    if (flag == NULL)
    {
        fprintf(stderr, "malloc() ASMFlag failed\n");
        exit(1);
    }

    flag->index = 0;
    accept_token(tokenList);

    return flag;
}

ASMInt* parse_int(TokenList* tokenList)
{
    // printf("parsing int\n");
    ASMInt* value = malloc(sizeof(ASMInt));
    if (value == NULL)
    {
        fprintf(stderr, "malloc() ASMInt failed\n");
        exit(1);
    }
    
    // Or : value->value = expect_token(tokenList, TK_INT_NUMBER);
    Token nextToken = show_next_token(tokenList);
    
    if (nextToken.kind == TK_INT_NUMBER)
    {
        // printf("got %d\n", nextToken.value.value_int);
        value->value = nextToken.value.value_int;
        accept_token(tokenList);
    }
    else
    {
        error("parsing int", &nextToken);
    }
    
    return value;
}

ASMDst* parse_dst(TokenList* tokenList)
{
    // printf("parsing dst\n");
    ASMDst* dst = malloc(sizeof(ASMDst));
    if (dst == NULL)
    {
        fprintf(stderr, "malloc() ASMDst failed\n");
        exit(1);
    }
    
    Token nextToken = show_next_token(tokenList);
    
    if (nextToken.kind == TK_REGISTER)
    {
        // printf("start reg\n");
        dst->kind = DST_REG;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        dst->reg = malloc(sizeof(ASMReg));
        
        dst->reg->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end reg\n");
    }
    else if (nextToken.kind == TK_MEMORY)
    {
        // printf("start mem\n");
        dst->kind = DST_MEM;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        dst->mem = malloc(sizeof(ASMMem));
        
        dst->mem->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end mem\n");
    }
    else if (nextToken.kind == TK_FLAG)
    {
        // printf("start flag\n");
        dst->kind = DST_FLAG;
        dst->flag = parse_flag(tokenList);
        // printf("end flag\n");
    }
    else
    {
        error("wrong dst operand", &nextToken);
    }
    
    return dst;
}

ASMSrc* parse_src(TokenList* tokenList)
{
    // printf("parsing src\n");
    ASMSrc* src = malloc(sizeof(ASMSrc));
    if (src == NULL)
    {
        fprintf(stderr, "malloc() ASMSrc failed\n");
        exit(1);
    }
    
    Token nextToken = show_next_token(tokenList);
    
    if (nextToken.kind == TK_INT_NUMBER)
    {
        // printf("start int\n");
        src->kind = SRC_INT;

        src->integer = parse_int(tokenList);
        // printf("end int\n");
    }
    else if (nextToken.kind == TK_REGISTER)
    {
        // printf("start reg\n");
        src->kind = SRC_REG;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        src->reg = malloc(sizeof(ASMReg));
        
        src->reg->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end reg\n");
    }
    else if (nextToken.kind == TK_MEMORY)
    {
        // printf("start mem\n");
        src->kind = SRC_MEM;
        accept_token(tokenList);
        
        expect_token(tokenList, TK_L_SQ_BRACKET);
        src->mem = malloc(sizeof(ASMMem));
        
        src->mem->index = parse_operand(tokenList);
        expect_token(tokenList, TK_R_SQ_BRACKET);
        // printf("end mem\n");
    }
    else if (nextToken.kind == TK_FLAG)
    {
        // printf("start flag\n");
        src->kind = SRC_FLAG;
        src->flag = parse_flag(tokenList);
        // printf("end flag\n");
    }
    else
    {
        error("wrong src operand", &nextToken);
    }

    return src;
}

ASMOperation* parse_operation(TokenList* tokenList)
{
    // printf("parsing operation\n");

    ASMOperation* operation = malloc(sizeof(ASMOperation));
    if (operation == NULL)
    {
        fprintf(stderr, "malloc() ASMOperation failed\n");
        exit(1);
    }

    operation->dst = parse_dst(tokenList); // WRONG: cannnot compute the result of the operation of 2 ints...
    
    Token nextToken = show_next_token(tokenList);

    switch (nextToken.kind)
    {
    case TK_LOGICAL_AND:
        operation->operator = OP_AND;
        break;
    
    default:
        fprintf(stderr, "Wrong operand in the operation\n");
        exit(1);
        break;
    }

    accept_token(tokenList);

    operation->src = parse_src(tokenList);

    return operation;
}

ASMCondition* parse_condition(TokenList* tokenList)
{
    // printf("parsing condition\n");
    ASMCondition* condition = malloc(sizeof(ASMCondition));
    if (condition == NULL)
    {
        fprintf(stderr, "malloc() ASMCondition failed\n");
        exit(1);
    }

    // printf("start condition\n");
    condition->lcond = parse_operation(tokenList);
    Token operationType = show_next_token(tokenList);
    switch (operationType.kind)
    {
    case OP_EQ:
        condition->kind = COND_EQ;
        break;
    
    default:
        fprintf(stderr, "Unrecognized condition operator\n");
        exit(1);
        break;
    }
    condition->rcond = parse_operation(tokenList);
    // printf("end condition\n");

    return condition;
}

Instruction parse_mov(TokenList* tokenList)
{
    // printf("parsing mov\n");
    Instruction instruction;
    
    expect_token(tokenList, MN_MOV);
    
    ASMDst* dst = parse_dst(tokenList);
    
    expect_token(tokenList, TK_COMMA);
    
    ASMSrc* src = parse_src(tokenList);
    
    ASMDstKind dstType = dst->kind;
    
    // load or store
    if (dstType == DST_REG)
    {
        instruction.kind = INSTR_STORE;
        instruction.store = malloc(sizeof(ASMStore));
        
        instruction.store->dst = dst;
        instruction.store->src = src;
    }
    else if (dstType == DST_MEM)
    {
        instruction.kind = INSTR_LOAD;
        instruction.load = malloc(sizeof(ASMLoad));

        instruction.load->dst = dst;
        instruction.load->src = src;
    }

    return instruction;
}

Instruction parse_movb(TokenList* tokenList)
{
    // printf("parsing movb\n");
    Instruction instruction;
    
    expect_token(tokenList, MN_MOVB);

    ASMDst* dst = parse_dst(tokenList);

    expect_token(tokenList, TK_COMMA);
    
    ASMSrc* src = parse_src(tokenList);
    
    
    ASMDstKind dstType = dst->kind;
    
    if (dstType == DST_REG)
    {
        instruction.kind = INSTR_STORE;
        instruction.store = malloc(sizeof(ASMStore));
        
        instruction.store->dst = dst;
        instruction.store->src = src;
    }
    else if (dstType == DST_MEM)
    {
        instruction.kind = INSTR_LOAD;
        instruction.load = malloc(sizeof(ASMLoad));

        instruction.load->dst = dst;
        instruction.load->src = src;
    }
    
    return instruction;
}

Instruction parse_op(TokenList* tokenList)
{
    // printf("parsing add\n");
    Instruction instruction;
    Token nextToken = show_next_token(tokenList);
    enum Token_t mnemonic = nextToken.kind;
    
    instruction.operation = malloc(sizeof(ASMOperation));

    switch (mnemonic)
    {
    case MN_ADD:
        expect_token(tokenList, MN_ADD);
        instruction.operation->operator = OP_ADD;
        break;
    case MN_SUB:
        expect_token(tokenList, MN_SUB);
        instruction.operation->operator = OP_SUB;
        break;
    case MN_XOR:
        expect_token(tokenList, MN_XOR);
        instruction.operation->operator = OP_ADD;
        break;
    case MN_AND:
        expect_token(tokenList, MN_AND);
        instruction.operation->operator = OP_AND;
        break;
    case MN_SHL:
        expect_token(tokenList, MN_SHL);
        instruction.operation->operator = OP_SHL;
        break;
    case MN_SHR:
        expect_token(tokenList, MN_SHR);
        instruction.operation->operator = OP_SHR;
        break;
    case MN_OR:
        expect_token(tokenList, MN_OR);
        instruction.operation->operator = OP_OR;
        break;
    case MN_MOD:
        expect_token(tokenList, MN_MOD);
        instruction.operation->operator = OP_MOD;
        break;
    default:
        fprintf(stderr, "And don't forget to implement the case of '%s' in parse_op :)\n", print_token(&nextToken));
        exit(1);
        break;
    }

    ASMDst* dst = parse_dst(tokenList);
    
    expect_token(tokenList, TK_COMMA);

    ASMSrc* src = parse_src(tokenList);
    
    instruction.kind = INSTR_OP;
    
    instruction.operation->dst = dst;
    instruction.operation->src = src;
    
    return instruction;
}

Instruction parse_cmp(TokenList* tokenList)
{
    // printf("parsing cmp\n");
    Instruction instruction;
    instruction.comparison = malloc(sizeof(ASMComparison));
    
    expect_token(tokenList, MN_CMP);

    ASMDst* dst = parse_dst(tokenList);
    
    expect_token(tokenList, TK_COMMA);
    
    ASMSrc* src = parse_src(tokenList);
    
    instruction.kind = INSTR_CMP;
    
    instruction.comparison->dst = dst;
    instruction.comparison->src = src;
    
    return instruction;
}

Instruction parse_call(TokenList* tokenList) {
    // printf("parsing call\n");
    Instruction instruction;
    
    expect_token(tokenList, MN_CALL);
    
    Token nextToken = show_next_token(tokenList);
    enum Token_t func = nextToken.kind;
    
    instruction.kind = INSTR_CALL;

    instruction.call = malloc(sizeof(ASMCall));
    
    switch (func)
    {
        case FUNC_PUTCHAR:
        instruction.call->name = malloc((7 + 1) * sizeof(char));
        if (instruction.call->name == NULL)
        {
            fprintf(stderr, "malloc() ASMCall failed\n");
            exit(1);
        }
        
        strcpy(instruction.call->name, "putchar");
        expect_token(tokenList, FUNC_PUTCHAR);
        expect_token(tokenList, TK_L_PARENTH);
        expect_token(tokenList, TK_R_PARENTH);
        break;
        
        case FUNC_READ:
        instruction.call->name = malloc((4 + 1) * sizeof(char));
        if (instruction.call->name == NULL)
        {
            fprintf(stderr, "malloc() ASMCall failed\n");
            exit(1);
        }

        strcpy(instruction.call->name, "read");
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

Instruction parse_jmp(TokenList* tokenList, int offset)
{
    Instruction instruction;
    Token nextToken = show_next_token(tokenList);
    enum Token_t mnemonic = nextToken.kind;

    instruction.jump = malloc(sizeof(ASMJump));
    if (instruction.jump == NULL)
    {
        fprintf(stderr, "malloc() ASMJump failed\n");
        exit(1);
    }
    
    ASMCondition* condition = malloc(sizeof(ASMCondition));
    if (condition == NULL)
    {
        fprintf(stderr, "malloc() ASMCondition failed\n");
        exit(1);
    }
    
    instruction.kind = INSTR_JMP;

    condition->lcond = NULL;
    condition->rcond = NULL;
        
    switch (mnemonic)
    {
    case MN_JIF:
        // printf("parsing jif\n");
        expect_token(tokenList, MN_JIF);
        expect_token(tokenList, TK_PIPE);
        condition->lcond = parse_operation(tokenList);
        expect_token(tokenList, TK_PIPE);
        condition->kind = COND_EQ;
        break;
    case MN_JA:
        // printf("parsing ja\n");
        condition->kind = COND_ABOVE;
        expect_token(tokenList, MN_JA);
        break;
    case MN_JE:
        // printf("parsing je\n");
        condition->kind = COND_EQ;
        expect_token(tokenList, MN_JE);
        break;
    case MN_JL:
        // printf("parsing jl\n");
        condition->kind = COND_LESS;
        expect_token(tokenList, MN_JL);
        break;
    case MN_JMP:
        // printf("parsing jl\n");
        condition->kind = COND_TRUE;
        expect_token(tokenList, MN_JMP);
        break;
    default:
        fprintf(stderr, "And don't forget to implement the case of '%s' in parse_jmp :)\n", print_token(&nextToken));
        exit(1);
        break;
    }

    // TODO: replace (bogus check, just to continue the program)
    ASMDst* dst = parse_dst(tokenList);
    free_dst(dst);

    expect_token(tokenList, TK_JUMP_ARROW);

    ASMInt* jump = parse_int(tokenList);
    int jump_addr = jump->value;
    free(jump);

    instruction.jump->true_branch = malloc(sizeof(Instruction));
    if (instruction.jump->true_branch == NULL)
    {
        fprintf(stderr, "malloc() Instruction failed\n");
        exit(1);
    }
    instruction.jump->true_branch->offset = jump_addr;

    if (condition->kind == COND_TRUE)
    {
        instruction.jump->false_branch = NULL;
    }
    else
    {
        instruction.jump->false_branch = malloc(sizeof(Instruction));
        if (instruction.jump->false_branch == NULL)
        {
            fprintf(stderr, "malloc() Instruction failed\n");
            exit(1);
        }
        instruction.jump->false_branch->offset = offset + 1;
    }

    instruction.jump->jump_resolved = false;
    instruction.jump->condition = condition;

    return instruction;
}