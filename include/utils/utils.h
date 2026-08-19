#ifndef H_UTILS
#define H_UTILS

#include <regex.h>
#include "frontend/token.h"
#include "backend/CFG.h"
#include "backend/graph.h"

typedef struct Stack
{
    Node** nodes;
    int nb_nodes;
    int node_idx;
} Stack;

typedef struct
{
    regex_t integer;
    regex_t charac;
} regexList;

void free_asm(Asm* program);
void free_instruction(Instruction* instruction);
void free_jump(ASMJump* jump);
void free_call(ASMCall* call);
void free_operation(ASMOperation* operation);
void free_comparison(ASMComparison* comparison);
void free_store(ASMStore* store);
void free_load(ASMLoad* load);
void free_dst(ASMDst* dst);
void free_src(ASMSrc* src);
void free_flag(ASMFlag* flag);
void free_mem(ASMMem* mem);
void free_reg(ASMReg* reg);
void free_int(ASMInt* integer);
void free_operand(ASMOperand* operand);
void free_condition(ASMCondition* condition);

void free_cfg(Cfg* cfg);
void free_node(Node* node);
void free_edge(Edge* edge);

void free_tokens(TokenList* tokenList);
void free_regexes(regexList* regexes);

void init_regexes(regexList* regexes);
TokenList *init_token_list(char** text);

char* extract_sub_string(char** string, int len);

char* read_asm_file(char* path);

#endif