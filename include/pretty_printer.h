#ifndef H_PRETTY_PRINTER
#define H_PRETTY_PRINTER

#include "CFG.h"

typedef struct Visitor Visitor;

typedef struct Visitor {
    void (*visit_program)(Visitor*, Asm*);
    void (*visit_instruction)(Visitor*, Instruction*);

    void (*visit_dst)(Visitor*, Dst*);
    void (*visit_src)(Visitor*, Src*);
    
    void (*visit_op)(Visitor*, Operation*);
    void (*visit_load)(Visitor* v, Load* load);
    void (*visit_store)(Visitor* v, Store* load);
    void (*visit_cmp)(Visitor* v, Comparison*);

    void (*visit_jmp_addr)(Visitor* v, Instruction* instruction);
    void (*visit_condition)(Visitor* v, Condition* condition);

    void (*visit_jif)(Visitor*, Jump*);
    void (*visit_ja)(Visitor*, Jump*);
    void (*visit_je)(Visitor*, Jump*);
    void (*visit_jl)(Visitor*, Jump*);

    void (*visit_call)(Visitor* v, Call* call);
} Visitor;

void visit_program(Visitor* v, Asm* program);
void visit_instruction(Visitor* v, Instruction* instruction);

void visit_src(Visitor* v, Src* src);
void visit_dst(Visitor* v, Dst* dst);

void visit_op(Visitor* v, Operation* operation);
void visit_load(Visitor* v, Load* load);
void visit_store(Visitor* v, Store* load);
void visit_cmp(Visitor* v, Comparison* comparison);

void visit_jmp_addr(Visitor* v, Instruction* instruction);
void visit_condition(Visitor* v, Condition* condition);

void visit_jmp(Visitor* v, Jump* jump);

void visit_call(Visitor* v, Call* call);

#endif