#ifndef H_PRINTCFG
#define H_PRINTCFG

#include "backend/CFG.h"

void graph_to_graphviz(CFG *graph, const char *dot_file, const char *png_file);

#endif