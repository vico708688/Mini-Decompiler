#ifndef H_GRAPH
#define H_GRAPH

#include <stdbool.h>
#include "backend/CFG.h"
#include "backend/IR.h"

// ------------------------------ FUNCTION PROTOTYPES ---------------------------------
CFG* ir_to_cfg(IR* program_IR);

CFG* create_graph(IR* program_IR, int* list_node_indices, int nb_nodes, int nb_edges);
AtomicInstr* get_instruction_from_node(Node* node, int instruction_offset);
Node* find_node_from_start_instruction_offset(CFG* cfg, int start_instruction_offset);
void update_neighbour_nodes(CFG* cfg, Node* current_node, int next_node_start_instruction_offset);
Node* generate_subgraph_from_instruction_offset(CFG* cfg, int instruction_offset, int last_instruction_last_node);
void create_edge(CFG* cfg, Node* start, Node* target);
CFG* initialize_empty_graph(int nb_nodes, int nb_edges);
CFG* initialize_graph(IR* program_IR, int* list_node_indices, int nb_nodes, int nb_edges);

void get_info_graph(IR* program_IR, int** basic_node_numbers, int* nb_basic_nodes, int* nb_edges);
int compare_ints(const void* a, const void* b);

#endif