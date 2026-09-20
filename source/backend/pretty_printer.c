#include <stdio.h>
#include <stdlib.h>

#include "backend/CFG.h"
#include "backend/pretty_printer.h"

void visit_program(Visitor *v, IR *ir, FILE *f)
{
    for (int i = 0; i < ir->index; i++)
    {
        visit_instruction(v, &(ir->atomic_instructions[i]), f);
    }
}

void visit_instruction(Visitor *v, AtomicInstr *instruction, FILE *f)
{
    switch (instruction->kind)
    {
    case ATM_ADD:
        fprintf(f, "ADD(");
        visit_id(v, &instruction->add.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->add.input1, f);
        fprintf(f, ")");
        break;
    case ATM_SUB:
        fprintf(f, "SUB(");
        visit_id(v, &instruction->sub.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->sub.input1, f);
        fprintf(f, ")");
        break;
    case ATM_XOR:
        fprintf(f, "XOR(");
        visit_id(v, &instruction->xor.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->xor.input1, f);
        fprintf(f, ")");
        break;
    case ATM_OR:
        fprintf(f, "OR(");
        visit_id(v, &instruction->or.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->or.input1, f);
        fprintf(f, ")");
        break;
    case ATM_REM:
        fprintf(f, "REM(");
        visit_id(v, &instruction->rem.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->rem.input1, f);
        fprintf(f, ")");
        break;
    case ATM_AND:
        fprintf(f, "AND(");
        visit_id(v, &instruction->and.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->and.input1, f);
        fprintf(f, ")");
        break;
    case ATM_SHR:
        fprintf(f, "SHR(");
        visit_id(v, &instruction->shr.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->shr.input1, f);
        fprintf(f, ")");
        break;
    case ATM_SHL:
        fprintf(f, "SHL(");
        visit_id(v, &instruction->shl.input0, f);
        fprintf(f, ", ");
        visit_id(v, &instruction->shl.input1, f);
        fprintf(f, ")");
        break;

    case ATM_COPY:
        visit_copy(v, &instruction->copy, f);
        break;

    case ATM_LOAD:
        visit_load(v, &instruction->load, f);
        break;

    case ATM_STORE:
        visit_store(v, &instruction->store, f);
        break;

    case ATM_EQUAL:
        visit_cmp(v, &instruction->equal, f);
        break;

    case ATM_JMP:
        visit_jmp(v, &instruction->jmp, f);
        break;

    case ATM_CONDJMP:
        visit_condjmp(v, &instruction->cond_jmp, f);
        break;

    case ATM_CALL:
        visit_call(v, &instruction->call, f);
        break;

    default:
        fprintf(f, "Instruction cannot be printed\n");
        // exit(0);
        break;
    }
    fprintf(f, "\n");
}

void visit_id(Visitor *v, ValueId *valueId, FILE *f)
{
    if (valueId->address.space == SPACE_CONST)
    {
        fprintf(f, "CONST_%d", valueId->address.offset);
    }
    else if (valueId->address.space == SPACE_REG)
    {
        fprintf(f, "REG:[%d]", valueId->address.offset);
    }
    else
    {
        fprintf(f, "MEM:[%d]", valueId->address.offset);
    }
}

void visit_copy(Visitor *v, Copy *copy, FILE *f)
{
    fprintf(f, "COPY(");
    visit_id(v, &copy->destination, f);
    fprintf(f, ", ");
    visit_id(v, &copy->source, f);
    fprintf(f, ")");
}

void visit_load(Visitor *v, Load *load, FILE *f)
{
    fprintf(f, "LOAD(");
    visit_id(v, &load->destination, f);
    fprintf(f, ", MEM:[");
    visit_id(v, &load->pointer, f);
    fprintf(f, "])");
}

void visit_store(Visitor *v, Store *store, FILE *f)
{
    fprintf(f, "STORE(");
    fprintf(f, "MEM:[");
    visit_id(v, &store->pointer, f);
    fprintf(f, "], ");
    visit_id(v, &store->source, f);
    fprintf(f, ")");
}

void visit_jmp_addr(Visitor *v, AtomicInstr *instruction, FILE *f)
{
    fprintf(f, "%d", instruction->offset);
}

void visit_condjmp(Visitor *v, CondJmp *jump, FILE *f)
{
    fprintf(f, "CONDJMP(");
    visit_id(v, &jump->jmp_addr, f);
    fprintf(f, ")");
}

void visit_jmp(Visitor *v, Jmp *jump, FILE *f)
{
    fprintf(f, "JMP(");
    visit_id(v, &jump->jmp_addr, f);
    fprintf(f, ")");
}

void visit_cmp(Visitor *v, Equal *equal, FILE *f)
{
    fprintf(f, "EQUAL(");
    visit_id(v, &equal->input0, f);
    fprintf(f, ", ");
    visit_id(v, &equal->input1, f);
    fprintf(f, ")");
}

void visit_call(Visitor *v, Call *call, FILE *f)
{
    fprintf(f, "%s", call->name);
}