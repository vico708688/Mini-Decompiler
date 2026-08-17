#ifndef H_GRAPH
#define H_GRAPH

#include <stdbool.h>
#include "CFG.h"

// ------------------------------ FUNCTION PROTOTYPES ---------------------------------
Cfg* asm_to_cfg(Asm* program);

Cfg* create_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges);
Instruction* get_instruction_from_node(Node* node, int instruction_offset);
Node* find_node_from_start_instruction_offset(Cfg* cfg, int start_instruction_offset);
void update_neighbour_nodes(Cfg* cfg, Node* current_node, int next_node_start_instruction_offset);
Node* generate_subgraph_from_node(Cfg* cfg, int instruction_offset, int last_instruction_last_node);
void create_edge(Cfg* cfg, Node* start, Node* target);
Cfg* initialize_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges);

void get_info_graph(Asm* program, int** basic_node_numbers, int* nb_basic_nodes, int* nb_edges);
int compare_ints(const void* a, const void* b);

#endif