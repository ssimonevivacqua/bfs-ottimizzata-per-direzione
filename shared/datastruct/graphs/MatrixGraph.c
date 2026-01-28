#include "../../header.h"

#define MAX_SIZE_LIMIT_MTRX (3500000000)
Bitset **used_valuesM;

MatrixGraph *newMtrxGraph() {
    MatrixGraph *graph = (MatrixGraph*)malloc(sizeof(MatrixGraph));
    if(!graph){
        perror("MatrixGraph, newMtrxGraph: Out of memory!\n\n");
        return NULL;
    }
    return graph;
}

int initMtrxGraph(MatrixGraph *graph, int nodes, bool directed) {
    if(graph == NULL){
        perror("MatrixGraph, newMtrxGraph: Out of memory!\n\n");
        return 1;
    }
    graph->numVertices = nodes;
    graph->numEdges = 0;
    long max = (long) nodes* (long) nodes;
    if(max > MAX_SIZE_LIMIT_MTRX) {
        perror("MatrixGraph, initMtrxGraph: Out of memory!\n\n");
        return 1;
    }
    graph->out_degree = (int*)calloc(nodes, sizeof(int));
    graph->adj_matrix = (bool*)calloc(nodes*nodes, sizeof(bool));
    graph->directed = directed;
    if(graph->out_degree == NULL || graph->adj_matrix == NULL) {
        perror("MatrixGraph, initMtrxGraph: Out of memory!\n\n");
        return 1;
    }
    return 0;
}

void initRandMtrxGraph(MatrixGraph *graph, int nodes, bool directed) {
    used_valuesM = malloc(sizeof(Bitset)*nodes);
    int t = 0;
    for (t = 0; t < nodes; t++) 
        used_valuesM[t] = newBitSet(nodes);
    initMtrxGraph(graph, nodes, directed);
    return ;
}

void freeMtrxGraph(MatrixGraph *g) {
    free(g->adj_matrix);
    free(g->out_degree);
    //free(g);
}


//doppia lettura, prima leggo e mi creo l'offeset usando gli out_degree, che poi resetterò a 0, per (e mi conto il numero di edge)
//poi rileggere e inserisco i dati nell'array (grandezza di edges), usando gli out_degree incrementati nell'offset come degli indici
void mtrxEdge(MatrixGraph *graph, int a, int b) {
    int maxNodes = graph->numVertices;
    if( a < 0 || a >= maxNodes || b < 0 || b > maxNodes) {
        perror("\narrEdge: Invalid edge!\n\n");
        exit(1);
    }

    if (graph->adj_matrix != NULL) {
        graph->out_degree[a]++;
        //printf("\narrEdge: %d ---> %d, degree: %d\n", a,b,curr_out_degree);
        graph->adj_matrix[ ( a * graph->numVertices) + b] = 1;
        graph->numEdges++;            //li calcolo solamente durante la generazione dell'array di offset
    }
    return ;
}

void doubleMtrxEdge(MatrixGraph *graph, int a, int b) {
    mtrxEdge(graph, a, b);
    mtrxEdge(graph, b, a);
    //graph->numEdges--;
    return ;
}

/*
void doubleMtrxOutDegreeAdd(MatrixGraph *graph, int a, int b) {
    graph->out_degree[a]++;
    graph->out_degree[b]++;
    graph->numEdges++;
    return ;
}*/

/*      da FINIREEEEEEEEEEEEE
void generateRandOffsetMtrx(MatrixGraph *graph) {
    graph->offset[a]++;
}
*/


/*
void generateMtrxEdge(MatrixGraph *graph, int a, int b) {
    printf("generateMtrxEdge\n\n");
    if (a < graph->numVertices && b < graph->numVertices && a != b) {
        printf("edge %d ---> %d, degree %d\n\n", a, b, graph->out_degree[a]);
        graph->out_edges[a][graph->out_degree[a]]= b;
        //graph->out_edges[b][graph->out_degree[b]]= a;
        graph->out_degree[a]++;
        //graph->out_degree[b] += 1;
        graph->numEdges++;
    }
    return ;
}
*/
/*
void generateRandMtrxGraph(MatrixGraph *g, int nodes){                   //crea un grafo non orientato generico
    if (g != NULL) {
        initMtrxGraph(g, nodes, g->directed);
        //printf("generateRand1\n\n");
        time_t t;
        srand((unsigned) time(&t));
        int randNum;
        for (int i = 0; i < g->numVertices; i++) {
            int edges_num = randEdgesNumber(g->numVertices);
            //g->out_degree[i]= edges_num;
            //printf("generateRandfor1 edges_num %d\n\n", edges_num);
            //g->out_edges[i] = (int*)malloc(sizeof(int)*edges_num);
            for(int i2= 0; i2 < edges_num; i2++) {
                randNum = randNode(g->numVertices, i);
                //printf("generateRandfor2 randNum %d\n\n", randNum);
                if ( i != randNum && !is_set(used_values[i],randNum)) {
                    doubleRandMtrxEdge(g, i, randNum);
                }
            }
        }
    }
}

int randEdgesNumber(int graph_vertices_n) {
    return rand() % ((graph_vertices_n/2) +1);       //tutti i nodi possono avere al massimo la metà dei nodi totali come archi
}

int randNode(int graph_vertices_n, int n) {
    bool check = false;
    int toReturn;
    while (!check) {
        toReturn = rand() % graph_vertices_n;
        if(toReturn != n) {
            check = true;
        }
    }

    return toReturn;         //genero un qualsiasi vertice da 0 ad il numero massimo dei nodi
}
*/
void printMtrxGraph(MatrixGraph *g) {
    printf("\n---------------------- MatrixGraph:\n\n");
    int row,column = g->numVertices;
    row = column;
    for(int i = 0; i < row; i++) {
        printf("Node %d, edges(tot:%d): ",i,g->out_degree[i]);
        for(int i2 = 0; i2 < column; i2++) {
            if(g->adj_matrix[ ( i * row) + i2] == 1)
                printf("%d ", i2);
        }
        printf("\n");
    }
    /*
    for(int i = 0; i < g->numEdges; i++)
        printf("%d ", g->all_out_edges[i]);
        */
    return ;
}