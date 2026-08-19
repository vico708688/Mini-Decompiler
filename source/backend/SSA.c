#include <stdlib.h>
#include <stdio.h>

#include "backend/SSA.h"
#include "backend/CFG.h"
#include "utils/graph_utils.h"

/**
 * Lazy SSA : file:///home/victor/Downloads/978-3-642-37051-9.pdf
 */
void cfg_to_SSA_form(Cfg* cfg)
{
    compute_dominance_frontier(cfg);
}


/**
 * Other approach:
 * 
 * 1. Identify all nodes where a var is assign (set S)
 * 2. Compute the dominance frontier of each of these nodes (DF(Node_1), DF(Node_2), ...)
 * 3. Take the union of these dominance frontiers to obtain DF(S), which is the initial DF called DF_1(S)
 * 4. We iteratively compute the next DF by calculating DF_i+1(S) = DF(S U DF_i(S)) until it converges (no nodes are added)
 *  -> iteratively add nodes that are in the frontiers of the current frontier.
 */