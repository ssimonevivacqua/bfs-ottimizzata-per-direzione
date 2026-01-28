#include "shared/header.h"

void top_down_stepA_verify(Graph *graph, Frontier* frontier, Parents* parents, VerticesDepth *d) {

    initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    //FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
                //printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                d->array_depth[out_edge] = d->depth;
                addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
            }
        }
    }
    d->depth++;
}

void bottom_up_stepA_verify(Graph *graph, Frontier *frontier, Parents* parents, VerticesDepth *d) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
    //FrontierPrint(frontier);
    Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    frontier->set = newBitSet(frontier_bitset->size);

    /*printf("Bitset frontier: ");
    printBitset(frontier_bitset);
    printf("\n\n\n");*/
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier_bitset, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    d->array_depth[curr_node] = d->depth;
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    deallocBitset(frontier_bitset);
    d->depth++;
}

void classic_top_down_stepA(Graph *graph, Frontier* frontier, Parents* parents) {

    //initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
          //      printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                arrEnqueue(frontier->queue, out_edge);                          //inserisco nella frontiera il nodo
            }
        }
    }
}

void top_down_stepA(Graph *graph, Frontier* frontier, Parents* parents) {

    initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
                //printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
            }
        }
    }
}

void bottom_up_stepA(Graph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    frontier->set = newBitSet(frontier_bitset->size);

    //printf("Bitset frontier: ");
    //printBitset(frontier_bitset);
    //printf("\n\n\n");
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier_bitset, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    deallocBitset(frontier_bitset);
    free(frontier_bitset);
    return ;    
}


////////////////////////////////////////////////////////////////// COUNT NUMBER OF EDGES CODE //////////////////////////////////////////////////////////////////
void classic_top_down_stepAWithEdgesCount(Graph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {

    //initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            (*visited_edges)++;
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
          //      printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                arrEnqueue(frontier->queue, out_edge);                          //inserisco nella frontiera il nodo
            }
        }
    }
}

void top_down_stepAWithEdgesCount(Graph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {

    initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            (*visited_edges)++;
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
                //printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
            }
        }
    }
}

void bottom_up_stepAWithEdgesCount(Graph *graph, Frontier *frontier, Parents* parents, int* visited_edges) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    frontier->set = newBitSet(frontier_bitset->size);

    //printf("Bitset frontier: ");
    //printBitset(frontier_bitset);
    //printf("\n\n\n");
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                (*visited_edges)++;
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier_bitset, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    deallocBitset(frontier_bitset);
    free(frontier_bitset);
    return ;    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7


void updateSizesA( Graph *graph, Frontier *frontier, Parents *parents, HeuristicData *heuristic)   {
    if(FrontierIsEmpty(frontier))
        return;
    heuristic->frontier_prev_size = heuristic->frontier_size;
    heuristic->frontier_size = getFrontierLen(frontier);     //recupero la grandezza della frontiera
    heuristic->edges_frontier_size = getFrontierEdgesA(frontier, graph);
    heuristic->edges_parents_size = parents->total_edges_degree;
    return ;
}

int getFrontierEdgesA ( Frontier *f, Graph *graph) {          //prende una coda e una matrice di adiacenza come input, ritorna il numero di archi dei nodi nella frontiera
    int totalEdges = 0;
    int currentNode;
    //int temp_rear = frontier->queue->rear;
    int front = f->queue->front;


    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += graph->out_degree[f->queue->value[front+i]];
    }
    return totalEdges;
}


void top_down_stepL(ListGraph *graph, Frontier* frontier, Parents* parents) {
    initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);

    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = getListLen(graph->adj_list[curr_node]);
      //  printf("\ncurr_node %d\n", curr_node);
        LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = t->valore;                  //nodo raggiunto dal nodo corrente dall'arco edge
            t = t->succ;
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
           //     printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, getListLen(graph->adj_list[out_edge]));                     //allora lo visito e segno da chi sono partito per visitarlo
                addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
            }
        }
    }
}

void bottom_up_stepL(ListGraph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);

    Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    frontier->set = newBitSet(frontier_bitset->size);

  /*  printf("Bitset frontier: ");
    printBitset(frontier_bitset);
    printf("\n\n\n");*/
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            curr_node_degree = getListLen(graph->adj_list[curr_node]);
            LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = t->valore;
                t = t->succ;
                if( is_set(frontier_bitset, out_edge) ) {
            //        printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    deallocBitset(frontier_bitset);
    free(frontier_bitset);
    return ;
}


/////////////////////////////////////////////////////////////////////////// visited EDGES COUNT FUNCTION ////////////////////////////////////////////////////////7

void top_down_stepLWithEdgesCount(ListGraph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {
    initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);

    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        curr_node_degree = getListLen(graph->adj_list[curr_node]);
      //  printf("\ncurr_node %d\n", curr_node);
        LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            (*visited_edges)++;
            out_edge = t->valore;                  //nodo raggiunto dal nodo corrente dall'arco edge
            t = t->succ;
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
            if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
           //     printf("Adds %d --> %d\n", curr_node, out_edge);
                setParents(parents, out_edge, curr_node, getListLen(graph->adj_list[out_edge]));                     //allora lo visito e segno da chi sono partito per visitarlo
                addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
            }
        }
    }
}

void bottom_up_stepLWithEdgesCount(ListGraph *graph, Frontier *frontier, Parents* parents, int* visited_edges) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);

    Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    frontier->set = newBitSet(frontier_bitset->size);

  /*  printf("Bitset frontier: ");
    printBitset(frontier_bitset);
    printf("\n\n\n");*/
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            curr_node_degree = getListLen(graph->adj_list[curr_node]);
            LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
            for(int edge = 0; edge < curr_node_degree; edge++) {
                (*visited_edges)++;
                out_edge = t->valore;
                t = t->succ;
                if( is_set(frontier_bitset, out_edge) ) {
            //        printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    deallocBitset(frontier_bitset);
    free(frontier_bitset);
    return ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void updateSizesL( ListGraph *graph, Frontier *frontier, Parents *parents, HeuristicData *heuristic)   {
    if(FrontierIsEmpty(frontier))
        return;
    heuristic->frontier_prev_size = heuristic->frontier_size;
    heuristic->frontier_size = getFrontierLen(frontier);     //recupero la grandezza della frontiera
    heuristic->edges_frontier_size = getFrontierEdgesL(frontier, graph);
    heuristic->edges_parents_size = parents->total_edges_degree;
    return ;
}

int getFrontierEdgesL ( Frontier *f, ListGraph *graph) {          //prende una coda e una matrice di adiacenza come input, ritorna il numero di archi dei nodi nella frontiera
    int totalEdges = 0;
    int currentNode;
    //int temp_rear = frontier->queue->rear;
    int front = f->queue->front;


    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += getListLen(graph->adj_list[f->queue->value[front+i]]);
    }
    return totalEdges;
}


void top_down_stepM(MatrixGraph *graph, Frontier* frontier, Parents* parents) {
    //initFrontierBits(frontier);             //non serve per il top-down, allora intanto la inizializzo, so già che svuoterò la frontiera
    int curr_node, curr_node_degree, max_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
//FrontierPrint(frontier);
    //printf("\nFrontierLen %d",curr_frontier_size);
    for(i = 0; i < curr_frontier_size; i++) {
    
        curr_node = getFromFrontier(frontier);      // == nodo corrente
        max_node_degree = graph->out_degree[curr_node];
        curr_node_degree = 0;
      //  printf("\ncurr_node %d\n", curr_node);
        //printf("\ncurr_node degree %d\n ",curr_node_degree);
        //if( graph->out_degree[curr_node] > 0) {                          
        for(int edge = 0; edge < graph->numVertices; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            if(curr_node_degree == max_node_degree)
                break;
            if(graph->adj_matrix[ (curr_node*graph->numVertices) + edge]) {
                curr_node_degree++;
                out_edge = edge;               //nodo raggiunto dal nodo corrente dall'arco edge
            //printf("\ncurr_node out edge %d: %d\n ",edge, out_edge);
                if(getParents(parents, out_edge) == -1) {                          //se il nodo raggiunto da un arco del nodo corrente non è stato visitato
           //         printf("Adds %d --> %d\n", curr_node, out_edge);
                    setParents(parents, out_edge, curr_node, graph->out_degree[out_edge]);                     //allora lo visito e segno da chi sono partito per visitarlo
                    addToFrontier(frontier, out_edge);                           //inserisco nella frontiera il nodo
                }
            }
        }
    }
}

void bottom_up_stepM(MatrixGraph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, max_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);

    //Bitset *frontier_bitset = frontier->set;         //creo nuovo bitset, cosi da usare il vecchio per le verifiche, ed il nuovo per gli insert
    //frontier->set = newBitSet(frontier_bitset->size);
//FrontierPrint(frontier);
    //printf("Bitset frontier: ");
    //printBitset(frontier_bitset);
    //printf("\n\n\n");
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        if(getParents(parents,curr_node) == -1) {
            max_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_frontier_size; edge++) {
                int frontier_elem = getFromFrontierN(frontier, edge);
                if(graph->adj_matrix[ (curr_node*graph->numVertices) + frontier_elem]) {
                    out_edge = frontier_elem;
            //        printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, graph->out_degree[curr_node]);
                    addToFrontier(frontier, curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    //deallocBitset(frontier_bitset);
    return ;
}

void updateSizesM( MatrixGraph *graph, Frontier *frontier, Parents *parents, HeuristicData *heuristic)   {
    if(FrontierIsEmpty(frontier))
        return;
    heuristic->frontier_prev_size = heuristic->frontier_size;
    heuristic->frontier_size = getFrontierLen(frontier);     //recupero la grandezza della frontiera
    heuristic->edges_frontier_size = getFrontierEdgesM(frontier, graph);
    heuristic->edges_parents_size = parents->total_edges_degree;
    return ;
}

int getFrontierEdgesM ( Frontier *f, MatrixGraph *graph) {          //prende una coda e una matrice di adiacenza come input, ritorna il numero di archi dei nodi nella frontiera
    int totalEdges = 0;
    int currentNode;
    //int temp_rear = frontier->queue->rear;
    int front = f->queue->front;


    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += graph->out_degree[f->queue->value[front+i]];
    }
    return totalEdges;
}


void updateFrontierTopDown(Frontier *frontier, ArrQueue *next) {  
    while ( !isEmptyS(next)) {
        addToFrontier(frontier, arrDequeue(next));
    }                      //inserisco nella frontiera i nodi trovati dal top-down o bottom-up
    return ;
}


void updateFrontierBottomUp(Frontier *frontier, ArrQueue *next) {  
    //FrontierClear(frontier);
    //updateFrontierTopDown(frontier,next);
    deallocFrontierQueue(frontier);     //rimuovo da memoria la ArrQueue contenuta in frontier, resettando il bitset
    frontier->queue = next;             //assegno come ArrQueue di frontier next, evito così lo spostamento di ogni valore nella coda next in frontier
    return ;        // ---------------> frontiera grande durante il bottom-up === più guadagno da questa operazione
}


bool next_direction(  HeuristicData *h, bool current_direction, int total_nodes, bool array_optimized) {
    int param_A;   //parametro empirico 
    int param_B;

    if(array_optimized) {
        param_A = 14;
        param_B = 24;
    } else {
        param_A = 20;
        param_B = 10;
    }

    if (current_direction == 0) {   //se mi trovo in top-down-step
        if(h->edges_frontier_size > h->edges_parents_size/param_A && h->frontier_size > h->frontier_prev_size)      // mf > mu/param_A & growing
            return 1;                                                                                       //vado in bottom-up
        else 
            return 0;                                                                                       //rimango in top-down
    }
    else {                          //se mi trovo in bottom-up-step
        if(h->frontier_size < total_nodes/param_B && h->frontier_size <= h->frontier_prev_size)                  // nf < n/param_B & shrinking 
            return 0;                                                                                       //vado in top-down
        else
            return 1;                                                                                       //rimango in bottom-up
    }
}


void initHeuristic(HeuristicData *h){
    h->frontier_size = 1;         //grandezza della frontiera corrente
    h->frontier_prev_size = 1;    //grandezza della frontiera dell'iterazione precedente alla corrente 
    h->edges_frontier_size = 0;   //out-degree dei nodi in frontier
    h->edges_parents_size = 0;    //out-degree dei nodi non ancora esplorati
}

void initParents(Parents *parents, int total_nodes, long edges, int source_num){
    //int *parents = (int *)malloc(sizeof(int)*(n));
    setArrayToN(parents->array, -1, total_nodes);               //imposto il vettore dei padri con tutti i nodi non visitati (-1)
    parents->array[source_num]= source_num;                    //visito il primo nodo (la source)
    parents->total_edges_degree = edges;
    return ;
}

void initFrontier(Frontier *frontier, int source) {
    //if( graph->out_degree[source] > 0 ) {                     //se ho almeno un arco apartire dal nodo sorgente
        addToFrontier(frontier, source);                      //inserisco la source nella frontiera
    //}
    //else {
      //  perror("Il nodo sorgente non ha archi uscenti!\n");
        //exit(1);
    //}
    return ;
}

int *edgesArray(int size) {
    int *edgesArr = (int *)malloc(sizeof(int)*(size));
    setArrayToN(edgesArr, -1, size);
    return edgesArr;
}

void setArrayToN(int *a, int n, int len) {
    size_t i = 0;
    for (i; i < len; i++)
        a[i] = n;
}

void freeAll(Frontier *frontier, Parents *parents){
    freeFrontier(frontier);
    free(parents);
    return ;
}

VerticesDepth* initNewDepth(int n, int source){
    VerticesDepth *d = malloc(sizeof(VerticesDepth));
    d->depth = 1;
    d->array_depth = malloc(sizeof(int)*n);
    setArrayToN(d->array_depth, -1, n);
    d->array_depth[source] = 0;
    return d;
}