#ifndef H_SSA
#define H_SSA

#include "backend/IR.h"
#include "backend/CFG.h"

void cfg_to_ssa(CFG* cfg);

#endif