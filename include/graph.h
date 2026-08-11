#ifndef H_GRAPH
#define H_GRAPH

#include <stdbool.h>
#include "AST.h"

typedef struct Node Node;
typedef struct Edge Edge;

/**
 * Graph constraints:
 *  - medium to big size (~100-1000 nodes)
 *  - use of DFS-based algorithms
 *  - search for edges
 *  - dynamic graph (modifiable)
 * 
 * Graph specifications:
 *  - directed
 *  - unweighted
 *  - non-simple
 *  - sparse
 *  - cyclic
 *  - topological (not define by geometry)
 *  - explicit (every edge exist at once)
 *  - unlabeled
 */
typedef struct Cfg {
    Node* nodes;
    int nb_nodes;
    int node_idx;
    
    Edge* edges;
    int nb_edges;
    int edge_idx;
} Cfg;

typedef struct Node {
    Instruction* start_instruction; // instructions contiguous in memory
    int nb_instructions;

    Node** children; // liked list (0-2 elements)
    int nb_children;
    int child_idx;
    
    Node** parents; // liked list (0-n elements)
    int nb_parents;
    int parent_idx;

    bool visited;
} Node;

typedef struct Edge {
    Node* start;
    Node* target;
    int visited;
} Edge;


// ------------------------------ FUNCTION PROTOTYPES ---------------------------------
Cfg* asm_to_cfg(Asm* program);

Cfg* create_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges);
// void create_nodes(Asm* program, Cfg* cfg, int* list_node_indices);
// void update_node(Cfg* cfg, Node* current_node, Instruction* next_node_start_instruction);
void add_child(Node* current_node, Node* child);
void add_parent(Node* current_node, Node* parent);
void add_edge(Cfg* cfg, Node* start, Node* target);

Cfg* initialize_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges);

void get_info_graph(Asm* program, int** basic_node_numbers, int* nb_basic_nodes, int* nb_edges);
int compare_ints(const void* a, const void* b);

void graph_to_graphviz(Cfg *graph, const char *dot_file, const char *png_file);

#endif