#ifndef H_PRETTY_PRINTER
#define H_PRETTY_PRINTER

#include "CFG.h"

typedef struct Visitor Visitor;

typedef struct Visitor {
    void (*visit_program)(Visitor*, Asm*, FILE*);
    void (*visit_instruction)(Visitor*, Instruction*, FILE*);

    void (*visit_dst)(Visitor*, ASMDst*, FILE*);
    void (*visit_src)(Visitor*, ASMSrc*, FILE*);
    
    void (*visit_op)(Visitor*, ASMOperation*, FILE*);
    void (*visit_load)(Visitor* v, ASMLoad* load, FILE*);
    void (*visit_store)(Visitor* v, ASMStore* load, FILE*);
    void (*visit_cmp)(Visitor* v, ASMComparison*, FILE*);

    void (*visit_jmp_addr)(Visitor* v, Instruction* instruction, FILE*);
    void (*visit_condition)(Visitor* v, ASMCondition* condition, FILE*);

    void (*visit_jif)(Visitor*, ASMJump*, FILE*);
    void (*visit_ja)(Visitor*, ASMJump*, FILE*);
    void (*visit_je)(Visitor*, ASMJump*, FILE*);
    void (*visit_jl)(Visitor*, ASMJump*, FILE*);

    void (*visit_call)(Visitor* v, ASMCall* call, FILE*);
} Visitor;

void visit_program(Visitor* v, Asm* program, FILE* f);
void visit_instruction(Visitor* v, Instruction* instruction, FILE* f);

void visit_src(Visitor* v, ASMSrc* src, FILE* f);
void visit_dst(Visitor* v, ASMDst* dst, FILE* f);

void visit_op(Visitor* v, ASMOperation* operation, FILE* f);
void visit_load(Visitor* v, ASMLoad* load, FILE* f);
void visit_store(Visitor* v, ASMStore* load, FILE* f);
void visit_cmp(Visitor* v, ASMComparison* comparison, FILE* f);

void visit_jmp_addr(Visitor* v, Instruction* instruction, FILE* f);
void visit_condition(Visitor* v, ASMCondition* condition, FILE* f);

void visit_jmp(Visitor* v, ASMJump* jump, FILE* f);

void visit_call(Visitor* v, ASMCall* call, FILE* f);

#endif