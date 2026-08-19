#include <stdio.h>
#include <stdlib.h>

#include "backend/CFG.h"
#include "backend/pretty_printer.h"

void graph_to_graphviz(Cfg *cfg, const char *dot_file, const char *png_file) {
    if (cfg == NULL)
        return;
    
    Visitor v;

    FILE *f = fopen(dot_file, "w");
    if (f == NULL) {
        fprintf(stderr, "fopen() failed\n");
        exit(1);
    }

    fprintf(f, "digraph G {\n");
    fprintf(f, "    rankdir=TB;\n");
    fprintf(f, "    node [shape=box];\n\n");

    int instr_count = 0;

    for (int i = 0; i < cfg->nb_nodes; i++) {
        Node* b = &cfg->nodes[i];
        if (b != NULL) {
            fprintf(f, "    node_%d -> {", b->start_instruction->offset);
            for (int successor_idx = 0; successor_idx < b->nb_successors; successor_idx++)
            {
                fprintf(f, "node_%d ", b->successors[successor_idx]->start_instruction->offset);
            }
            fprintf(f, "}\n");

            fprintf(f, "node_%d [label=\"", b->start_instruction->offset);
            for (int instr_idx = 0; instr_idx < b->nb_instructions; instr_idx++)
            {
                Instruction* instr = (b->start_instruction + instr_idx);
                fprintf(f, "%d | ", instr_count);
                visit_instruction(&v, instr, f);
                instr_count++;
            }
            fprintf(f, "\"]\n");
            
        }
    }

    fprintf(f, "}\n");
    fclose(f);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o %s && xdg-open %s", dot_file, png_file, png_file);
    system(cmd);

    printf("Graphviz written in %s\n", dot_file);
    printf("Image generated in %s\n", png_file);
}