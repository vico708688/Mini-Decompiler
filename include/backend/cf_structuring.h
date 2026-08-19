#ifndef H_CF_STRUCTURING
#define H_CF_STRUCTURING

#include "backend/CFG.h"

bool cfg_has_unvisited_edges(Cfg* cfg);
Cfg* graph_slice(Cfg* cfg, Node* source, Node* sink);
 
#endif