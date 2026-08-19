#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "backend/cf_structuring.h"
#include "backend/CFG.h"
#include "utils/graph_utils.h"
#include "backend/graph.h"
#include "utils/utils.h"

bool cfg_has_unvisited_edges(Cfg* cfg)
{
    for (int i = 0; i < cfg->nb_edges; i++)
    {
        if (cfg->edges[i].visited)
        {
            return true;
        }
    }
    return false;
}

// Cfg* graph_slice(Cfg* cfg, Node* source, Node* sink)
// {
//     /* At least 2 nodes (source and sink) and 1 edge */
//     int nb_nodes = 2;
//     int nb_edges = 1;
//     Cfg* slice = initialize_empty_graph(nb_nodes, nb_edges);

//     Stack stack;
//     stack.nb_nodes = nb_nodes;
//     stack.node_idx = 0;
//     stack.nodes = malloc(nb_nodes * sizeof(Node*));

//     stack.nodes[(stack.node_idx)++] = source;

//     while (cfg_has_unvisited_edges(cfg))
//     {
//         Edge* edge = 
//     }
    


//     return slice;
// }