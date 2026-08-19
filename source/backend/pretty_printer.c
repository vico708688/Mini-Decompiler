#include <stdio.h>
#include <stdlib.h>

#include "backend/CFG.h"
#include "backend/pretty_printer.h"

void visit_program(Visitor* v, Asm* program, FILE* f)
{
    for (int i = 0; i < program->nb_instructions; i++)
    {
        visit_instruction(v, &(program->instructions[i]), f);
    }
}

void visit_instruction(Visitor* v, Instruction* instruction, FILE* f)
{
    switch (instruction->kind)
    {
    case INSTR_OP:
        visit_op(v, instruction->operation, f);
        break;
    
    case INSTR_LOAD:
        visit_load(v, instruction->load, f);
        break;

    case INSTR_STORE:
        visit_store(v, instruction->store, f);
        break;

    case INSTR_CMP:
        visit_cmp(v, instruction->comparison, f);
        break;
    
    case INSTR_JMP:
        visit_jmp(v, instruction->jump, f);
        break;
    
    case INSTR_CALL:
        visit_call(v, instruction->call, f);
        break;
    
    default:
        fprintf(stderr, "Instruction cannot be printed\n");
        exit(0);
        break;
    }
    fprintf(f, "\n");
}

void visit_operand(Visitor* v, ASMOperand* operand, FILE* f)
{
    if (operand->kind == OPERAND_INT)
    {
        fprintf(f, "%d", operand->integer->value);
    }
    else if (operand->kind == OPERAND_REG)
    {
        fprintf(f, "reg[");
        visit_operand(v, operand->reg->index, f);
        fprintf(f, "]");
    }
    else
    {
        fprintf(f, "mem[");
        visit_operand(v, operand->mem->index, f);
        fprintf(f, "]");
    }
}

void visit_dst(Visitor* v, ASMDst* dst, FILE* f)
{
    if (dst->kind == DST_REG)
    {
        fprintf(f, "reg[");
        visit_operand(v, dst->reg->index, f);
        fprintf(f, "]");
    }
    else if (dst->kind == DST_MEM)
    {
        fprintf(f, "mem[");
        visit_operand(v, dst->mem->index, f);
        fprintf(f, "]");
    }
    else if (dst->kind == DST_FLAG)
    {
        fprintf(f, "flag");
    }
}

void visit_src(Visitor* v, ASMSrc* src, FILE* f)
{
    if (src->kind == SRC_INT)
    {
        fprintf(f, "%d", src->integer->value);
    }
    else if (src->kind == SRC_REG)
    {
        fprintf(f, "reg[");
        visit_operand(v, src->reg->index, f);
        fprintf(f, "]");
    }
    else if (src->kind == SRC_MEM)
    {
        fprintf(f, "mem[");
        visit_operand(v, src->mem->index, f);
        fprintf(f, "]");
    }
    else if (src->kind == SRC_FLAG)
    {
        fprintf(f, "flag");
    }
}


void visit_op(Visitor* v, ASMOperation* operation, FILE* f)
{
    switch (operation->operator)
    {
    case OP_ADD:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " + ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_SUB:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " - ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_XOR:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " ^ ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_OR:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " | ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_MOD:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " %% ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_AND:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " & ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_SHR:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " >> ");
        visit_src(v, operation->src, f);
        break;
    
    case OP_SHL:
        visit_dst(v, operation->dst, f);
        fprintf(f, " = ");
        visit_dst(v, operation->dst, f);
        fprintf(f, " << ");
        visit_src(v, operation->src, f);
        break;
    
    default:
        fprintf(stderr, "Don't forget to implement the pretty printer\n");
        exit(0);
        break;
    }
}

void visit_load(Visitor* v, ASMLoad* load, FILE* f)
{
    visit_dst(v, load->dst, f);
    fprintf(f, " <- ");
    visit_src(v, load->src, f);
}

void visit_store(Visitor* v, ASMStore* load, FILE* f)
{
    visit_src(v, load->src, f);
    fprintf(f, " -> ");
    visit_dst(v, load->dst, f);
}

void visit_cmp(Visitor* v, ASMComparison* comparison, FILE* f)
{
    fprintf(f, "cmp ");
    visit_dst(v, comparison->dst, f);
    fprintf(f, ", ");
    visit_src(v, comparison->src, f);
}

void visit_jmp_addr(Visitor* v, Instruction* instruction, FILE* f)
{
    fprintf(f, "%d", instruction->offset);
}

// WRONG: only for my first program
void visit_operation(Visitor* v, ASMOperation* operation, FILE* f)
{
    visit_dst(v, operation->dst, f);

    fprintf(f, " ");
    switch (operation->operator)
    {
    case OP_ADD:
        fprintf(f, "+");
        break;
    
    case OP_SUB:
        fprintf(f, "-");
        break;
    
    case OP_AND:
        fprintf(f, "&");
        break;
    
    case OP_MOD:
        fprintf(f, "%%");
        break;
    
    case OP_OR:
        fprintf(f, "|");
        break;
    
    case OP_SHL:
        fprintf(f, "<<");
        break;
    
    case OP_SHR:
        fprintf(f, ">>");
        break;
    
    case OP_XOR:
        fprintf(f, "^");
        break;
    
    case OP_EQ:
        fprintf(f, "==");
        break;
    
    default:
        fprintf(stderr, "Unrecognized operator\n");
        break;
    }
    fprintf(f, " ");
    
    visit_src(v, operation->src, f);
}

void visit_condition(Visitor* v, ASMCondition* condition, FILE* f)
{
    // fprintf(f, "visiting condition\n");
    if (condition->lcond != NULL)
    {
        // fprintf(f, "visiting operation\n");
        visit_operation(v, condition->lcond, f);
    }
    else
    {
        // fprintf(stderr, "Error no lvalue in condition\n");
        // exit(1);
        return;
    }
    
    fprintf(f, " ");
    switch (condition->kind)
    {
    case COND_EQ:
        fprintf(f, "=");
        break;
        
    case COND_LESS:
        fprintf(f, "<");
        break;
        
    case COND_ABOVE:
        fprintf(f, ">");
        break;
        
    default:
        break;
    }
    fprintf(f, " ");

    if (condition->rcond != NULL)
    {
        visit_operation(v, condition->rcond, f);
    }
    else
    {
        fprintf(f, "0");
        return;
    }
}

void visit_jmp(Visitor* v, ASMJump* jump, FILE* f)
{
    switch (jump->condition->kind)
    {
    case COND_EQ:
        fprintf(f, "if equal ");
        break;
    
    case COND_ABOVE:
        fprintf(f, "if above than ");
        break;
    
    case COND_LESS:
        fprintf(f, "if less than ");
        break;
    
    case COND_TRUE:
        fprintf(f, "jmp ");
        break;
    
    default:
        fprintf(stderr, "WTF are you doing ? How is it possible to fail this check ???\n");
        exit(1);
        break;
    }

    if (jump->jump_resolved)
    {
        visit_condition(v, jump->condition, f);
    }
    else
    {
        fprintf(stderr, "ASMJump address not resolved\n");
        exit(1);
    }

    fprintf(f, " then\n\t");
    visit_jmp_addr(v, jump->true_branch, f);
    if (jump->condition->kind != COND_TRUE)
    {
        fprintf(f, "\nelse\n\t");
        visit_jmp_addr(v, jump->false_branch, f);
    }
}

void visit_call(Visitor* v, ASMCall* call, FILE* f)
{
    fprintf(f, "%s", call->name);
}
