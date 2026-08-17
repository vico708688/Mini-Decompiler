#ifndef H_GRAPH_UTILS
#define H_GRAPH_UTILS

#include "CFG.h"

typedef struct List
{
    Node*** nodes;
    int index;
} List;

void graph_to_graphviz(Cfg *graph, const char *dot_file, const char *png_file);

void dominator_tree(Cfg* cfg);

#endif