#include <stdio.h>
#include <stdlib.h>

#include "AST.h"
#include "pretty_printer.h"

void visit_program(Visitor* v, Asm* program)
{
    for (int i = 0; i < program->nb_instructions; i++)
    {
        visit_instruction(v, &(program->instructions[i]));
    }
}

void visit_instruction(Visitor* v, Instruction* instruction)
{
    switch (instruction->kind)
    {
    case INSTR_OP:
        visit_op(v, instruction->operation);
        break;
    
    case INSTR_LOAD:
        visit_load(v, instruction->load);
        break;

    case INSTR_STORE:
        visit_store(v, instruction->store);
        break;

    case INSTR_CMP:
        visit_cmp(v, instruction->comparison);
        break;
    
    case INSTR_JMP:
        visit_jmp(v, instruction->jump);
        break;
    
    case INSTR_CALL:
        visit_call(v, instruction->call);
        break;
    
    default:
        fprintf(stderr, "Instruction cannot be printed\n");
        exit(0);
        break;
    }
    printf("\n");
}

void visit_operand(Visitor* v, Operand* operand)
{
    if (operand->kind == OPERAND_INT)
    {
        printf("%d", operand->integer->value);
    }
    else if (operand->kind == OPERAND_REG)
    {
        printf("reg[");
        visit_operand(v, operand->reg->index);
        printf("]");
    }
    else
    {
        printf("mem[");
        visit_operand(v, operand->mem->index);
        printf("]");
    }
}

void visit_dst(Visitor* v, Dst* dst)
{
    if (dst->kind == DST_REG)
    {
        printf("reg[");
        visit_operand(v, dst->reg->index);
        printf("]");
    }
    else if (dst->kind == DST_MEM)
    {
        printf("mem[");
        visit_operand(v, dst->mem->index);
        printf("]");
    }
    else if (dst->kind == DST_FLAG)
    {
        printf("flag");
    }
}

void visit_src(Visitor* v, Src* src)
{
    if (src->kind == SRC_INT)
    {
        printf("%d", src->integer->value);
    }
    else if (src->kind == SRC_REG)
    {
        printf("reg[");
        visit_operand(v, src->reg->index);
        printf("]");
    }
    else if (src->kind == SRC_MEM)
    {
        printf("mem[");
        visit_operand(v, src->mem->index);
        printf("]");
    }
    else if (src->kind == SRC_FLAG)
    {
        printf("flag");
    }
}


void visit_op(Visitor* v, Operation* operation)
{
    switch (operation->operator)
    {
    case OP_ADD:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" + ");
        visit_src(v, operation->src);
        break;
    
    case OP_SUB:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" - ");
        visit_src(v, operation->src);
        break;
    
    case OP_XOR:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" ^ ");
        visit_src(v, operation->src);
        break;
    
    case OP_OR:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" | ");
        visit_src(v, operation->src);
        break;
    
    case OP_MOD:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" %% ");
        visit_src(v, operation->src);
        break;
    
    case OP_AND:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" & ");
        visit_src(v, operation->src);
        break;
    
    case OP_SHR:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" >> ");
        visit_src(v, operation->src);
        break;
    
    case OP_SHL:
        visit_dst(v, operation->dst);
        printf(" = ");
        visit_dst(v, operation->dst);
        printf(" << ");
        visit_src(v, operation->src);
        break;
    
    default:
        fprintf(stderr, "Don't forget to implement the pretty printer\n");
        exit(0);
        break;
    }
}

void visit_load(Visitor* v, Load* load)
{
    visit_dst(v, load->dst);
    printf(" <- ");
    visit_src(v, load->src);
}

void visit_store(Visitor* v, Store* load)
{
    visit_src(v, load->src);
    printf(" -> ");
    visit_dst(v, load->dst);
}

void visit_cmp(Visitor* v, Comparison* comparison)
{
    printf("cmp ");
    visit_dst(v, comparison->dst);
    printf(", ");
    visit_src(v, comparison->src);
}

void visit_jmp_addr(Visitor* v, Instruction* instruction)
{
    printf("%d", instruction->offset);
}

// WRONG: only for my first program
void visit_operation(Visitor* v, Operation* operation)
{
    visit_dst(v, operation->dst);

    printf(" ");
    switch (operation->operator)
    {
    case OP_ADD:
        printf("+");
        break;
    
    case OP_SUB:
        printf("-");
        break;
    
    case OP_AND:
        printf("&");
        break;
    
    case OP_MOD:
        printf("%%");
        break;
    
    case OP_OR:
        printf("|");
        break;
    
    case OP_SHL:
        printf("<<");
        break;
    
    case OP_SHR:
        printf(">>");
        break;
    
    case OP_XOR:
        printf("^");
        break;
    
    case OP_EQ:
        printf("==");
        break;
    
    default:
        fprintf(stderr, "Unrecognized operator\n");
        break;
    }
    printf(" ");
    
    visit_src(v, operation->src);
}

void visit_condition(Visitor* v, Condition* condition)
{
    // printf("visiting condition\n");
    if (condition->lcond != NULL)
    {
        // printf("visiting operation\n");
        visit_operation(v, condition->lcond);
    }
    else
    {
        // fprintf(stderr, "Error no lvalue in condition\n");
        // exit(1);
        return;
    }
    
    printf(" ");
    switch (condition->kind)
    {
    case COND_EQ:
        printf("=");
        break;
        
    case COND_LESS:
        printf("<");
        break;
        
    case COND_ABOVE:
        printf(">");
        break;
        
    default:
        break;
    }
    printf(" ");

    if (condition->rcond != NULL)
    {
        visit_operation(v, condition->rcond);
    }
    else
    {
        printf("0");
        return;
    }
}

void visit_jmp(Visitor* v, Jump* jump)
{
    switch (jump->condition->kind)
    {
    case COND_EQ:
        printf("if equal ");
        break;
    
    case COND_ABOVE:
        printf("if above than ");
        break;
    
    case COND_LESS:
        printf("if less than ");
        break;
    
    default:
        fprintf(stderr, "WTF are you doing ? How is it possible to fail this check ???\n");
        exit(1);
        break;
    }

    if (jump->jump_resolved)
    {
        visit_condition(v, jump->condition);
    }
    else
    {
        fprintf(stderr, "Jump address not resolved\n");
        exit(1);
    }

    printf(" then\n\t");
    visit_jmp_addr(v, jump->true_branch);
    printf("\nelse\n\t");
    visit_jmp_addr(v, jump->false_branch);
}

void visit_call(Visitor* v, Call* call)
{
    printf("%s", call->name);
}
