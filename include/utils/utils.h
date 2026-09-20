#ifndef H_UTILS
#define H_UTILS

#include <regex.h>
#include "backend/IR.h"
#include "backend/CFG.h"
#include "backend/graph.h"

typedef struct Stack
{
    Node** nodes;
    int nb_nodes;
    int node_idx;
} Stack;

void free_ir(IR* program);
// void free_instruction(Instruction* instruction);
// void free_jump(Jump* jump);
// void free_call(Call* call);
// void free_operation(Operation* operation);
// void free_comparison(Comparison* comparison);
// void free_store(Store* store);
// void free_load(Load* load);
// void free_dst(Dst* dst);
// void free_src(Src* src);
// void free_flag(Flag* flag);
// void free_mem(Mem* mem);
// void free_reg(Reg* reg);
// void free_int(Int* integer);
// void free_operand(Operand* operand);
// void free_condition(Condition* condition);

void free_cfg(CFG* cfg);
void free_node(Node* node);
void free_edge(Edge* edge);

char* extract_sub_string(char** string, int len);

char* read_asm_file(char* path);

#endif