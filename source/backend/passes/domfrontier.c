#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils/graph_utils.h"
#include "backend/CFG.h"


void DFS(Cfg* cfg, Node* node, List* list_nodes)
{
    node->visited = true;
    for (int i = 0; i < node->nb_successors; i++)
    {
        Node* next_node = node->successors[i];
        if (next_node->visited == false)
        {
            DFS(cfg, next_node, list_nodes);
            /* update edge number */
            for (int edge_idx = 0; edge_idx < cfg->nb_edges; edge_idx++)
            {
                Edge edge = cfg->edges[edge_idx];
                if (edge.start == node && edge.target == next_node)
                {
                    edge.number = list_nodes->index;
                    break;
                }
            }
            
            /* update node number */
            next_node->number = list_nodes->index;
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
    temp[cfg->nb_nodes - 1]->number = cfg->nb_nodes - 1;
    
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
    
    while (finger1->number != finger2->number)
    {
        while (finger1->number < finger2->number)
        {
            finger1 = finger1->dominator;
        }
        while (finger1->number > finger2->number)
        {
            finger2 = finger2->dominator;
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
void compute_idom(Cfg* cfg)
{
    /* reverse postorder list computation */
    Node** list_post_order = malloc(cfg->nb_nodes * sizeof(Node*));
    reverse_post_order(cfg, &list_post_order);

    cfg->header->dominator = cfg->header;
    cfg->header->processed = true;
    
    bool changed = true;
    while (changed)
    {
        changed = false;
        /* for all nodes b in reverse postorder except for start_node */
        for (int i = 1; i < cfg->nb_nodes; i++)
        {
            Node* b = list_post_order[i];
            Node* new_idom;
            for (int predecessor_idx = 0; predecessor_idx < b->nb_predecessors; predecessor_idx++)
            {
                Node* predecessor = b->predecessors[predecessor_idx];
                if (predecessor->processed)
                {
                    new_idom = b->predecessors[predecessor_idx];
                    break;
                }
            }
            
            for (int predecessor_idx = 0; predecessor_idx < b->nb_predecessors; predecessor_idx++)
            {
                Node* p = b->predecessors[predecessor_idx];
                if (p->dominator != NULL)
                {
                    new_idom = intersect(p, new_idom);
                }
            }

            if (b->dominator != new_idom)
            {
                b->dominator = new_idom;
                b->processed = true;
                changed = true;
            }
        }

        // printf("\niteration:\n\n");
        // for (int i = 0; i < cfg->nb_nodes; i++)
        // {
        //     printf("node %d: dom = %d\n", cfg->nodes[i].start_instruction->offset, cfg->nodes[i].dominator->start_instruction->offset);
        // }
    }
    
    free(list_post_order);
}

bool set_contains(Node* b, Node* set)
{
    for (int i = 0; i < set->nb_dominators; i++)
    {
        if (b == set->dominator_frontier[i])
        {
            return true;
        }
    }

    return false;
}

void add_to_set(Node* b, Node* runner)
{
    if (set_contains(b, runner))
    {
        return;
    }
    
    if (runner->dominator_frontier == NULL)
    {
        runner->dominator_frontier = malloc(sizeof(Node*));
    }
    else
    {
        runner->dominator_frontier = realloc(runner->dominator_frontier, (runner->nb_dominators + 1) * sizeof(Node*));
    }
    (runner->nb_dominators)++;
    runner->dominator_frontier[(runner->dominator_idx)++] = b;
}

void compute_dominance_frontier(Cfg* cfg)
{
    compute_idom(cfg);

    for (int node_idx = 0; node_idx < cfg->nb_nodes; node_idx++)
    {
        Node* b = &cfg->nodes[node_idx];
        if (b->nb_predecessors >= 2)
        {
            for (int predecessor_idx = 0; predecessor_idx < b->nb_predecessors; predecessor_idx++)
            {
                Node* runner = b->predecessors[predecessor_idx];
                while (runner != b->dominator)
                {
                    /* add b to runner's dominance frontier set */
                    add_to_set(b, runner);
                    
                    runner = runner->dominator;
                }
            }
        }
    }

    for (int i = 0; i < cfg->nb_nodes; i++)
    {
        Node* node = &cfg->nodes[i];
        // printf("Dominance frontier for node %d:\n", node->start_instruction->offset);
        // for (int dom_idx = 0; dom_idx < node->nb_dominators; dom_idx++)
        // {
        //     printf("%d ", node->dominator_frontier[dom_idx]->start_instruction->offset);
        // }
        // printf("\n");
    }
}