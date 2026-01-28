#include "shared/header.h"

Chunk ** local_frontier = NULL;
ArrQueue ** local_frontier2 = NULL;

void top_down_stepA_verify(Graph *graph, Frontier* frontier, Parents* parents, VerticesDepth *d) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }

    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    //FrontierPrint(frontier);
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d\n", curr_node);
                        
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                //printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                //local_frontier2[tid]->value[++local_frontier2[tid]->rear] = out_edge;
                chunkEnqueue(local_frontier[tid], out_edge);
                //assert(ok && "fallita!!\n");
                d->array_depth[out_edge] = d->depth;
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
    /*
    printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }
        */
    
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //printf("a2\n");
    d->depth++;
}

void bottom_up_stepA_verify(Graph *graph, Frontier *frontier, Parents* parents, VerticesDepth *d) {
    int curr_node, curr_node_degree, out_edge;
    //FrontierPrint(frontier);
    //int curr_frontier_size = getFrontierLen(frontier);
    /*printf("Bitset frontier: ");
    printBitset(frontier->set);
    printf("\n\n\n");*/
    

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {       //se non visitato
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier->set, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge,  curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    //addToFrontier(frontier, curr_node);
                    //local_frontier2[tid]->value[++local_frontier2[tid]->rear] = curr_node;
                    chunkEnqueue(local_frontier[tid], curr_node);
                    //assert(ok && "fallita!!\n");
                    d->array_depth[curr_node] = d->depth;
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    d->depth++;
    return ;
}

void classic_top_down_stepA(Graph *graph, Frontier* frontier, Parents* parents) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }
    
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d, degree %d\n", curr_node, curr_node_degree);
                     
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                //printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
    /*printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
}

    // DEBUG padding per false sharing
static inline int is_al64(const void* p){ return ((uintptr_t)p & 63)==0; }

void dbg_check_chunk(Chunk* c){
  printf("Chunk      %p  aligned64=%d\n", (void*)c, is_al64(c));
  printf("&curr_size %p  aligned64=%d\n", (void*)&c->curr_size, is_al64(&c->curr_size));
  printf("value[]    %p  aligned64=%d\n", (void*)c->value, is_al64(c->value));
  if (c->value[0]) printf("slot0      %p  aligned64=%d\n", (void*)c->value[0], is_al64(c->value[0]));
}

void top_down_stepA(Graph *graph, Frontier* frontier, Parents* parents) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }
    
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d, degree %d\n", curr_node, curr_node_degree);
                     
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                //printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
    /*printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
}

void bottom_up_stepA(Graph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
    /*printf("Bitset frontier: ");
    printBitset(frontier->set);
    printf("\n\n\n");*/

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {       //se non visitato
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier->set, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, graph->out_degree[curr_node]);
                    //addToFrontier(frontier, curr_node);
                    chunkEnqueue(local_frontier[tid], curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    return ;
}


void top_down_stepL(ListGraph *graph, Frontier* frontier, Parents* parents) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }

    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = getListLen(graph->adj_list[curr_node]);
        LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
        //printf("\ncurr_node %d\n", curr_node);
                        
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = t->valore;             //nodo raggiunto dal nodo corrente dall'arco edge
            t = t->succ;                            //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
          //      printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += getListLen(graph->adj_list[out_edge]);                           //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
  /*  printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //printf("a2\n");
}

void bottom_up_stepL(ListGraph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
  /*  printf("Bitset frontier: ");
    printBitset(frontier->set);
    printf("\n\n\n");*/

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {       //se non visitato
            curr_node_degree = getListLen(graph->adj_list[curr_node]);
            LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = t->valore;
                t = t->succ;
                if( is_set(frontier->set, out_edge) ) {
                  //  printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    //addToFrontier(frontier, curr_node);
                    chunkEnqueue(local_frontier[tid], curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    //updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    return ;
}


void top_down_stepM(MatrixGraph *graph, Frontier* frontier, Parents* parents) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }

    int curr_node, curr_node_degree, max_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, max_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        max_node_degree = graph->out_degree[curr_node];
        curr_node_degree = 0;
      //  printf("\ncurr_node %d\n", curr_node);
                        
        for(int edge = 0; edge < graph->numVertices; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            if( graph->adj_matrix[ (curr_node*graph->numVertices) + edge]) {
                if(curr_node_degree == max_node_degree)
                    break;
                out_edge = edge;               //nodo raggiunto dal nodo corrente dall'arco edge
                curr_node_degree++;
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
                if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                 //   printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                    edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                    chunkEnqueue(local_frontier[tid], out_edge);
                }
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
  /*  printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //printf("a2\n");
}

void bottom_up_stepM(MatrixGraph *graph, Frontier *frontier, Parents* parents) {
    int curr_node, max_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
  //  printf("Bitset frontier: ");
   // printBitset(frontier->set);
    //printf("\n\n\n");

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, max_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {
            max_node_degree = graph->out_degree[curr_node];
            //curr_node_degree = 0;
            for(int edge = 0; edge < curr_frontier_size; edge++) {
                int frontier_elem = getFromFrontierN(frontier, edge);
                if(graph->adj_matrix[ (curr_node*graph->numVertices) + frontier_elem] ) {
                    out_edge = frontier_elem;
                    //curr_node_degree++;
                   // printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, graph->out_degree[curr_node]);
                    //addToFrontier(frontier, curr_node);
                    chunkEnqueue(local_frontier[tid], curr_node);
                    break;                                             //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    return ;
}


Chunk **initLocalFrontier(int nodes, int reducer) {
    Chunk **local_frontier = malloc(sizeof(Chunk*)*omp_get_max_threads());
    for(int i = 0; i < omp_get_max_threads(); i++) {
        local_frontier[i] = newChunk(nodes/reducer+1);
    }
    return local_frontier;
}

ArrQueue **initLocalFrontier2(int nodes, int reducer) {
    ArrQueue **local_frontier = malloc(sizeof(ArrQueue*)*omp_get_max_threads());
    for(int i = 0; i < omp_get_max_threads(); i++) {
        local_frontier[i] = newArrQueue(nodes/reducer);
    }
    return local_frontier;
}

void updateFrontier(Frontier *frontier) { 
    //printf("\n\n\n bitset verify:\n");
    FrontierClear(frontier); 
    //printBitset(frontier->set);
    const int T = omp_get_max_threads();
        //salvo le lungezze di ogni frontiera locale, per non richiamare chunkLen più di T volte
    int thread_sizes[T];
    int total_thread_sizes = 0;

    #pragma omp parallel for reduction(+:total_thread_sizes)
    for (int t = 0; t < T; t++){
        thread_sizes[t] = chunkLen(local_frontier[t]);
        total_thread_sizes = thread_sizes[t];
    }

    if(total_thread_sizes > 0) {

        //calcolo offset per ogni thread
        int thread_offset[T];
        thread_offset[0] = 0;       //perche per il primo parto da 0 nell'array
    
        for(int i = 1; i < T ; i++)      //parto da 1 siccome 0 già settato 
        {
            thread_offset[i] = thread_offset[i - 1] + thread_sizes[i - 1];
        //printf("off: %d\n",thread_offset[i]);    
        }      

    /*#pragma omp parallel for
    for (int t = 0; t < T; t++){
        printf("local_frontier_size %d: %d\n\n\n", t, local_frontier[t]->rear);
    }*/

        //fondo
        #pragma omp parallel for schedule(dynamic, NUM_CHUNK)
        for (int t = 0; t < T; t++) {
            int max_slot_len = local_frontier[t]->slot_len;
            int slot = 0;
            int u = 0;
            int toAdd;
                for(int i = 0; i < thread_sizes[t]; i++) {
                    if( u >= max_slot_len ) {
                        slot++;
                        u = 0;
                    }
                    toAdd = local_frontier[t]->value[slot][u++];
                //printf("frontier adds in %d: %d (thread: %d)\n", thread_offset[t]+i, toAdd, t);
                    frontier->queue->value[thread_offset[t] + i] = toAdd;
                //frontier->queue->rear++;      //non buono in race condition!!!!
                //set_bit(frontier->set , toAdd);   //non buono! idealmente ok ma siccome sono divisi in byte, più thread potrebbero scrivere sullo stesso ugualmente
            }
        }
        frontier->queue->front = 0;
        frontier->queue->rear = total_thread_sizes-1;

        //printf("\n\n ? \n\n");
        setFrontierBitset(frontier);
    //printf("f\n");
        //inizializzo la frontiera locale
    #pragma omp parallel for 
        for (int t = 0; t < T; t++) {
            initChunk(local_frontier[t]);
        }
    //printf("a\n");
        //printf("\n\n end \n\n");
    }
}

void updateFrontier2(Frontier *frontier) { 
    //printf("\n\n\n bitset verify:\n");
    FrontierClear(frontier); 
    //printBitset(frontier->set);
    const int T = omp_get_max_threads();
        //salvo le lungezze di ogni frontiera locale, per non richiamare chunkLen più di T volte
    int thread_sizes[T];
    int total_thread_sizes = 0;

    #pragma omp parallel for reduction(+:total_thread_sizes)
    for (int t = 0; t < T; t++){
        thread_sizes[t] = arrQueueLen(local_frontier2[t]);
        total_thread_sizes = thread_sizes[t];
        //printf("thread sizes %d: %d\n", omp_get_thread_num(), total_thread_sizes);
    }
    //printf("total thread sizes: %d\n", total_thread_sizes);
    if(total_thread_sizes > 0) {

        //calcolo offset per ogni thread
        int thread_offset[T];
        thread_offset[0] = 0;       //perche per il primo parto da 0 nell'array
    
        for(int i = 1; i < T ; i++)      //parto da 1 siccome 0 già settato 
        {
            thread_offset[i] = thread_offset[i - 1] + thread_sizes[i - 1];
        //printf("off: %d\n",thread_offset[i]);    
        }      

    /*#pragma omp parallel for
    for (int t = 0; t < T; t++){
        printf("local_frontier_size %d: %d\n\n\n", t, local_frontier[t]->rear);
    }*/

        //fondo
        #pragma omp parallel for schedule(dynamic, NUM_CHUNK)
        for (int t = 0; t < T; t++) {
            int toAdd;
            for(int i = 0; i < thread_sizes[t]; i++) {
                toAdd = local_frontier2[t]->value[i];
                //printf("frontier adds in %d: %d (thread: %d)\n", thread_offset[t]+i, toAdd, t);
                frontier->queue->value[thread_offset[t] + i] = toAdd;
                //frontier->queue->rear++;      //non buono in race condition!!!!
                //set_bit(frontier->set , toAdd);   //non buono! idealmente ok ma siccome sono divisi in byte, più thread potrebbero scrivere sullo stesso ugualmente
            }
        }
        frontier->queue->front = 0;
        frontier->queue->rear = total_thread_sizes-1;

        //printf("\n\n ? \n\n");
        setFrontierBitset(frontier);
    //printf("f\n");
        //inizializzo la frontiera locale
    #pragma omp parallel for 
        for (int t = 0; t < T; t++)
            initArrQueue(local_frontier2[t]); 
    //printf("a\n");
        //printf("\n\n end \n\n");
    }
}


void updateSizesA( Graph *graph, Frontier *frontier, Parents *parents, HeuristicData *heuristic)   {
    if(FrontierIsEmpty(frontier))
        return ;
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

    #pragma omp parallel for reduction(+:totalEdges)
    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += graph->out_degree[f->queue->value[front+i]];
    }
    return totalEdges;
}

void updateSizesL( ListGraph *graph, Frontier *frontier, Parents *parents, HeuristicData *heuristic)   {
    if(FrontierIsEmpty(frontier))
        return ;
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

    #pragma omp parallel for reduction(+:totalEdges)
    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += getListLen(graph->adj_list[f->queue->value[front+i]]);
    }
    return totalEdges;
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

    #pragma omp parallel for reduction(+:totalEdges)
    for(int i = 0; i < getFrontierLen(f); i++) {
        totalEdges += graph->out_degree[f->queue->value[front+i]];
    }
    return totalEdges;
}

void printLocalFrontier() {
        Chunk ** local_frontier;
    printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }
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
/*
void initFrontier(Frontier *frontier, Graph *graph, int source) {
    if( graph->out_degree[source] > 0 ) {                     //se ho almeno un arco apartire dal nodo sorgente
        addToFrontier(frontier, source);                      //inserisco la source nella frontiera
    }
    else {
        perror("Il nodo sorgente non ha archi uscenti!\n");
        exit(1);
    }
    return ;
}
*/
int *edgesArray(int size) {
    int *edgesArr = (int *)malloc(sizeof(int)*(size));
    setArrayToN(edgesArr, -1, size);
    return edgesArr;
}

void setArrayToN(int *a, int n, int len) {
    size_t i;

    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < len; i++)
        a[i] = n;
}

void freeAll(Frontier *frontier, Parents *parents){
    freeFrontier(frontier);
    free(parents);
    const int T = omp_get_max_threads();
    #pragma omp parallel for 
        for (int t = 0; t < T; t++) {
            deallocChunk(local_frontier[t]);
            free(local_frontier[t]);
        }
    free(local_frontier);
    local_frontier = NULL;
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























//EDGES COUNT FUNCTION giusto per compatibilità, ma non contano gli archi a differenza della versione single thread


void classic_top_down_stepAWithEdgesCount(Graph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }
    
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d, degree %d\n", curr_node, curr_node_degree);
                     
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                //printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
    /*printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
}

void top_down_stepAWithEdgesCount(Graph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }
    
    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        //dbg_check_chunk(local_frontier[tid]);     //debug check, stampa 1 se ben allineato, altrimenti 0
        curr_node = frontier->queue->value[i];

        curr_node_degree = graph->out_degree[curr_node];
        //printf("\ncurr_node %d, degree %d\n", curr_node, curr_node_degree);
                     
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];               //nodo raggiunto dal nodo corrente dall'arco edge
                                        //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
                //printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += graph->out_degree[out_edge];                            //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
    /*printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
}

void bottom_up_stepAWithEdgesCount(Graph *graph, Frontier *frontier, Parents* parents, int* visited_edges) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
    /*printf("Bitset frontier: ");
    printBitset(frontier->set);
    printf("\n\n\n");*/

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {       //se non visitato
            curr_node_degree = graph->out_degree[curr_node];
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = graph->all_out_edges[ graph->offset[curr_node] + edge];
                if( is_set(frontier->set, out_edge) ) {
                    //printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, graph->out_degree[curr_node]);
                    //addToFrontier(frontier, curr_node);
                    chunkEnqueue(local_frontier[tid], curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    return ;
}


void top_down_stepLWithEdgesCount(ListGraph *graph, Frontier* frontier, Parents* parents, int* visited_edges) {
    if(local_frontier == NULL) {
        local_frontier = initLocalFrontier(graph->numVertices, LOCAL_FRONTIER_ALLOC_REDUCER);
    }

    int curr_node, curr_node_degree, out_edge;
    size_t i;
    int curr_frontier_size = getFrontierLen(frontier);
    int edges_to_remove = 0;
    
    //RIVEDERE IL BITSET E TUTTO il resto , NON HA SENSO CHE CAMBIO UN BITSET ALLA VOLTA TANTO LA FRONTIERA SI RESETTA OGNI TOP-DOWN, OLTRE CHE OGNI BOTTOM-UP
    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge) reduction(+:edges_to_remove)
    for(i = 0; i < curr_frontier_size; i++) {
        
        int tid = omp_get_thread_num();
        curr_node = frontier->queue->value[i];

        curr_node_degree = getListLen(graph->adj_list[curr_node]);
        LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
        //printf("\ncurr_node %d\n", curr_node);
                        
        for(int edge = 0; edge < curr_node_degree; edge++) {       // se il nodo ha almeno un arco, e per ogni arco del nodo corrente
            out_edge = t->valore;             //nodo raggiunto dal nodo corrente dall'arco edge
            t = t->succ;                            //  non va bene per il parallelo -->  if(getParents(parents, out_edge) == -1) {                        
            if(__sync_bool_compare_and_swap(&parents->array[out_edge], -1, curr_node)) {     //se il nodo non è già stato esplorato (-1) ritorno true e lo modifico (curr_node)
          //      printf("Adds %d --> %d  (thread: %d) (q->slots: %d, slot-len: %d)\n", curr_node, out_edge, tid, local_frontier[tid]->slot, local_frontier[tid]->slot_len);
                edges_to_remove += getListLen(graph->adj_list[out_edge]);                           //mi tengo gli archi già esplorati
                chunkEnqueue(local_frontier[tid], out_edge);
            }
        }
        //total_etr += edges_to_remove;           //addizziono gli edge esplorati al totale
    }
    parents->total_edges_degree -= edges_to_remove;         //rimuovo gli archi 'esplorati' dal num totale
  /*  printf("\n\nLocal frontier: \n");
    for(int t = 0; t < omp_get_max_threads(); t++) {
        printChunk(local_frontier[t]);
        printf("\nchunktQueueLen: %d\n\n", chunkLen(local_frontier[t]));
    }*/
    updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //printf("a2\n");
}

void bottom_up_stepLWithEdgesCount(ListGraph *graph, Frontier *frontier, Parents* parents, int* visited_edges) {
    int curr_node, curr_node_degree, out_edge;
    int curr_frontier_size = getFrontierLen(frontier);
  /*  printf("Bitset frontier: ");
    printBitset(frontier->set);
    printf("\n\n\n");*/

    #pragma omp parallel for schedule(dynamic, NUM_CHUNK) private(curr_node, curr_node_degree, out_edge)
    for (curr_node = 0; curr_node < graph->numVertices; curr_node++) {
        int tid = omp_get_thread_num();

        if(getParents(parents,curr_node) == -1) {       //se non visitato
            curr_node_degree = getListLen(graph->adj_list[curr_node]);
            LinkedList_std *t = getFirstListElem(graph->adj_list[curr_node]);
            for(int edge = 0; edge < curr_node_degree; edge++) {
                out_edge = t->valore;
                t = t->succ;
                if( is_set(frontier->set, out_edge) ) {
                  //  printf("Adds %d --> %d\n", out_edge, curr_node);
                    setParents(parents, curr_node, out_edge, curr_node_degree);
                    //addToFrontier(frontier, curr_node);
                    chunkEnqueue(local_frontier[tid], curr_node);
                    break;                                          //basta un genitore, passo al prossimo nodo
                }
            }
        }
    }
    
    //updateFrontier(frontier);       //inizializza la frontiera, inserendo poi in essa tutti gli elementi in local_frontier
    //frontier->queue->front += curr_frontier_size;      //resetto la frontiera
    return ;
}