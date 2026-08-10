#ifndef H_GRAPH
#define H_GRAPH

#include "AST.h"

typedef struct Node Node;

typedef struct Node {
    Instruction* instruction_start; // instructions contiguous in memory
    int nb_instructions;

    Node** children; // not contiguous
    int nb_children;
    int child_idx;
    
    Node** parents; // not contiguous
    int nb_parents;
    int parent_idx;
} Node;

typedef struct Edge {
    Node* start;
    Node* target;
    int visited;
} Edge;

typedef struct Cfg {
    Node* nodes;
    int nb_nodes;
    int node_idx;
    
    Edge* edges;
    int nb_edges;
    int edge_idx;
} Cfg;


// ------------------------------ FUNCTION PROTOTYPES ---------------------------------
Cfg* asm_to_cfg(Asm* program);

void create_nodes(Asm* program, Cfg* cfg, int* list_node_indices);
void update_node(Cfg* cfg, Node* current_node, Instruction* next_node_start_instruction);
void add_child(Node* current_node, Node* child);
void add_parent(Node* current_node, Node* parent);
void add_edge(Cfg* cfg, Node* start, Node* target);

Cfg* init_graph(int nb_nodes, int nb_edges);

void get_info_cfg(Asm* program, int** basic_node_numbers, int* nb_basic_nodes, int* nb_edges);
int compare_ints(const void* a, const void* b);

void graph_to_graphviz(Cfg *graph, const char *dot_file, const char *png_file);

#endif