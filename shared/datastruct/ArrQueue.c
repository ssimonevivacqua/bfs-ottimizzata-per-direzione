#include "../header.h"
//coda gestita con array

    //per sperimentazione, tengo quante volte realloco sia con la versione a matrice per le local_frontier che la standard per la frontiera
int arrReallocNum = 0;

ArrQueue *newArrQueue(int size) {
    ArrQueue *q = malloc(sizeof(ArrQueue));
    if(q == NULL) {
        perror("ArrQueue runs out of memory!\n\n");
        return NULL;
    }
    q->value = malloc(sizeof(int)*size);
    q->size = size;
    initArrQueue(q);
}


void initArrQueue(ArrQueue *q) {
    q->front = q->rear = -1;    
    return ;
}


bool isEmptyS(ArrQueue *q) {
    return q->front == -1;
}

int arrEnqueue(ArrQueue *q, int n) {
    /*if (q->rear == q->size) {        //se raggiungo il limite dell'array alloco nuova memoria nello slot successivo disponibile
        q->value = realloc(q->value, sizeof(int)*q->size*2);       //più cache friendly per la frontiera
        arrReallocNum++;
        printf("realloco!\n\n");
    } */

    if (q->front == -1 ) 
        q->front = 0;

    q->value[++q->rear] = n;
    return 1;
}

int arrDequeue(ArrQueue *q) {
    if (isEmptyS(q)) 
        return -1;
    int val = q->value[q->front];
    if( q->front >= q->rear )           //se vuota
        q->front = q->rear = -1;
    else
        q->front++;
    return val;
}

int getFromQueueN(ArrQueue *q, int n) {
    if(n < arrQueueLen(q))
        return q->value[ q->front+n];
}

int arrQueueLen(ArrQueue *q) {
    if(isEmptyS(q))
        return 0;
    return q->rear - q->front + 1;
}

bool arrQueueIsEmpty(ArrQueue *q) {
    if(q->rear - q->front < 0 || isEmptyS(q))
        return true;
    return false;
}

void printArrQueue(ArrQueue *q) {
    int start = q->front;
    printf("\n\n----------------ArrQueue start:\n\n");
    while ( start <= q->rear )
        printf("%d ", q->value[start++]);
    printf("\nArrQueueEnd------------------------\n");
}

void deallocArrQueue(ArrQueue *q) {
    free(q->value);
    free(q);
}

int arrRealloc(){
    return arrReallocNum;
}