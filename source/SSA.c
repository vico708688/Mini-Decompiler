#include <stdlib.h>
#include <stdio.h>

#include "SSA.h"
#include "CFG.h"

/**
 * 1. Identify all nodes where a var is assign (set S)
 * 2. Compute the dominance frontier of each of these nodes (DF(Node_1), DF(Node_2), ...)
 * 3. Take the union of these dominance frontiers to obtain DF(S), which is the initial DF called DF_1(S)
 * 4. We iteratively compute the next DF by calculating DF_i+1(S) = DF(S U DF_i(S)) until it converges (no nodes are added)
 *  -> iteratively add nodes that are in the frontiers of the current frontier.
 */

void cfg_to_SSA_form(Cfg* cfg)
{

}