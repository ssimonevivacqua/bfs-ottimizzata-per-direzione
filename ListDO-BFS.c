#include "shared/header.h"

int* LBFSHybridSearch(ListGraph *graph, int source, bool hybrid) {
    printf("\n");
    if(graph == NULL || graph->numVertices == 0 || graph->adj_list == NULL)
        exit(1);

    Parents *parents = newParents(graph->numVertices);
    Frontier *frontier = newFrontier(graph->numVertices);            //genero la struct che conterrà la coda (frontiera) e un bitset (frontiera)
    //ArrQueue *next = newArrQueue(graph_vertices);                                      //genero la coda per la frontiera della prossima iterazione
            //rimuovo next -> utilizzo frontier sia come frontier che come next, basta ottimizzare l'uso degli indici
    //initArrQueue(next); 
    HeuristicData heuristic;
    bool current_direction = 0;     //0 = top-down-step, 1 = bottom-up-step 

    if(parents == NULL || frontier == NULL){
        perror("BFSSearch run out of memory!\n\n");
        return NULL;
    }

    initHeuristic(&heuristic);
    //initFrontier(frontier, source);
    addToFrontier(frontier, source);                      //inserisco la source nella frontiera
    initParents(parents, graph->numVertices, graph->numEdges, source);

    
    if(hybrid) {
        while ( !FrontierIsEmpty(frontier) ) {
            //printf("\n\nfrontier: (front:%d, rear:%d)\n", frontier->queue->front->valore, frontier->queue->rear->valore);
            //FrontierPrint(frontier);                                                            //false -> non array_optimized
            if( current_direction = next_direction(&heuristic, current_direction, graph->numVertices, false)     == 0 )              //0 == top down step, 1 == bottom up step
            {
          //      printf("---- (top-down) ---\n");
                top_down_stepL(graph, frontier, parents);
                                    //non ho bisogno di aggiornare la frontiera con next se frontier e next sono la stessa cosa!!!
            //updateFrontierTopDown(frontier,next);               //inserisco nella frontiera i nodi trovati dal top-down rimuovendoli da next
            }
            else 
            {
          //      printf("\n\n-- (bottom-up) ---\n");
                bottom_up_stepL(graph, frontier, parents);
            //updateFrontierBottomUp(frontier,next);              //sostituisce i nodi in frontier con i nodi in next 
            //next = initArrQueue(next);                                //inizializzo una nuova coda per next
            }
            updateSizesL(graph, frontier, parents, &heuristic);
        }
    } else {
        while ( !FrontierIsEmpty(frontier)) 
            top_down_stepL(graph, frontier, parents);
    }
   // printf("\n\nfrontier END: (front:%d, rear:%d)\n", frontier->queue->front, frontier->queue->rear);
   // FrontierPrint(frontier);
    int *parentsArray = parents->array;
    freeAll(frontier, parents);    
    return parentsArray;
}

int* LBFSHybridSearchWithEdgesCount(ListGraph *graph, int source, bool hybrid, int* visited_edges) {
    printf("\n");
    if(graph == NULL || graph->numVertices == 0 || graph->adj_list == NULL)
        exit(1);

    Parents *parents = newParents(graph->numVertices);
    Frontier *frontier = newFrontier(graph->numVertices);            //genero la struct che conterrà la coda (frontiera) e un bitset (frontiera)
    //ArrQueue *next = newArrQueue(graph_vertices);                                      //genero la coda per la frontiera della prossima iterazione
            //rimuovo next -> utilizzo frontier sia come frontier che come next, basta ottimizzare l'uso degli indici
    //initArrQueue(next); 
    HeuristicData heuristic;
    bool current_direction = 0;     //0 = top-down-step, 1 = bottom-up-step 

    if(parents == NULL || frontier == NULL){
        perror("BFSSearch run out of memory!\n\n");
        return NULL;
    }

    initHeuristic(&heuristic);
    //initFrontier(frontier, source);
    addToFrontier(frontier, source);                      //inserisco la source nella frontiera
    initParents(parents, graph->numVertices, graph->numEdges, source);

    
    if(hybrid) {
        while ( !FrontierIsEmpty(frontier) ) {
            //printf("\n\nfrontier: (front:%d, rear:%d)\n", frontier->queue->front->valore, frontier->queue->rear->valore);
            //FrontierPrint(frontier);                                                            //false -> non array_optimized
            if( current_direction = next_direction(&heuristic, current_direction, graph->numVertices, true)     == 0 )              //0 == top down step, 1 == bottom up step
            {
          //      printf("---- (top-down) ---\n");
                top_down_stepLWithEdgesCount(graph, frontier, parents, visited_edges);
                                    //non ho bisogno di aggiornare la frontiera con next se frontier e next sono la stessa cosa!!!
            //updateFrontierTopDown(frontier,next);               //inserisco nella frontiera i nodi trovati dal top-down rimuovendoli da next
            }
            else 
            {
          //      printf("\n\n-- (bottom-up) ---\n");
                bottom_up_stepLWithEdgesCount(graph, frontier, parents, visited_edges);
            //updateFrontierBottomUp(frontier,next);              //sostituisce i nodi in frontier con i nodi in next 
            //next = initArrQueue(next);                                //inizializzo una nuova coda per next
            }
            updateSizesL(graph, frontier, parents, &heuristic);
        }
    } else {
        while ( !FrontierIsEmpty(frontier)) 
            top_down_stepLWithEdgesCount(graph, frontier, parents, visited_edges);
    }
   // printf("\n\nfrontier END: (front:%d, rear:%d)\n", frontier->queue->front, frontier->queue->rear);
   // FrontierPrint(frontier);
    int *parentsArray = parents->array;
    freeAll(frontier, parents);    
    return parentsArray;
}


int* LBFSHybridSearch_study(ListGraph *graph, int source, bool hybrid, Iterations *iters) {
    int curr_iter = iters->curr_iter;

    printf("\nRicerca num %d,\n", curr_iter);
    if(graph == NULL || graph->numVertices == 0 || graph->adj_list == NULL)
        exit(1);

    Parents *parents = newParents(graph->numVertices);
    Frontier *frontier = newFrontier(graph->numVertices);            //genero la struct che conterrà la coda (frontiera) e un bitset (frontiera)
    //ArrQueue *next = newArrQueue(graph_vertices);                                      //genero la coda per la frontiera della prossima iterazione
            //rimuovo next -> utilizzo frontier sia come frontier che come next, basta ottimizzare l'uso degli indici
    //initArrQueue(next); 
    HeuristicData heuristic;
    bool current_direction = 0;     //0 = top-down-step, 1 = bottom-up-step 

    initHeuristic(&heuristic);
    //initFrontier(frontier, source);
    addToFrontier(frontier, source);                      //inserisco la source nella frontiera
    initParents(parents, graph->numVertices, graph->numEdges, source);

    if(hybrid) {
        while ( !FrontierIsEmpty(frontier) ) {
            //printf("\n\nfrontier: (front:%d, rear:%d)\n", frontier->queue->front->valore, frontier->queue->rear->valore);
            //FrontierPrint(frontier);                                                              //false -> non array_optimized
            if( current_direction = next_direction(&heuristic, current_direction, graph->numVertices, true)     == 0 )              //0 == top down step, 1 == bottom up step
            {
                //printf("---- (top-down) ---\n");
                top_down_stepL(graph, frontier, parents);
                iters->iter[curr_iter].top_down_step_count++;
                                    //non ho bisogno di aggiornare la frontiera con next se frontier e next sono la stessa cosa!!!
            //updateFrontierTopDown(frontier,next);               //inserisco nella frontiera i nodi trovati dal top-down rimuovendoli da next
            }
            else 
            {
                //printf("\n\n-- (bottom-up) ---\n");
                bottom_up_stepL(graph, frontier, parents);
                iters->iter[curr_iter].bottom_up_step_count++;
            //updateFrontierBottomUp(frontier,next);              //sostituisce i nodi in frontier con i nodi in next 
            //next = initArrQueue(next);                                //inizializzo una nuova coda per next
            }
            updateSizesL(graph, frontier, parents, &heuristic);
        }
    } else {
        while ( !FrontierIsEmpty(frontier)) 
            top_down_stepL(graph, frontier, parents);
    }
    printf("\n\nfrontier END: (front:%d, rear:%d)\n", frontier->queue->front, frontier->queue->rear);
    FrontierPrint(frontier);
   // freeAll(frontier);      
    return parents->array;
}