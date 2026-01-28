#include "../header.h"
// chunk_padded.h


// Helper: malloc allineata a CACHELINE
static inline void* xaligned_malloc(size_t bytes) {
    void *p = NULL;
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
    if (posix_memalign(&p, CACHELINE, bytes) != 0) return NULL;
#else
    // C11 aligned_alloc richiede size multiplo dell'allineamento.
    size_t sz = (bytes + (CACHELINE - 1)) & ~(size_t)(CACHELINE - 1);
    p = aligned_alloc(CACHELINE, sz);
    if (!p) return NULL;
#endif
    return p;
}


// contatore per debug/telemetria (quante volte espandi)
static int chunkReallocNum = 0;

// Implementazione
Chunk *newChunk(int size) {
    //if (size <= 0) { perror("newchunk size wrong\n") ; return NULL; }
    Chunk *c = (Chunk*) malloc(sizeof(Chunk));     // struct allineata a 64B
    //if (!c) { perror("alloc Chunk"); return NULL; }

    // array di puntatori ai blocchi, allineato (non indispensabile ma pulito)
    c->value = calloc(SLOT_NUM, sizeof(int*));     //per possibili incrementi
    c->value[0] = (int*) xaligned_malloc(size * sizeof(int));
    //if (!c->value[0]) { perror("alloc first slot"); free(c->value); free(c); return NULL; }
    c->slot_len  = size;
    c->slot      = 1; 
    c->curr_size = 0;
    return c;
}

void initChunk(Chunk *c) {
    c->slot = 1;
    c->curr_size = 0;    
    return ;
}

bool chunkIsEmpty(Chunk *c) {
    return c->curr_size == 0 && c->slot <= 1;
}

void chunkEnqueue(Chunk *c, int val) {
    if(c->curr_size == c->slot_len) {
        if(c->slot-1 >= SLOT_NUM)   {     //se supero il limite di slot fermo il programma
            perror("\nError: Not enough slots!! \n\n");
            exit(1);
        }
        c->value[c->slot++] = (int*) xaligned_malloc(c->slot_len * sizeof(int));
        //if (!c->value[c->slot]) { perror("alloc next slot"); exit(1); }
        c->curr_size = 0;
        chunkReallocNum++;
    }
    c->value[c->slot - 1][c->curr_size++] = val;
}

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
    if (!c) return;
    for(int i = 0; i < c->slot; i++)
        free(c->value[i]);
    free(c->value);
    //free(c);
}

int chunkRealloc(){
    return chunkReallocNum;
}
