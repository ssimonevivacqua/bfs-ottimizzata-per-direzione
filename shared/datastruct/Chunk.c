#include "../header.h"
//chunk list gestito con matrice, non uso più una coda in cuanto in parallelo è spreco di risorse

int chunkReallocNum = 0;        //per sperimentazione, tengo cuante volte realloco sia con la versione a matrice per le local_frontier che la standard per la frontiera

Chunk *newChunk(int size) {
    Chunk *c = malloc(sizeof(Chunk));
    c->value = calloc(SLOT_NUM, sizeof(int*));     //per possibili incrementi
    c->value[0] = malloc(sizeof(int)*size);         //inizializzo solo uno slot 
    c->slot_len = size;
    initChunk(c);
}

void initChunk(Chunk *c) {
    c->slot = 1;
    c->curr_size = 0;    
    return ;
}

bool chunkIsEmpty(Chunk *c) {
    return c->curr_size == 0 && c->slot <= 1;
}
/*
void chunkEncueue(ChunkQueue *c, int n) {
    if (c->rear == c->slot_len -1) {        //se raggiungo il limite dell'array alloco nuova memoria nello slot successivo disponibile
        if(c->slot-1 >= SLOT_NUM)   {     //se supero il limite di slot fermo il programma
            perror("\nError: Not enough slots!!\n\n");
            exit(1);
        }
        c->value[c->slot++] = malloc(sizeof(int)*c->slot_len);
        //c->slot+= 1;  //incremento da sopra
        chunkReallocNum++;
        c->front = c->rear = -1;   
    } 

    if (c->front == -1 ) 
        c->front = 0;

    c->value[c->slot-1][++c->rear] = n;
}
*/

void chunkEnqueue(Chunk *c, int val) {
    if(c->curr_size == c->slot_len) {
        if(c->slot-1 >= SLOT_NUM)   {     //se supero il limite di slot fermo il programma
            perror("\nError: Not enough slots!! \n\n");
            exit(1);
        }
        c->value[c->slot++] = malloc(sizeof(int)*c->slot_len);
        //c->slot+= 1;  //incremento da sopra
        chunkReallocNum++;
        c->curr_size = 0; 
    } 
    c->value[c->slot-1][c->curr_size++] = val;
}

/*int getChunkValue(Chunk *c, int index) {
    int opt_index = index - ((c->slot_len)*(c->slot-1));
    return c->value[opt_index];
}*/

int chunkLen(Chunk *c) {
    if(chunkIsEmpty(c)) {
        //printf("\nchunkQLen is empty!\n\n");
        return 0;
    }
    int total = 0;
    for(int i = c->slot-1; i > -1; i--) {
        if( i == c->slot-1)
            total+= c->curr_size;
        else 
            total+= c->slot_len;
    }
    return total;
}

void printChunk(Chunk *c) {
    if(chunkIsEmpty(c)){
        printf("printChunkQueue: empty!\n\n");
        return;
    } else {
        printf("\n\n----------------MtrQueue start:\n\n");
        for(int i = 0; i < c->slot; i++) {
            if(i == c->slot-1) {
                for(int i2 = 0; i2 < c->curr_size; i2++)
                    printf("%d ", c->value[i][i2]);
            }
            else {
                for(int i2 = 0; i2 < c->slot_len; i2++)
                    printf("%d ", c->value[i][i2]);
            }
        }
    }
    printf("\nChunkQueueEnd---------------------\n");
}

void deallocChunk(Chunk *c) {
    for(int i = 0; i < c->slot; i++)
        free(c->value[i]);
    free(c->value);
    //free(c);
}

int chunkRealloc(){
    return chunkReallocNum;
}
