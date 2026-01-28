#include "../header.h"

Bitset * newArrBitset(int size) {

    Bitset *bitset = (Bitset *)malloc(sizeof(Bitset));
    if (!bitset) {
        perror("Errore alloc memoria bitset!");
        exit(1);
    }
    
    bitset->size = size*size;
    bitset->bits = (uint8_t *) calloc(((size*size) + 7) / 8, sizeof(uint8_t)); //arrotonda al byte per eccesso (se usavo 32 bit sarebbe stato N + 31 / 32.. etc, qui uso 8 bit)
    if(!bitset->bits) {             //inoltre uso calloc per avere già il bitset inizializzato con tutti 0
        perror("Errore alloc memoria bits bitset!");
        exit(1);
    }
    return bitset;
}
/*

void initBits(Bitset *bitset, int size) {
    if(bitset->bits == NULL && size > 0) {
        bitset->size = size;
        bitset->bits = (uint8_t *) calloc((size + 7) / 8, sizeof(uint8_t)); //arrotonda al byte per eccesso (se usavo 32 bit sarebbe stato N + 31 / 32.. etc, qui uso 8 bit)
        if(!bitset->bits) {             //inoltre uso calloc per avere già il bitset inizializzato con tutti 0
            perror("Errore alloc memoria bits bitset!");
            exit(1);
        }
    }
}
*/
void set_bit_arr(Bitset *set, int x, int y, int offset) {
    //if( i >= set->size)
    /*if( x >= set->size ||) {
        perror("\nset_bit: i > vertices!\n");
        exit(1);
    }*/
    int i = (x * offset) + y;
    set->bits[i / 8] |= (1 << (i % 8));
}

void set_bit_arrDouble(Bitset *set, int x, int y, int offset) {
    //if( i >= set->size)
    /*if( x >= set->size ||) {
        perror("\nset_bit: i > vertices!\n");
        exit(1);
    }*/
    int i = (x * offset) + y;
    set->bits[i / 8] |= (1 << (i % 8));
    int i2 = x + (y * offset);
    set->bits[i2 / 8] |= (1 << (i2 % 8));
}

/*
void clear_bit(Bitset *set, int i) {
    if( i >= set->size) {
        perror("\nclear_bit: i > vertices!\n");
        exit(1);
    }
    set->bits[i / 8] &= ~(1 << (i % 8));  
}
*/
int is_set_arr(Bitset *set, int x, int y, int offset) {
    int i = (x * offset) + y;
    return (set->bits[i / 8] & (1 << (i % 8))) != 0;
    // array [ i / 8]   ----> mi trovo l'indice corretto dell'array, dove andrò a salvare il bit
    // 1 << (i % 8)   ------> sposto a sinistra il bit da settare i % 8 volte, ovvero il resto che mi darà la divisione di i con 8
            // e.g. devo settare il bit 2, si troverà nell'indice 0 dell'array (i / 8 = 0), e si trova nella posizione num2 del byte (2 % 8 = 2   ->> 2 diviso 8 fa 0 con resto di 2)
}
