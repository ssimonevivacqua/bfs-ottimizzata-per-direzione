//#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include <assert.h>
#include <stdalign.h>

#define graph_vertices (4)      //usato solo per grafi randomici
#define SLOT_NUM (8)
#define LOCAL_FRONTIER_ALLOC_REDUCER (8)
#define FRONTIER_ALLOC_REDUCER (1)
#define NUM_CHUNK (4)
#ifndef CACHELINE           //VERIFICA SE GIA' ESISTE UN CHACHELINE, SE NON ESISTE LO INSERISCE, BUONA PRATICA
#define CACHELINE 64
#endif

//--------Standard data's structures

struct ArrQueue {           //coda gestita con array ed indici, realloc in caso si raggiunga la fine
    int *value;
    //int **value;
    int front, rear, size;
    //int slot_len, slot;
};
typedef struct ArrQueue ArrQueue;

struct Chunk {        //(chunk list)matrice a blocchi per la frontiera locale, ottimo per grafi molto grandi e sparsi, non utilizza realloc
    int **value;
    int curr_size;
    int slot_len, slot;
};
typedef struct Chunk Chunk;


/*
// Metadati "freddi" (raramente aggiornati) + posizionamento della parte "calda", curr_size per primo
typedef struct{
    // --- parte "calda" (su una cache line dedicata) ---
    int curr_size; // quanti elementi usati nel blocco corrente
    //char _pad_hot[CACHELINE - sizeof(int)]; //riempi il resto della cacheline
    // --- parte "fredda" (entra nella prima cache line) ---
    int   **value;     // array di puntatori ai blocchi
    int     slot_len;  // capacità di ciascun blocco (in int)
    int     slot;      // quanti blocchi sono allocati (>=1)
} Chunk;
*/
struct LinkedList_std {
    int valore;
    struct LinkedList_std *succ;
    struct LinkedList_std *prec;
};
typedef struct LinkedList_std LinkedList_std;

struct LinkedList {
    struct LinkedList_std *start;
    struct LinkedList_std *end;
};
typedef struct LinkedList LinkedList;

struct Bitset{
    uint8_t *bits;
    size_t size;
};
typedef struct Bitset Bitset;

//-------------- END 


struct Frontier {
    struct ArrQueue* queue;
    struct Bitset* set;
};
typedef struct Frontier Frontier;

struct Parents {
    int *array;
    long total_edges_degree;
};
typedef struct Parents Parents;

struct Heuristic {
    int frontier_size;         //grandezza della frontiera corrente
    int frontier_prev_size;    //grandezza della frontiera dell'iterazione precedente alla corrente 
    int edges_frontier_size;   //out-degree dei nodi in frontier
    int edges_parents_size;    //out-degree dei nodi non ancora esplorati
};
typedef struct Heuristic HeuristicData;


//------------ Graphs data's structure

struct Graph {
    int numVertices;
    long numEdges;
    int *out_degree, *all_out_edges, *offset;
    bool directed;
    //int *in_degree, **in_edges;
};
typedef struct Graph Graph;


struct ListGraph {
    int numVertices;
    long numEdges;
    LinkedList ** adj_list;
    bool directed;
};
typedef struct ListGraph ListGraph;


struct MatrixGraph {
    int numVertices;
    long numEdges;
    bool *adj_matrix;
    int *out_degree;
    bool directed;
    //matrix
};
typedef struct MatrixGraph MatrixGraph;


//-------depth: -----------------//
struct VerticesDepth {
    int depth;
    int *array_depth;
};
typedef struct VerticesDepth VerticesDepth;

int* aBFSHybridSearch_verify(Graph *, int, bool, VerticesDepth*);
void top_down_stepA_verify(Graph * , Frontier*, Parents*, VerticesDepth*);
void bottom_up_stepA_verify(Graph * , Frontier*, Parents*, VerticesDepth*);
VerticesDepth* initNewDepth(int, int);
//--------------------------------------//

//---------------------------------studio dei risultati-----------------//
struct Research {
    double max, min, average;
    double *exec_times;
    int source_num;
    int *sources;
};
typedef struct Research Research;

int* BFSResearch_arr(Graph *, char* , bool );
int* BFSResearch_list(ListGraph *, char* , bool );
int* BFSResearch_mtrx(MatrixGraph *, char* , bool );
Research *BFSResearch(char* );
int getSourceNum(char* );
void printResearchResults(Research *);
void getAverage(Research *);
void checkMinMax(Research* , int);
Research *initResearch(int );

//------------------------------------------------------------------------------------_//


//----------------------------studio delle iterazioni----------------------//
struct Iteration{
    int top_down_step_count, bottom_up_step_count;
};
typedef struct Iteration Iteration;

struct Iterations{
    Iteration *iter;
    int curr_iter;
};
typedef struct Iterations Iterations;

int* aBFSHybridSearch_study(Graph *, int, bool, Iterations*);
int* LBFSHybridSearch_study(ListGraph *, int , bool , Iterations *);
Iterations *initIterations(int );
void printCurrIteration(Iterations *);
void printIterations(Iterations *);

void freeMultipleSourceSearch(Research *, Iterations*); 

int generateSources(char*, int , int );

int ResultSum(int , int , int , bool );
void AverageOnlySearch(char** , int , int , bool, bool );
void SingleGraphFullSearch(char** , int , int , bool, bool );
void SingleGraphMultiFullSearch(char** , int , int , bool , bool );
void MinMaxSearch(char** , int , int , bool, bool );
//------------------------------------------------ DICHIARO FUNZIONI ------------------------------------------------------//

//--------------- main.c

void deallocMain(Graph *, int *);
void printArr(int *, int);


//---------------- ImportGraph.c

int ImportArrayGraph(Graph *, char* , bool);
int ImportListGraph(ListGraph *, char* , bool);
//int ImportMatrixGraph(MatrixGraph *, char* , bool);
int ImportUndirectedGraph(Graph *, char*);
int ImportDirectedGraph(Graph *, char*);
int ImportUndirectedListGraph(ListGraph *, char*);
int ImportDirectedListGraph(ListGraph *, char*); 
int ImportUndirectedMtrxGraph(MatrixGraph* , char*);
int ImportDirectedMtrxGraph(MatrixGraph *, char*);
void initOutdegrees(Graph * , char* );
void printfArrOffset(int* , int );
void printfAddOper(int, int);
FILE * fileOpen(char*);


//----------------------GraphGenerator.c
void generateRandUndirectedGraph(char *, int , double , int,  int );
void generateRandUndirectedGraphWithDDiameter(char *, int , double , int, int );
int generateRandUndirectedGraphWithDDiameterCore(char *, int , double , int , int );
void generateBarabasiUndirectedGraph(char *, int );
void generateWattsUndirectedGraphs(char *, int );
int choose_node(int *, int , int );
int randEdgesNumber(int );
int randNode(int , int);

void init_genrand(unsigned long );
unsigned long genrand_int32(void);
int random_between(int, int );



//----------------------------- DO-BFS_core.c & DO-BFS_core_p.c

    //---------------   Array Hybrid BFS
        //main (ArrayDO-BFS.c)
        int* aBFSHybridSearch(Graph *, int, bool);
        int* aBFSHybridSearchWithEdgesCount(Graph *, int, bool, int*);        //with edges count
    
    void top_down_stepA(Graph * , Frontier*, Parents*);
    void bottom_up_stepA(Graph * , Frontier*, Parents*);
    void classic_top_down_stepA(Graph *, Frontier* , Parents* );
    void top_down_stepAWithEdgesCount(Graph * , Frontier*, Parents*, int*);
    void bottom_up_stepAWithEdgesCount(Graph * , Frontier*, Parents*, int*);
    void classic_top_down_stepAWithEdgesCount(Graph *, Frontier* , Parents* , int*);
    void updateSizesA(  Graph *, Frontier *, Parents*, HeuristicData*);
    int getFrontierEdgesA ( Frontier *, Graph *);   

    //--------------------- List Hybrid BFS
        //main (ListDO-BFS.c)
        int* LBFSHybridSearch(ListGraph *, int, bool);
        int* LBFSHybridSearchWithEdgesCount(ListGraph *, int, bool, int*);        //with edges count

    void top_down_stepL(ListGraph *, Frontier* , Parents* );
    void bottom_up_stepL(ListGraph *, Frontier *, Parents* );
    void top_down_stepLWithEdgesCount(ListGraph *, Frontier* , Parents*, int* );
    void bottom_up_stepLWithEdgesCount(ListGraph *, Frontier *, Parents*, int* );
    void updateSizesL( ListGraph *, Frontier *, Parents *, HeuristicData *);
    int getFrontierEdgesL( Frontier *, ListGraph *);
    //--------------------- Matrix Hybrid BFS
        //main (MatrixDO-BFS.c)
        int* MBFSHybridSearch(MatrixGraph *, int, bool);
    //    int* MBFSHybridSearchWithEdgesCount(MatrixGraph *, int, bool, int*);

    void top_down_stepM(MatrixGraph *, Frontier* , Parents* );
    void bottom_up_stepM(MatrixGraph *, Frontier *, Parents* );
    void updateSizesM( MatrixGraph *, Frontier *, Parents *, HeuristicData *);
    int getFrontierEdgesM( Frontier *, MatrixGraph *);


void updateFrontierTopDown(Frontier *,ArrQueue *); 
void updateFrontierBottomUp(Frontier *, ArrQueue *); 
bool next_direction( HeuristicData* , bool, int, bool);
void initParents(Parents *, int , long , int );
void initHeuristic(HeuristicData*);
void freeAll(Frontier *, Parents *);
void setArrayToN(int*, int, int);
void initFrontier(Frontier*, int);

    //parallel add-on
Chunk **initLocalFrontier(int, int );
void updateFrontier(Frontier *);
void printLocalFrontier();

ArrQueue **initLocalFrontier2(int, int);
void updateFrontier2(Frontier *); 
 









//----------------------------- Frontier

Frontier *newFrontier(int );
void addToFrontier(Frontier *, int );
int getFromFrontier(Frontier *);
int getFromFrontierN(Frontier *, int );
int getFrontierLen(Frontier *);
bool FrontierIsEmpty(Frontier *);
void FrontierPrint(Frontier *);
void NextToFrontier(Frontier *, ArrQueue *);
void FrontierClear(Frontier *);
void freeFrontier(Frontier *);
void initFrontierBits(Frontier *);
void deallocFrontierQueue(Frontier *);
//void updateFrontierLen(Frontier *);
void setFrontierBitset(Frontier*);




//---------------------------- Parents

Parents *newParents(int );
void setParents(Parents *, int , int , int);
//void setParentsM(MatrixGraph *, Parents *, int , int );
int getParents(Parents *, int );
void freeParents(Parents *);



//----------------------------------------------- GRAPHS -----------------------------------//

//----------------  ArrayGraph

Graph *newArrGraph();
void initArrGraph( Graph*, int, bool);
void initOutDegree(Graph *);
int initOffsetArr(Graph * );
int initAllOutEdges(Graph *);
void freeArrGraph(Graph *);
void doubleOutDegreeAdd(Graph *, int , int ); 
void arrEdge(Graph *, int , int );
void doubleArrEdge(Graph *, int, int);
void printArrGraph(Graph *);
bool existsEdgeXY(Graph *, int , int );


//------------------- ListGraph

ListGraph *newListGraph();
int initListGraph( ListGraph*, int, bool);
void freeListGraph(ListGraph *);
int listEdge(ListGraph *, int , int );
int doubleListEdge(ListGraph *, int, int);
void printListGraph(ListGraph *);


//------------------- MatrixGraph

MatrixGraph *newMtrxGraph();
int initMtrxGraph( MatrixGraph*, int, bool);
void freeMtrxGraph(MatrixGraph *);
void mtrxEdge(MatrixGraph *, int , int );
void doubleMtrxEdge(MatrixGraph *, int, int);
void printMtrxGraph(MatrixGraph *);


//---------------------- ArrQueue 

ArrQueue* newArrQueue(int );
void initArrQueue(ArrQueue *);
bool isEmptyS(ArrQueue *);
int arrQueueLen(ArrQueue *);
int arrEnqueue(ArrQueue *, int );
int arrDequeue(ArrQueue *);
int getFromQueueN(ArrQueue *, int );
void printArrQueue(ArrQueue *); 
void deallocArrQueue(ArrQueue *); 
bool arrQueueIsEmpty(ArrQueue *);
int arrRealloc();


//----------------- LinkedList

LinkedList *NewList();
int addListElem(LinkedList *, int);
int getListLen(LinkedList *);
void ListPrint(LinkedList *);
bool IsListEmpty(LinkedList *);
bool ListContains(LinkedList *, int);
LinkedList_std *getFirstListElem(LinkedList *);
int popListElem(LinkedList* );
void freeList(LinkedList* );


//---------------------- Chunk
Chunk *newChunk(int );
void initChunk(Chunk *) ;
void chunkEnqueue(Chunk *, int);
int chunkLen(Chunk *);
bool chunkIsEmpty(Chunk *);
void printChunk(Chunk *); 
void deallocChunk(Chunk *); 

int chunkRealloc();

/*
// Prototipi new
Chunk* newChunk(int size);
void   initChunk(Chunk *c);
bool   chunkIsEmpty(const Chunk *c);
void   chunkEnqueue(Chunk *c, int val);
int    chunkLen(const Chunk *c);
void   printChunk(const Chunk *c);
void   deallocChunk(Chunk *c);
int    chunkRealloc(void);
*/
//------------------ Bitset

Bitset *newBitSet(int );
void initBits(Bitset *, int );
void set_bit(Bitset *, int );
void clear_bit(Bitset *, int );
int is_set(Bitset *, int );
void printBitset(Bitset * );
void deallocBitset(Bitset *); 


//------------------ ArrBitset
Bitset * newArrBitset(int );
void set_bit_arr(Bitset *, int , int, int );
void set_bit_arrDouble(Bitset *, int , int, int );
int is_set_arr(Bitset *, int , int, int );





// CODIFICA PER USO HASH, necessaria per verifica duplicati archi e per barabasi Holme--Kim
typedef struct {
    uint32_t *keys;
    uint32_t capacity;
} EdgeHash;

void hash_init(EdgeHash *, uint64_t );
uint64_t hash_func(uint64_t , uint64_t );
int hash_lookup(EdgeHash *, uint64_t );
void hash_insert(EdgeHash *, uint64_t );
uint64_t encode_edge(int , int );