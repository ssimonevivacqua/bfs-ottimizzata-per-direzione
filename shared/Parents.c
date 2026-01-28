#include "header.h"

Parents *newParents(int n){
    Parents *p = malloc(sizeof(Parents));
    int *parents = (int *)malloc(sizeof(int)*(n));
    if(parents == NULL) {
        perror("newParents: Runs out of memory!\n\n");
        return NULL;
    }
    p->array = parents;
    p->total_edges_degree = 0;
    return p;
}

void setParents(Parents *p, int a, int b, int a_degree){
    p->array[a] = b;
    p->total_edges_degree -= a_degree;
}


int getParents(Parents *p, int n){
    return p->array[n];
}

void freeParents(Parents *p) {
    free(p->array);
    free(p);
}