#include "../header.h"

LinkedList *NewList(){

    LinkedList_std *list_Start = malloc(sizeof(LinkedList_std));
    if(!list_Start){         //t == NULL
        perror("LinkedList, NewList: Runs out of memory!\n\n");
        return NULL;
    }
  /*  LinkedList_std *list_End = malloc(sizeof(LinkedList_std));
    if(!list_End){         //t == NULL
        perror("LinkedList, NewList: Runs out of memory!\n\n");
        return NULL;
    }*/
    list_Start->valore = 0;
    list_Start->succ = NULL;
    list_Start->prec = NULL;
  //  list_Start->valore = 0;
  //  list_End->prec = list_Start;
  //  list_End->succ = NULL;

    LinkedList *list = malloc(sizeof(LinkedList));
    if(!list){         //t == NULL
        perror("LinkedList, NewList: Runs out of memory!\n\n");
        return NULL;
    }
    list->start = list_Start;
    list->end = list_Start;
    return list;
}


int addListElem(LinkedList *a, int v){
    LinkedList_std *t = malloc(sizeof(LinkedList_std));
    if(!t){         //t == NULL
        perror("LinkedList, addListElem: Runs out of memory!\n\n");
        return 1;
    }
	t->valore = v;
	t->succ = NULL;
    t->prec = a->end;

    a->end->succ = t;

    a->end = t;
    a->start->valore++;     //numero gli elementi contenuti
    return 0;
}

int getListLen(LinkedList *a) {
    return a->start->valore;
}

void ListPrint(LinkedList *a){
    LinkedList_std *t = a->start->succ;
    if (t == NULL)
        printf("LIST   empty!");
    else 
        while ( t != NULL) {
            printf("%d ", t->valore);
            t = t->succ;
    }
    printf("\n\n");
}

bool IsListEmpty(LinkedList *a){
    if (a == NULL) {
        printf("Error: LIST NOT INIZIALIZED");
    }
    else {
        if (a->start->succ == NULL) //non succ succ in quanto end non è un dummy, solo start
            return true;
        else 
            return false;
    }
}

bool ListContains(LinkedList* a, int n) {
    LinkedList_std *t = a->start->succ;
    if (t == NULL)
        return false;
    else 
        while ( t != NULL) {
            if(t->valore == n) 
                return true;
            t = t->succ;
    }
    return false;
}

LinkedList_std *getFirstListElem(LinkedList *l) {
    if(l->start != NULL)
        return l->start->succ;
}

int popListElem(LinkedList* a) {
    LinkedList_std *t = a->start->succ;
    int val = 0;
    //LinkedList_std *toDealloc = NULL;
    if(t->succ != NULL) {
        a->start->succ = t->succ;
        t->succ->prec = NULL;
        //toDealloc = t;
        //t = t->succ;
        val = t->valore;
        a->start->valore--;
        free(t);
    }
    /*else {
        perror("popListElem: vuoto!");
        exit(1);
    }*/
    return val;
}

void freeList(LinkedList* a) {
    while( !IsListEmpty(a)) {
        LinkedList_std *t = a->start->succ;
        a->start->succ = t->succ;
        free(t);
    }
    free(a->start);
 //  free(a->end);     //lo già dealloco, non è dummy l'ultimo elemento!
    free(a);
/*
    LinkedList_std *t = a->start;
    while(t != NULL) {
        LinkedList_std *next = t->succ;
        free(t);
        t = next;
    }
    free(a);
    */
}