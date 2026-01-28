#include "header.h"

Frontier *newFrontier(int size) {
    Frontier *f = malloc(sizeof(Frontier));
    
    ArrQueue *frontierQueue = newArrQueue(size/FRONTIER_ALLOC_REDUCER);      //genero la coda per la frontiera
    
    Bitset *frontierBitset = newBitSet(size);       //genero il bitset che rappresenta gli elementi contenuti nella frontiera, tutti 0 all'inizio

    f->queue = frontierQueue;
    f->set = frontierBitset;
    //f->len = 0;
    return f;
}

void addToFrontier(Frontier *f, int n) {
    //enqueue(f->queue, n);
    arrEnqueue(f->queue, n);
    set_bit(f->set, n);
    //f->size++;
    return ;
}

int getFromFrontier(Frontier *f) {
    int toReturn = arrDequeue(f->queue);
    //clear_bit(f->set, toReturn);      //non mi interessa farlo, resetto tutto a 0 in modo più efficiente
    //f->size--;
    return toReturn;
}

int getFromFrontierN(Frontier *f, int n) {
    int toReturn = getFromQueueN(f->queue, n);
    return toReturn;
}

int getFrontierLen(Frontier *f) {
    return arrQueueLen(f->queue);
}

bool FrontierIsEmpty(Frontier *f) {
    if (arrQueueIsEmpty(f->queue)) {
        return true;
        printf("\nFrontier is empty!! front: %d\n\n", f->queue->front);
    }
    return false;
}

void FrontierPrint(Frontier *f) {
    printf("\n------ Frontier (len: %d)------------\n", getFrontierLen(f));
    if(!FrontierIsEmpty(f)) 
        printArrQueue(f->queue);
    return ;
}

void NextToFrontier(Frontier *f, ArrQueue *next){
    while ( !FrontierIsEmpty(f)) {
        addToFrontier(f, arrDequeue(next));
    }
}

void FrontierClear(Frontier *f) {
    initArrQueue(f->queue);
    initFrontierBits(f);
}

void initFrontierBits(Frontier *f) {
    free(f->set->bits);
    f->set->bits = NULL;
    initBits(f->set, f->set->size);
}

void setFrontierBitset(Frontier *f) {
       // initFrontierBits(f);
    int fr = f->queue->front;
    int re = f->queue->rear;
    for(fr; fr <= re; fr++) 
        set_bit(f->set, f->queue->value[fr]);
}
/*
void updateFrontierLen(Frontier *f) {
    f->len = ArrQueueLen(f->queue);
}
*/
void deallocFrontierQueue(Frontier *f) {
    free(f->queue->value);
    free(f->queue);
    initFrontierBits(f);
}

void freeFrontier(Frontier *f) {
    deallocArrQueue(f->queue);
    deallocBitset(f->set);
    free(f->set);
    free(f);
    return ;
}
