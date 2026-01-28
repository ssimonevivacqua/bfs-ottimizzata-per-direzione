#include "../header.h"

#define EMPTY_KEY 0xFFFFFFFF  // valore che non può essere una chiave valida
/*
typedef struct {
    uint32_t *keys;
    uint64_t capacity;
} EdgeHash;
*/
void hash_init(EdgeHash *h, uint64_t capacity) {
    h->capacity = capacity;
    h->keys = malloc(sizeof(uint64_t) * capacity);
    for (uint64_t i = 0; i < capacity; i++)
        h->keys[i] = EMPTY_KEY;
}

uint64_t hash_func(uint64_t key, uint64_t capacity) {
    key ^= key >> 30;
    key *= 0xbf58476d1ce4e5b9ULL;
    key ^= key >> 27;
    key *= 0x94d049bb133111ebULL;
    key ^= key >> 31;
    return key % capacity;
}

int hash_lookup(EdgeHash *h, uint64_t key) {
    //printf("hashstrt\n");
    uint64_t idx = hash_func(key, h->capacity);
    uint64_t start = idx;
    while (h->keys[idx] != EMPTY_KEY) {
        if (h->keys[idx] == key) {
            //printf("hashendtrov\n");
            return 1; // trovato
        }
        idx = (idx + 1) % h->capacity; // linear probing
        if (idx == start) break; // girato tutto
    }
//printf("hashendnontrov\n");
    return 0; // non trovato
}

void hash_insert(EdgeHash *h, uint64_t key) {
    uint64_t idx = hash_func(key, h->capacity);
    while (h->keys[idx] != EMPTY_KEY) {
        if (h->keys[idx] == key)
            return; // già presente
        idx = (idx + 1) % h->capacity;
    }
    h->keys[idx] = key;
}

// Codifica arco non orientato
uint64_t encode_edge(int u, int v) {
    return (u < v) ? ((uint64_t)u << 32 | (uint64_t)v) : ((uint64_t)v << 32 | (uint64_t)u);
}
/*
// Nel ciclo Holme–Kim:
uint32_t key = encode_edge(new_node, chosen);
if (!hash_lookup(&hash, key)) {
    add_edge(&graph, &elist, &hash, new_node, chosen);
}

*/