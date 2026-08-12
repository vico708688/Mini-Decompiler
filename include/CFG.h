#ifndef H_CFG
#define H_CFG

#include <stdbool.h>
#include "parser.h"

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

    Node** children; // linked list (0-2 elements) -> change in a union of jump or simple node ?
    int nb_children;
    int child_idx;
    
    Node** parents; // linked list (0-n elements)
    int nb_parents;
    int parent_idx;

    bool visited;
} Node;

typedef struct Edge {
    Node* start;
    Node* target;

    bool visited;
} Edge;

#endif