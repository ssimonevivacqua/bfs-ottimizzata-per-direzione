#include "../../header.h"

Bitset **used_values;

Graph *newArrGraph() {
    Graph *graph = (Graph*)malloc(sizeof(Graph));
    return graph;
}

void initArrGraph(Graph *graph, int nodes, bool directed) {
    graph->numVertices = nodes;
    graph->numEdges = 0;
    graph->out_degree = (int*)calloc(nodes, sizeof(int));
    graph->all_out_edges = NULL;
    //graph->offset = NULL;
    graph->offset = (int*)malloc(sizeof(int)*nodes);
    graph->directed = directed;
    return ;
}

void initOutDegree(Graph * graph) {
    if(graph->out_degree != NULL ){
        free(graph->out_degree);
        graph->out_degree = (int*)calloc(graph->numVertices, sizeof(int));
    }
    return ;
}

int initOffsetArr(Graph * graph){
    if(graph->offset == NULL){
        perror("ArrGraph, initAlloutedges OFFSET: Memory has run out!\n\n");
        return 1;
    }
    graph->offset[0] = 0;
    for(int i = 1; i < graph->numVertices; i++) {
        graph->offset[i] = graph->offset[i - 1] + graph->out_degree[i - 1];
    }
    return 0;
}

int initAllOutEdges(Graph *graph) {
    if(graph->numEdges != 0) {
        graph->all_out_edges = malloc(sizeof(int) * (graph->numEdges));
      //  graph->offset = (int*)malloc(sizeof(int)*graph->numVertices);
    }
    else {
        perror("\nMalloc AllOutEdges: Invalid numEdges!\n\n");
        exit(1);
    }
    if(graph->all_out_edges == NULL){
        perror("ArrGraph, initAlloutedges: Memory has run out!\n\n");
        return 1;
    }
    return 0;
}

void initRandArrGraph(Graph *graph, int nodes, bool directed) {
    used_values = malloc(sizeof(Bitset)*nodes);
    int t = 0;
    for (t = 0; t < nodes; t++) 
        used_values[t] = newBitSet(nodes);
    initArrGraph(graph, nodes, directed);
    return ;
}

void freeArrGraph(Graph *g) {
    free(g->all_out_edges);
    free(g->out_degree);
    if(g->offset != NULL)
        free(g->offset);
    //free(g);
}


//doppia lettura, prima leggo e mi creo l'offeset usando gli out_degree, che poi resetterò a 0, per (e mi conto il numero di edge)
//poi rileggere e inserisco i dati nell'array (grandezza di edges), usando gli out_degree incrementati nell'offset come degli indici
void arrEdge(Graph *graph, int a, int b) {
    int maxNodes = graph->numVertices;
    if( a < 0 || a >= maxNodes || b < 0 || b > maxNodes) {
        perror("\narrEdge: Invalid edge!\n\n");
        exit(1);
    }

    if (graph->all_out_edges != NULL) {
        int curr_out_degree = graph->out_degree[a]++;
        //printf("\narrEdge: %d ---> %d, degree: %d\n", a,b,curr_out_degree);
        graph->all_out_edges[graph->offset[a] + curr_out_degree] = b;
        //graph->numEdges++;            //li calcolo solamente durante la generazione dell'array di offset
    }
    return ;
}
/*
void randArrEdge(Graph *graph, int a, int b) {
    arrEdge(graph, a, b);
    //set_bit(used_values[a],b);
    return ;
}
*/
void doubleArrEdge(Graph *graph, int a, int b) {
    arrEdge(graph, a, b);
    arrEdge(graph, b, a);
    //graph->numEdges--;
    return ;
}
/*
void doubleRandArrEdge(Graph *graph, int a, int b) {
    randArrEdge(graph, a, b);
    randArrEdge(graph, b, a);
    //graph->numEdges--;
    return ;
}*/


void doubleOutDegreeAdd(Graph *graph, int a, int b) {
    graph->out_degree[a]++;
    graph->out_degree[b]++;
    graph->numEdges++;
    return ;
}

/*      da FINIREEEEEEEEEEEEE
void generateRandOffsetArr(Graph *graph) {
    graph->offset[a]++;
}
*/

bool existsEdgeXY(Graph *g, int x, int y) {

    bool toReturn = false;
    for(int i = 0; i < g->out_degree[x]; i++)
        if(g->all_out_edges[ g->offset[x] + i] == y) {
            toReturn = true;
            break;
        }

    return toReturn;
}

/*
void generateArrEdge(Graph *graph, int a, int b) {
    printf("generateArrEdge\n\n");
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
*//*
void generateRandArrGraph(Graph *g, int nodes){                   //crea un grafo non orientato generico
    if (g != NULL) {
        initArrGraph(g, nodes, g->directed);
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
                    doubleRandArrEdge(g, i, randNum);
                }
            }
        }
    }
}*/
/*
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
void printArrGraph(Graph *g) {
    printf("\n---------------------- Graph:\n\n");
    
    for(int i = 0; i < g->numVertices; i++) {
        printf("Node %d, edges(tot:%d): ",i,g->out_degree[i]);
        for(int i2 = 0; i2 < g->out_degree[i]; i2++) {
            printf("%d ", g->all_out_edges[ g->offset[i] + i2 ]);
        }
        printf("\n");
    }
    /*
    for(int i = 0; i < g->numEdges; i++)
        printf("%d ", g->all_out_edges[i]);
        */
    return ;
}