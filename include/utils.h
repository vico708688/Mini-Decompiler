#ifndef H_UTILS
#define H_UTILS

#include <regex.h>
#include "token.h"
#include "CFG.h"
#include "graph.h"

typedef struct {
    regex_t integer;
    regex_t charac;
} regexList;

void free_asm(Asm* program);
void free_instruction(Instruction* instruction);
void free_jump(Jump* jump);
void free_call(Call* call);
void free_operation(Operation* operation);
void free_comparison(Comparison* comparison);
void free_store(Store* store);
void free_load(Load* load);
void free_dst(Dst* dst);
void free_src(Src* src);
void free_flag(ExprFlag* flag);
void free_mem(ExprMem* mem);
void free_reg(ExprReg* reg);
void free_int(ExprInt* integer);
void free_operand(Operand* operand);
void free_condition(Condition* condition);

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