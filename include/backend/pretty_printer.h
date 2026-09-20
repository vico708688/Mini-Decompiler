#ifndef H_PRETTY_PRINTER
#define H_PRETTY_PRINTER

#include "backend/CFG.h"

typedef struct Visitor Visitor;

typedef struct Visitor
{
    void (*visit_program)(Visitor *, IR *, FILE *);
    void (*visit_ir_instruction)(Visitor *, AtomicInstr *, FILE *);

    // void (*visit_op)(Visitor*, Operation*, FILE*);
    void (*visit_load)(Visitor *v, Load *load, FILE *);
    void (*visit_store)(Visitor *v, Store *load, FILE *);
    // void (*visit_cmp)(Visitor* v, Comparison*, FILE*);

    void (*visit_jmp_addr)(Visitor *v, AtomicInstr *instruction, FILE *);

    void (*visit_jif)(Visitor *, Jmp *, FILE *);
    void (*visit_ja)(Visitor *, Jmp *, FILE *);
    void (*visit_je)(Visitor *, Jmp *, FILE *);
    void (*visit_jl)(Visitor *, Jmp *, FILE *);

    void (*visit_call)(Visitor *v, Call *call, FILE *);
} Visitor;

void visit_program(Visitor *v, IR *program_IR, FILE *f);
void visit_instruction(Visitor *v, AtomicInstr *instruction, FILE *f);

void visit_copy(Visitor *v, Copy *copy, FILE *f);
void visit_load(Visitor *v, Load *load, FILE *f);
void visit_store(Visitor *v, Store *load, FILE *f);
void visit_cmp(Visitor* v, Equal* equal, FILE* f);

void visit_jmp_addr(Visitor *v, AtomicInstr *instruction, FILE *f);

void visit_jmp(Visitor *v, Jmp *jump, FILE *f);
void visit_condjmp(Visitor *v, CondJmp *jump, FILE *f);
void visit_id(Visitor *v, ValueId *valueId, FILE *f);

void visit_call(Visitor *v, Call *call, FILE *f);

#endif