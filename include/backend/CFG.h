#ifndef H_CFG
#define H_CFG

#include <stdbool.h>
#include "frontend/parser.h"

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
    Node* header;

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

    Node** successors; // linked list (0-2 elements) -> change in a union of jump or simple node ?
    int nb_successors;
    int successor_idx;
    
    Node** predecessors; // linked list (0-n elements)
    int nb_predecessors;
    int predecessor_idx;

    Node* dominator;
    int number;
    bool processed;

    Node** dominator_frontier;
    int nb_dominators;
    int dominator_idx;

    bool visited;
} Node;

typedef struct Edge {
    Node* start;
    Node* target;

    int number;

    bool visited;
} Edge;

#endif