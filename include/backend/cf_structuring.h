#ifndef H_CF_STRUCTURING
#define H_CF_STRUCTURING

#include "backend/CFG.h"

bool cfg_has_unvisited_edges(CFG* cfg);
CFG* graph_slice(CFG* cfg, Node* source, Node* sink);
 
#endif