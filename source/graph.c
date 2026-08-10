#include <stdlib.h>
#include <stdio.h>
#include <graphviz/gvc.h>

#include "graph.h"
#include "utils.h"
#include "AST.h"

void graph_to_graphviz(Cfg *cfg, const char *dot_file, const char *png_file) {
    if (cfg == NULL)
        return;

    FILE *f = fopen(dot_file, "w");
    if (f == NULL) {
        fprintf(stderr, "fopen() failed\n");
        exit(1);
    }

    fprintf(f, "digraph G {\n");
    fprintf(f, "    rankdir=TB;\n");
    fprintf(f, "    node [shape=box];\n\n");

    /* nodes declaration */
    for (int i = 0; i < cfg->node_idx; i++) {
        Node* b = &cfg->nodes[i];
        if (b != NULL) {
            fprintf(f, "    n%d [label=node_%d];\n", b->instruction_start->offset, b->instruction_start->offset);
        }
    }

    fprintf(f, "\n");

    /* edges declaration */
    for (int i = 0; i < cfg->edge_idx; i++) {
        Edge *e = &cfg->edges[i];
        if (e != NULL && e->start != NULL && e->target != NULL) {
            fprintf(f, "    n%d -> n%d", e->start->instruction_start->offset, e->target->instruction_start->offset);

            if (e->visited)
                fprintf(f, " [color=red, penwidth=2]");

            fprintf(f, ";\n");
        }
    }

    fprintf(f, "}\n");
    fclose(f);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o %s", dot_file, png_file);
    system(cmd);

    printf("Graphviz written in %s\n", dot_file);
    printf("Image generated in %s\n", png_file);
}

int compare_ints(const void* a, const void* b)
{
    return *(const int *) a - *(const int *) b;
}

void get_info_cfg(Asm* program, int** list_node_indices, int* nb_basic_nodes, int* nb_edges)
{
    (*nb_edges) = 0;
    (*nb_basic_nodes) = 0;

    (*list_node_indices)[(*nb_edges)++] = 0;
    
    for (int instruction_index = 0; instruction_index < program->nb_instructions; instruction_index++)
    {
        Instruction instruction = program->instructions[instruction_index];
        
        if (instruction.kind == INSTR_JMP)
        {
            (*list_node_indices)[(*nb_edges)++] = instruction.jump->true_branch->offset;
            (*list_node_indices)[(*nb_edges)++] = instruction.jump->false_branch->offset;
        }
        else
        {
            (*list_node_indices)[(*nb_edges)++] = instruction.offset + 1;
        }
    }
    
    // sort
    printf("before\n");
    qsort(*list_node_indices, *nb_edges, sizeof(int), compare_ints);
    printf("after\n");
    
    // remove duplicate
    for (int i = 0; i < *nb_edges; i++) {
        if (i == 0 || (*list_node_indices)[i] != (*list_node_indices)[i - 1])
        {
            (*list_node_indices)[(*nb_basic_nodes)++] = (*list_node_indices)[i];
        }
    }
    *list_node_indices = realloc(*list_node_indices, (*nb_basic_nodes) * sizeof(int));

    // compute the number of edges
    int nb_edges_final = 0;
    for (int i = 0; i < *nb_basic_nodes - 1; i++)
    {
        if (program->instructions[(*list_node_indices)[i + 1] - 1].kind == INSTR_JMP)
        {
            nb_edges_final++;
        }
        nb_edges_final++;
    }

    *nb_edges = nb_edges_final;
}

Cfg* init_graph(int nb_nodes, int nb_edges)
{
    Cfg* cfg = malloc(sizeof(Cfg));
    if (cfg == NULL)
    {
        fprintf(stderr, "malloc() Cfg failed\n");
        exit(1);
    }
    
    cfg->edge_idx = 0;
    cfg->node_idx = 0;
    cfg->edges = calloc(nb_edges, sizeof(Edge));
    if (cfg->edges == NULL)
    {
        fprintf(stderr, "malloc() Edge failed\n");
        exit(1);
    }
    cfg->nodes = calloc(nb_nodes, sizeof(Node));
    if (cfg->nodes == NULL)
    {
        fprintf(stderr, "malloc() Node failed\n");
        exit(1);
    }

    cfg->nb_nodes = nb_nodes;
    cfg->nb_edges = nb_edges;

    return cfg;
}

void add_parent(Node* current_node, Node* parent)
{
    Node** parents = current_node->parents;
    if (current_node->parent_idx > current_node->nb_parents)
    {
        parents = realloc(parents, sizeof(parents) + sizeof(Node*));
        (current_node->nb_parents)++;
    }
    
    current_node->parents[current_node->parent_idx++] = parent;
}

void add_child(Node* current_node, Node* child)
{
    Node** children = current_node->children;
    if (current_node->child_idx > current_node->nb_children)
    {
        children = realloc(children, sizeof(children) + sizeof(Node*));
        (current_node->nb_children)++;
    }
    
    current_node->children[current_node->child_idx++] = child;
}

void add_edge(Cfg* cfg, Node* start, Node* target)
{
    // sould not be taken
    if (cfg->edge_idx > cfg->nb_edges)
    {
        cfg->edges = realloc(cfg->edges, sizeof(cfg->edges) + sizeof(Edge*));
        (cfg->nb_edges)++;
    }

    cfg->edges[cfg->edge_idx].start = start;
    cfg->edges[cfg->edge_idx].target = target;
    (cfg->edge_idx)++;
}

// nb_instructions
void update_node(Cfg* cfg, Node* current_node, Instruction* next_node_start_instruction)
{
    int next_addr = next_node_start_instruction->offset;
    int next_node_addr;

    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        Node* next_node = &cfg->nodes[i];
        next_node_addr = next_node->instruction_start->offset;

        // found the next node
        if (next_node_addr == next_addr)
        {
            // update its children
            add_child(current_node, next_node);

            // update the next node's parents
            add_parent(next_node, current_node);

            // update edges
            add_edge(cfg, current_node, next_node);
            break;
        }
    }
}

void create_nodes(Asm* program, Cfg* cfg, int* list_node_indices)
{
    for (int node_index = 0; node_index < cfg->nb_nodes; node_index++)
    {
        printf("Creating node n°%d\n", node_index);
        Node* current_node = &cfg->nodes[node_index];

        current_node->instruction_start = &program->instructions[list_node_indices[node_index]];
        current_node->nb_instructions = list_node_indices[node_index + 1] - list_node_indices[node_index - 1];

        Instruction* last_instruction = &program->instructions[list_node_indices[node_index + 1] - 1];

        if (last_instruction->kind == INSTR_JMP)
        {
            update_node(cfg, current_node, last_instruction->jump->true_branch);
            update_node(cfg, current_node, last_instruction->jump->false_branch);
        }
        else
        {
            update_node(cfg, current_node, &program->instructions[list_node_indices[node_index]]);
        }
    }
}

Cfg* asm_to_cfg(Asm* program) {
    int* list_node_indices;
    int nb_nodes;
    int nb_edges;

    get_info_cfg(program, &list_node_indices, &nb_nodes, &nb_edges);
    printf("blocks generated\n");
    exit(0);
    Cfg* cfg = init_graph(nb_nodes, nb_edges);

    create_nodes(program, cfg, list_node_indices);

    return cfg;
}