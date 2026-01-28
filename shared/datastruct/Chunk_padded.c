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
    if (size <= 0) { perror("newchunk size wrong\n") ; return NULL; }
    Chunk *c = (Chunk*) xaligned_malloc(sizeof(Chunk));     // struct allineata a 64B
    if (!c) { perror("alloc Chunk"); return NULL; }

    // array di puntatori ai blocchi, allineato (non indispensabile ma pulito)
    c->value = (int**) xaligned_malloc(sizeof(int*) * SLOT_NUM);
    if (!c->value) { perror("alloc value[]"); free(c); return NULL; }
    memset(c->value, 0, sizeof(int*) * SLOT_NUM);
    // primo blocco, allineato a 64B
    c->value[0] = (int*) xaligned_malloc((size_t)size * sizeof(int));
    if (!c->value[0]) { perror("alloc first slot"); free(c->value); free(c); return NULL; }

    c->slot_len  = size;
    c->slot      = 1; 
    c->curr_size = 0;
    return c;
}

void initChunk(Chunk *c) {
    c->slot      = 1;    // c'è il blocco 0
    c->curr_size = 0;    // ancora vuoto
}

bool chunkIsEmpty(const Chunk *c) {
    // vuoto se ho solo il primo blocco e curr_size == 0
    return (c->slot == 1) && (c->curr_size == 0);
}

void chunkEnqueue(Chunk *c, int val) {
    // se il blocco corrente è pieno, apri un nuovo slot
    if (c->curr_size == c->slot_len) {
        if (c->slot >= SLOT_NUM) {
            fprintf(stderr, "Error: Not enough slots (SLOT_NUM=%d)\n", SLOT_NUM);
            exit(1);
        }
        c->value[c->slot] = (int*) xaligned_malloc((size_t)c->slot_len * sizeof(int));
        if (!c->value[c->slot]) { perror("alloc next slot"); exit(1); }
        c->slot += 1;
        c->curr_size = 0;
        chunkReallocNum++;
    }
    c->value[c->slot - 1][c->curr_size++] = val;
}

int chunkLen(const Chunk *c) {
    // tutti i blocchi pieni (slot-1)*slot_len + l'ultimo parziale curr_size
    return (c->slot - 1) * c->slot_len + c->curr_size;
}

void printChunk(const Chunk *c) {
    if (chunkIsEmpty(c)) {
        printf("printChunk: empty\n");
        return;
    }
    printf("\n--- Chunk dump ---\n");
    for (int i = 0; i < c->slot; ++i) {
        int lim = (i == c->slot - 1) ? c->curr_size : c->slot_len;
        for (int k = 0; k < lim; ++k) {
            printf("%d ", c->value[i][k]);
        }
    }
    printf("\n------------------\n");
}

void deallocChunk(Chunk *c) {
    if (!c) return;
    for (int i = 0; i < c->slot; ++i) free(c->value[i]);
    free(c->value);
    //free(c);
}

int chunkRealloc() { return chunkReallocNum; }