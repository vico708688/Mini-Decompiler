#ifndef H_GRAPH_UTILS
#define H_GRAPH_UTILS

#include "backend/CFG.h"

typedef struct List
{
    Node*** nodes;
    int index;
} List;

void compute_dominance_frontier(Cfg* cfg);
void compute_idom(Cfg* cfg);
Node* intersect(Node* b1, Node* b2);
void reverse_post_order(Cfg* cfg, Node*** list_nodes);
void DFS(Cfg* cfg, Node* node, List* list_nodes);

#endif