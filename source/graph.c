#include <stdlib.h>
#include <stdio.h>
#include <graphviz/gvc.h>

#include "graph.h"
#include "utils.h"
#include "pretty_printer.h"
#include "AST.h"

int START_NODE = 0;

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
            fprintf(f, "    n%d [label=node_%d];\n", b->start_instruction->offset, b->start_instruction->offset);
        }
    }

    fprintf(f, "\n");

    /* edges declaration */
    for (int i = 0; i < cfg->edge_idx; i++) {
        Edge *e = &cfg->edges[i];
        if (e != NULL && e->start != NULL && e->target != NULL) {
            fprintf(f, "    n%d -> n%d", e->start->start_instruction->offset, e->target->start_instruction->offset);

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

void get_info_graph(Asm* program, int** list_node_indices, int* nb_basic_nodes, int* nb_edges)
{
    (*nb_edges) = 0;
    (*nb_basic_nodes) = 0;
    
    (*list_node_indices) = malloc(program->nb_instructions * sizeof(int));
    if ((*list_node_indices) == NULL) {
        fprintf(stderr, "malloc() list_node_instruction failed\n");
        exit(1);
    }
    
    (*list_node_indices)[(*nb_edges)++] = 0;
    
    for (int instruction_index = 0; instruction_index < program->nb_instructions; instruction_index++)
    {
        Instruction instruction = program->instructions[instruction_index];
        
        if (instruction.kind == INSTR_JMP)
        {
            (*list_node_indices)[(*nb_edges)++] = instruction.jump->true_branch->offset;
            (*list_node_indices)[(*nb_edges)++] = instruction.jump->false_branch->offset;
        }
    }
    
    // sort
    qsort((*list_node_indices), *nb_edges, sizeof(int), compare_ints);

    // remove duplicate
    for (int i = 0; i < *nb_edges; i++) {
        if (i == 0 || (*list_node_indices)[i] != (*list_node_indices)[i - 1])
        {
            (*list_node_indices)[(*nb_basic_nodes)++] = (*list_node_indices)[i];
        }
    }
    (*list_node_indices) = realloc((*list_node_indices), (*nb_basic_nodes) * sizeof(int));

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

Cfg* initialize_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges)
{
    Cfg* cfg = malloc(sizeof(Cfg));
    if (cfg == NULL)
    {
        fprintf(stderr, "malloc() Cfg failed\n");
        exit(1);
    }
    
    cfg->nb_nodes = nb_nodes;
    cfg->node_idx = 0;
    cfg->nodes = calloc(cfg->nb_nodes, sizeof(Node));
    if (cfg->nodes == NULL)
    {
        fprintf(stderr, "malloc() Node failed\n");
        exit(1);
    }
    
    cfg->nb_edges = nb_edges;
    cfg->edge_idx = 0;
    cfg->edges = calloc(cfg->nb_edges, sizeof(Edge));
    if (cfg->edges == NULL)
    {
        fprintf(stderr, "malloc() Edge failed\n");
        exit(1);
    }

    // start node
    Node* start_node = &cfg->nodes[0];
    start_node->start_instruction = &program->instructions[list_node_indices[0]];
    start_node->nb_instructions = list_node_indices[1] - list_node_indices[0];
    start_node->parents = NULL;
    start_node->children = NULL;
    
    // middle nodes
    for (int node_idx = 1; node_idx < cfg->nb_nodes - 1; node_idx++)
    {
        Node* node = &cfg->nodes[node_idx];
        node->start_instruction = &program->instructions[list_node_indices[node_idx]];
        node->nb_instructions = list_node_indices[node_idx + 1] - list_node_indices[node_idx];
        node->parents = NULL;
        node->children = NULL;
    }
    
    // end node
    Node* end_node = &cfg->nodes[cfg->nb_nodes - 1];
    end_node->start_instruction = &program->instructions[list_node_indices[nb_nodes - 1]];
    end_node->nb_instructions = program->nb_instructions - list_node_indices[nb_nodes - 1];
    end_node->parents = NULL;
    end_node->children = NULL;
    
    return cfg;
}

Instruction* get_instruction_from_node(Node* node, int instruction_offset)
{
    return (Instruction*)(node->start_instruction + instruction_offset);
}

Node* find_node_from_start_instruction_offset(Cfg* cfg, int start_instruction_offset)
{
    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        Node* node = &cfg->nodes[i];
        if (node->start_instruction->offset == start_instruction_offset)
        {
            return node;
        }
    }
    return NULL;
}

void update_neighbour_nodes(Cfg* cfg, Node* current_node, int next_node_start_instruction_offset)
{
    Node* true_branch_node = find_node_from_start_instruction_offset(cfg, next_node_start_instruction_offset);
    current_node->children[(current_node->child_idx)++] = true_branch_node;
    
    if (true_branch_node->parents == NULL)
    {
        true_branch_node->parents = malloc(sizeof(Node*));
    }
    else
    {
        true_branch_node->parents = realloc(true_branch_node->parents, true_branch_node->nb_parents * sizeof(*(true_branch_node->parents)) + sizeof(Node*));
    }
    
    (true_branch_node->nb_parents)++;
    true_branch_node->parents[(true_branch_node->parent_idx)++] = current_node;
}

Node* create_node(Cfg* cfg, int instruction_offset, int last_instruction_last_node)
{
    Node* current_node = find_node_from_start_instruction_offset(cfg, instruction_offset);
    if (current_node == NULL)
    {
        fprintf(stderr, "Node %ld not found\n", current_node - cfg->nodes);
        exit(1);
    }

    Instruction* last_instruction = get_instruction_from_node(current_node, current_node->nb_instructions - 1);
    
    if (current_node->visited == true)
    {
        return NULL;
    }

    // current_node creation
    current_node->visited = true;

    // here, we suppose that the program cannot end with a jmp instruction (strong supposition but hey...)
    if (last_instruction->kind == INSTR_JMP)
    {
        current_node->nb_children = 2;
        current_node->children = malloc(current_node->nb_children * sizeof(Node*));

        // true branch node ------------------------------------------------

        int true_branch_start_instruction_offset = last_instruction->jump->true_branch->offset;
        
        update_neighbour_nodes(cfg, current_node, true_branch_start_instruction_offset);
        
        create_node(cfg, true_branch_start_instruction_offset, last_instruction_last_node);
        
        // false branch node -----------------------------------------------
        
        int false_branch_start_instruction_offset = last_instruction->jump->false_branch->offset;
        
        update_neighbour_nodes(cfg, current_node, false_branch_start_instruction_offset);
        
        create_node(cfg, false_branch_start_instruction_offset, last_instruction_last_node);
    }
    else
    {
        // not last current_node
        if (last_instruction->offset != last_instruction_last_node)
        {
            int next_node_start_instruction_offset = (last_instruction + 1)->offset;
            current_node->nb_children = 1;
            current_node->children = malloc(current_node->nb_children * sizeof(Node*));
            
            update_neighbour_nodes(cfg, current_node, next_node_start_instruction_offset);
            
            create_node(cfg, next_node_start_instruction_offset, last_instruction_last_node);
        }
    }
    
    return current_node;
}

// TODO: create edges :)

Cfg* create_graph(Asm* program, int* list_node_indices, int nb_nodes, int nb_edges)
{
    Cfg* cfg = initialize_graph(program, list_node_indices, nb_nodes, nb_edges);

    create_node(cfg, START_NODE, program->nb_instructions - 1);

    return cfg;
}

Cfg* asm_to_cfg(Asm* program) {
    int* list_node_indices;
    int nb_nodes;
    int nb_edges;

    get_info_graph(program, &list_node_indices, &nb_nodes, &nb_edges);
    
    Cfg* cfg = create_graph(program, list_node_indices, nb_nodes, nb_edges);

    free(list_node_indices);
    
    return cfg;
}