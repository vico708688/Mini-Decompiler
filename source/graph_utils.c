#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "graph_utils.h"
#include "pretty_printer.h"
#include "CFG.h"

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
            for (int child_idx = 0; child_idx < b->nb_children; child_idx++)
            {
                fprintf(f, "node_%d ", b->children[child_idx]->start_instruction->offset);
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

void DFS(Cfg* cfg, Node* node, List* list_nodes) {
    node->visited = true;
    for (int i = 0; i < node->nb_children; i++) {
        Node* next_node = node->children[i];
        if (next_node->visited == false) {
            DFS(cfg, next_node, list_nodes);
            next_node->post_order_number = list_nodes->index;
            (*list_nodes->nodes)[(list_nodes->index)++] = next_node;
        }
    }
}

/**
 * Also called topological sort
 */
void reverse_post_order(Cfg* cfg, Node*** list_nodes)
{
    Node** temp = malloc(cfg->nb_nodes * sizeof(Node*));
    List list;
    list.index = 0;
    list.nodes = &temp;
    /* Depth First Search */
    DFS(cfg, cfg->header, &list);
    temp[cfg->nb_nodes - 1] = cfg->header;
    temp[cfg->nb_nodes - 1]->post_order_number = cfg->nb_nodes - 1;
    
    /* reset each node to unvisited state */
    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        cfg->nodes[i].visited = false;
    }

    /* invert the list */
    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        (*list_nodes)[cfg->nb_nodes - 1 - i] = temp[i];
    }

    free(temp);
}

Node* intersect(Node* b1, Node* b2)
{
    Node* finger1 = b1;
    Node* finger2 = b2;
    
    while (finger1->post_order_number != finger2->post_order_number)
    {
        printf("in\n");
        while (finger1->post_order_number < finger2->post_order_number)
        {
            finger1 = finger1->dominator;
            printf("dominator: %d\n", finger1->start_instruction->offset);
        }
        while (finger1->post_order_number > finger2->post_order_number)
        {
            finger2 = finger2->dominator;
            printf("dominator: %d\n", finger2->start_instruction->offset);
        }
    }
    
    return finger1;
}

/**
 * Iterative dominator algorithm: https://c9x.me/compile/bib/quickdom.pdf
 * 
 * idom(b): immediate dominator of b, "dominator closest to b"
 * 
 */
void dominator_tree(Cfg* cfg)
{
    /* reverse postorder list computation */
    Node** list_post_order = malloc(cfg->nb_nodes * sizeof(Node*));
    reverse_post_order(cfg, &list_post_order);

    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        printf("node %d, nb = %d\n", list_post_order[i]->start_instruction->offset, list_post_order[i]->post_order_number);
    }
    printf("\n");
    exit(1);
    cfg->header->dominator = cfg->header;
    
    bool changed = true;
    while (changed)
    {
        changed = false;
        /* for all nodes b in reverse postorder except for start_node */
        for (int i = 1; i < cfg->nb_nodes; i++)
        {
            Node* b = list_post_order[i];
            printf("current node = %d\n", b->start_instruction->offset);
            Node* new_idom;
            /* first processed predecessor of b */
            // not sure
            for (int predecessor_idx = 0; predecessor_idx < b->nb_parents; predecessor_idx++)
            {
                Node* predecessor = b->parents[predecessor_idx];
                if (predecessor->processed)
                {
                    new_idom = b->parents[i];
                    printf("immediate dominator (1st cycle): %d\n", new_idom->start_instruction->offset);
                }
            }
            
            for (int predecessor_idx = 0; predecessor_idx < b->nb_parents; predecessor_idx++)
            {
                Node* p = b->parents[predecessor_idx];
                if (p->dominator != NULL)
                {
                    new_idom = intersect(p, new_idom);
                }
            }

            if (b->dominator != new_idom)
            {
                printf("dominator of node: %d: %d\n", b->start_instruction->offset, new_idom->start_instruction->offset);
                b->dominator = new_idom;
                b->processed = true;
                changed = true;
            }
        }
        exit(1);
    }
    
    free(list_post_order);
}