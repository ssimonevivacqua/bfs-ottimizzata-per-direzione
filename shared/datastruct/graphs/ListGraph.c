#include "../../header.h"

Bitset **used_valuesL;

ListGraph *newListGraph() {
    ListGraph *graph = (ListGraph*)malloc(sizeof(ListGraph));
    return graph;
}

int initListGraph(ListGraph *graph, int nodes, bool directed) {
    if(graph == NULL){
        perror("ListGraph, newListGraph: Runs out of memory!\n\n");
        return 1;
    }
    graph->numVertices = nodes;
    graph->numEdges = 0;
    graph->directed = directed;
    graph->adj_list = malloc(sizeof(LinkedList)*nodes);
    if (nodes > 0) { 
        int i;
        for(i = 0; i < nodes; i++) {
            graph->adj_list[i] = NewList();
            if(graph->adj_list[i] == NULL) {
                perror("ListGraph, initListGraph: Runs out of memory!\n\n");
                return 1;
            }
        }
    }
    return 0;
}

void initRandListGraph(ListGraph *graph, int nodes, bool directed) {
    used_valuesL = malloc(sizeof(Bitset)*nodes);
    int t = 0;
    for (t = 0; t < nodes; t++) 
        used_valuesL[t] = newBitSet(nodes);
    initListGraph(graph, nodes, directed);
    return ;
}

void freeListGraph(ListGraph *g) {
   // printf("entra in list free graph, vertices %d\n", g->numVertices);
    for(int i = 0; i < g->numVertices; i++)
        freeList(g->adj_list[i]);
   // printf("finisce for list free graph\n");
    free(g->adj_list);
   //     printf("finisce list free graph\n");
    //free(g);
}


int listEdge(ListGraph *graph, int a, int b) {
    int maxNodes = graph->numVertices;
    if( a < 0 || a >= maxNodes || b < 0 || b > maxNodes) {
        perror("\nlistEdge: Invalid edge!\n\n");
        exit(1);
    }

    if (graph->adj_list != NULL) {
        if( addListElem(graph->adj_list[a], b) == 1){
            perror("ListGraph, listEdge: Runs out of memory!\n\n");
            return 1;
        }
        graph->numEdges++;
    }
    return 0;
}

int doubleListEdge(ListGraph *graph, int a, int b) {
    if (listEdge(graph, a, b) == 1 || listEdge(graph, b, a) == 1)
        return 1;
    //graph->numEdges--;
    return 0;
}

void printListGraph(ListGraph *g) {
    printf("\n---------------------- Graph:\n\n");
    LinkedList_std *t;
    for(int i = 0; i < g->numVertices; i++) {
        printf("Node %d, edges(tot:%d): ",i,getListLen(g->adj_list[i]));
        t = g->adj_list[i]->start->succ;
        for(int i2 = 0; i2 < getListLen(g->adj_list[i]); i2++) {
            printf("%d ", t->valore);
            t = t->succ;
        }
        printf("\n");
    }
    /*
    for(int i = 0; i < g->numEdges; i++)
        printf("%d ", g->all_out_edges[i]);
        */
    return ;
}