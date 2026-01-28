#include "../header.h"
//#include <sys/resource.h>
                            //EDGES
#define EDGEFACTOR (5)        // x*n
#define LOG_TOGGLE (0)          //0 -> off, 1 -> on
#define POWER (1)              // n^x
#define MULTIPLIER (10)         // 100, + 500 + 500 etc 
#define SOURCES_PER_SEARCH (50) //numero di ricerche con diverse sorgenti generate per grafo
#define REPEAT (1) //quante ripetizioni dell'algo 

#define BA (1)      //se on genera grafi scale-free (pochi grossi hub)
#define NW (0)      //se on genera grafi small-world
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//- Esegue la Visita in Ampiezza su di molteplici grafi crescenti su multeplici algoritmi di ricerca, memorizzando le sorgenti di input, uguali per ogni algoritmo --//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//ATTENZIONE, IL CONTATORE DEGLI ARCHI ESPLORATI FUNZIONA SOLO CON LA VERSIONE NON PARALLELA----
 //   gcc -o MES MultipleExamplesSearch.c GraphGenerator.c SourceGenerator.c ResultsSum.c mt19937ar.c hashkey.c ../../DO-BFS_core.c ../../ArrayDO-BFS.c ../../ListDO-BFS.c ../../MatrixDO-BFS.c ../header.h ../Frontier.c ../Parents.c GraphImporter.c ../datastruct/graphs/ArrayGraph.c ../datastruct/graphs/ListGraph.c ../datastruct/graphs/MatrixGraph.c ../datastruct/ArrQueue.c ../datastruct/LinkedList.c ../datastruct/Bitset.c ../datastruct/ArrBitset.c
                                                        //(()) -lm alla fine per attivare le funzioni di math.h (())
//gcc -o MESp -fopenmp MultipleExamplesSearch.c GraphGenerator.c SourceGenerator.c ResultsSum.c mt19937ar.c hashkey.c ../../DO-BFS_core_p.c ../../ArrayDO-BFS.c ../../ListDO-BFS.c ../../MatrixDO-BFS.c ../header.h ../Frontier.c ../Parents.c GraphImporter.c ../datastruct/graphs/ArrayGraph.c ../datastruct/graphs/ListGraph.c ../datastruct/graphs/MatrixGraph.c ../datastruct/ArrQueue.c ../datastruct/LinkedList.c ../datastruct/Chunk.c ../datastruct/Bitset.c ../datastruct/ArrBitset.c

//gcc -o MESp -fopenmp MultipleExamplesSearch.c GraphGenerator.c SourceGenerator.c ResultsSum.c mt19937ar.c hashkey.c ../../DO-BFS_core_p.c ../../ArrayDO-BFS.c ../../ListDO-BFS.c ../../MatrixDO-BFS.c ../header.h ../Frontier.c ../Parents.c GraphImporter.c ../datastruct/graphs/ArrayGraph.c ../datastruct/graphs/ListGraph.c ../datastruct/graphs/MatrixGraph.c ../datastruct/ArrQueue.c ../datastruct/LinkedList.c ../datastruct/Chunk_softpadded.c ../datastruct/Bitset.c ../datastruct/ArrBitset.c

/*
          path                  start num nodes & numer di grafi usati
    ./MES ../../../grafi/generati/ 10 5 1 0 0

        sudo sysctl vm.swappiness=5 //per impostare l'uso dello swap solo quando necessaria, 60 di default

    //check memory leak
        valgrind --leak-check=full ./MES ../../../grafi/generati/ 100000 2 0 0 0 
*/

/*
void limitMemory(size_t maxBytes) {
    struct rlimit rl;
    rl.rlim_cur = maxBytes;
    rl.rlim_max = maxBytes;
    if(setrlimit(RLIMIT_AS, &rl) != 0) {
        perror("setlimit vmemory");
        exit(1);
    }
}

void limitMemoryStack(size_t maxBytes) {
    struct rlimit rl;
    rl.rlim_cur = maxBytes;
    rl.rlim_max = maxBytes;
    if(setrlimit(RLIMIT_STACK, &rl) != 0) {
        perror("setlimit stack");
        exit(1);
    }
}
*/

void checkSwap(FILE *fPointer) {
    FILE *fp = fopen("/proc/self/status", "r");
    if(!fp) {
        perror("Cant open the file!\n\n");
        return ;
    }

    char line[256];
    while(fgets(line, sizeof(line), fp)){
        if(strncmp(line, "VmSwap:", 7) == 0) {
            fclose(fp);
            long swap_kb = 0;
            sscanf(line + 7, "%ld", &swap_kb);
            fprintf(fPointer, "%d\n", (int)(swap_kb > 0));
            return ;
        }
    }
    fclose(fp);
}

void nGenerateGraphs(char *directory, int n, int num_nodes) {
    //return ;            //DISABILITO GENERAZIONE GRAFI
    //int num_nodes = 10;
    char filepath[256]; //buffer
    int multi = num_nodes*MULTIPLIER;
    for (int i = 0; i < n; i++) {
        snprintf(filepath, sizeof(filepath), "%sgraph_%d.txt", directory, num_nodes);

        if(BA){
            generateBarabasiUndirectedGraph(filepath, num_nodes);   //grafi scale free
        }
        else if(NW)
            generateWattsUndirectedGraphs(filepath, num_nodes);     //grafi small world
        else
            generateRandUndirectedGraph(filepath, num_nodes, EDGEFACTOR, LOG_TOGGLE, POWER);

        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        
        //num_nodes += 1200;        //per mdalita 1, stesso incremento per parte finale, partitoda 22000
    }
}

void main(int argc, char ** argv) {
    int start_nodes, n, hybrid_int, generator_int, mode, vmemory_limited;
    bool hybrid, generator;

    if ( argc < 7) {
        perror("\nInvalid input!\n\n");
        exit(1);
    }
    else {
        if( !sscanf(argv[2],"%d", &start_nodes) || !sscanf(argv[3], "%d", &n) || !sscanf(argv[4], "%d", &hybrid_int) || 
              !sscanf(argv[5], "%d", &mode) || !sscanf(argv[6], "%d", &vmemory_limited) || !sscanf(argv[7], "%d", &generator_int)) {
            perror("\nInvalid input integer for the search/graph type!\n\n");
            exit(1);
        }
        //if( vmemory_limited == 1) {
        //    limitMemory(13L * 1024L * 1024L * 1024L + 512L * 1024L * 1024L + 1024L * 1024L * 8);    //virtual memory limitato a 14gb+stack    (+ 512L * 1024L * 1024L) per aggiungerne altri 0,5gb
        //    limitMemoryStack(1024L * 1024L * 8L);                //stack limitato a 8mb, come di default
        //}
        if( hybrid_int == 0 || hybrid_int == 1) 
            hybrid = hybrid_int;
        else {
            perror("\nInvalid hybrid choice!\n\n");
            exit(1);
        }
        if( generator_int == 0 || generator_int == 1) 
            generator = generator_int;
        else {
            perror("\nInvalid generator choice! 0 or 1!\n\n");
            exit(1);
        }
    }
    if (mode == 0)
        AverageOnlySearch(argv, start_nodes, n, hybrid, generator);
    else if (mode == 1)
        MinMaxSearch(argv, start_nodes, n, hybrid, generator);
    else if (mode == 2)
        SingleGraphFullSearch(argv, start_nodes, n, hybrid, generator);
    else if (mode == 3)
        SingleGraphMultiFullSearch(argv, start_nodes, n, hybrid, generator);
}

void AverageOnlySearch(char** argv, int start_nodes, int n, bool hybrid, bool generator) {
    //------------------------------ REPEAT CICLE -------------------------------------------------------------------------------------------
    for(int repeats=0; repeats < REPEAT; repeats++) {

    printf("Ricerca tramite le 3 strutture dati su %d grafi con nodi iniziali=%d, crescente (%dn), Start!\n\n", n, start_nodes, MULTIPLIER);
    //printf("ok0\n");
    if(generator)
        nGenerateGraphs(argv[1], n, start_nodes);



    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./avgresults/temp/results%d-%d-%d.csv", start_nodes, (int) hybrid, repeats);
    FILE *filePointer = fopen(filePath, "w");
    //FILE *filePointer = fopen("results.txt", "w");
    //char filepath[256]; //buffer
    //char sourceFilepath[256];
    int num_nodes = start_nodes;
    int multi = num_nodes*MULTIPLIER;

    //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/swap/results%d-a%d.csv", start_nodes, (int) hybrid);
    FILE *swapPointer = fopen(filePath, "w");

    //init Edges count
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/results%d-a%d-edges.csv", start_nodes, (int) hybrid);
    FILE *numOfEdgesPointer = fopen(filePath, "w");

    Graph *graph0 = newArrGraph();
    for(int i = 0; i < n; i++) {

        int research_iter_num;
        int *parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;
        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        if(generator)
            generateSources(filePath, num_nodes, research_iter_num);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);

        if( ImportUndirectedGraph(graph0, filePath) == 1){
            perror("main: out of memory arrgraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }
 

        int source = 0, research_count = 0, visited_edges = 0;
        clock_t begin = clock();
        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("aBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
            //parentsArray = aBFSHybridSearch(graph0, source, hybrid);
            parentsArray = aBFSHybridSearchWithEdgesCount(graph0, source, hybrid, &visited_edges);
            if(parentsArray == NULL) {
                perror("main: out of memory abfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
                fprintf(numOfEdgesPointer, "%d\n", visited_edges);
                visited_edges = 0;
            }
        }
        clock_t end = clock();

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL) {
            freeArrGraph(graph0);
            break;
        }
        fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche

        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeArrGraph(graph0);
    }
    fprintf(filePointer, "\n");
    //fclose(filePointer);
    fclose(swapPointer);
    fclose(numOfEdgesPointer);
    free(graph0);


    num_nodes = start_nodes;


    //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/swap/results%d-l%d.csv", start_nodes, (int) hybrid);
    swapPointer = fopen(filePath, "w");

    //init Edges count
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/results%d-l%d-edges.csv", start_nodes, (int) hybrid);
    numOfEdgesPointer = fopen(filePath, "w");

    ListGraph *graph1 = newListGraph();
    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int *parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedListGraph(graph1, filePath) == 1){
            perror("main: out of memory listGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }


        int source = 0, research_count = 0, visited_edges = 0;
        clock_t begin = clock();

        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("\nLBFSHybridSearch, n: %d\n iter: %d, start_source: %d", num_nodes, ++research_count, source);
            //printListGraph(graph1);
            //parentsArray = LBFSHybridSearch(graph1, source, hybrid);
            parentsArray = LBFSHybridSearchWithEdgesCount(graph1, source, hybrid, &visited_edges);
            if(parentsArray == NULL) {
                perror("main: out of memory abfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
                fprintf(numOfEdgesPointer, "%d\n", visited_edges);
                visited_edges = 0;
            }
        }
        clock_t end = clock();

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL){
            freeListGraph(graph1);
            break;
        }
        fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche

        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeListGraph(graph1);

        //printf("ok0\n");
    }
    fprintf(filePointer, "\n");
    fclose(swapPointer);
    fclose(numOfEdgesPointer);
    free(graph1);

    num_nodes = start_nodes;


        //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/swap/results%d-m%d.csv", start_nodes, (int) hybrid);
    swapPointer = fopen(filePath, "w");


    MatrixGraph *graph2 = newMtrxGraph();
    //printf("ok0\n");
    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int* parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedMtrxGraph(graph2, filePath) == 1){
            perror("main: out of memory mtrxGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }

        int source = 0, research_count = 0;
        clock_t begin = clock();

        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("\n\nMBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
            parentsArray = MBFSHybridSearch(graph2, source, hybrid);
            if(parentsArray == NULL) {
                perror("main: out of memory abfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
            }
        }
        clock_t end = clock();

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);
        if(parentsArray == NULL) {
            freeMtrxGraph(graph2);
            break;
        }
        fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche

        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeMtrxGraph(graph2);
    }
    fprintf(filePointer, "\n");
    fclose(swapPointer);
    fclose(filePointer);
    free(graph2);

    }
    //------------------------------ENDS REPEAT CICLE --------------------------------------------------------------------_/

    //sommo i risultati del repeat
    printf("Eseguo la media delle %d iterazioni del MES.\n\n", REPEAT);
    ResultSum(start_nodes, n, REPEAT, hybrid);
    
}



//creo per i grafi regolari. devo ora tenere traccia di tutt ivalori (per mettere barre di errore), quindi depreco averageonlysearch, e creo questa che sfrutta la singlegraph fatta per lo studio di barabasi e holme
//in piu cicla in automatico sia hybrid che non, se abilitato
void SingleGraphMultiFullSearch(char** argv, int start_nodes, int n, bool hybrid, bool generator) {
    printf("Ricerca tramite le 3 strutture dati su %d grafi con nodi iniziali=%d, crescente (%dn), Start!\n\n", n, start_nodes, MULTIPLIER);
    //printf("ok0\n");

    //FILE *filePointer = fopen("results.txt", "w");
    //char filepath[256]; //buffer
    //char sourceFilepath[256];
    int num_nodes = start_nodes;
    int multi = num_nodes*MULTIPLIER;

    for(int i = 0; i < n; i++) {

        SingleGraphFullSearch(argv, num_nodes, 1, hybrid, generator);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
        
    }

    num_nodes = start_nodes;

    for(int i = 0; i < n; i++) {

        SingleGraphFullSearch(argv, num_nodes, 1, true, false);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
        
    }

    start_nodes = 10000;       //REITERO PER FARE PURE 100000 = n IN CONTEMPORANEA
    num_nodes = start_nodes;
    multi = num_nodes*MULTIPLIER;

    for(int i = 0; i < n; i++) {
        
        SingleGraphFullSearch(argv, num_nodes, 1, hybrid, generator);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
        
    }

    num_nodes = start_nodes;

    for(int i = 0; i < n; i++) {

        SingleGraphFullSearch(argv, num_nodes, 1, true, false);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
        
    }
/*

    start_nodes = 100000;       //REITERO PER FARE PURE 100000 = n IN CONTEMPORANEA
    num_nodes = start_nodes;
    multi = num_nodes*MULTIPLIER;

    for(int i = 0; i < n; i++) {

        SingleGraphFullSearch(argv, num_nodes, 1, hybrid, generator);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
        
    }

    num_nodes = start_nodes;

    for(int i = 0; i < n; i++) {

        SingleGraphFullSearch(argv, num_nodes, 1, true, false);
 
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di MULTIPLIER*n in MULTIPLIER*n dopo di che 
    }
*/
}

void SingleGraphFullSearch(char** argv, int start_nodes, int n, bool hybrid, bool generator) {

    printf("Ricerca tramite le 3 strutture dati su %d grafi, Start!\n\n", n);
    //printf("ok0\n");
    if(generator)
        nGenerateGraphs(argv[1], n, start_nodes);



    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./singlegraphresults/results%d-a%d.csv", start_nodes, (int) hybrid);
    FILE *filePointer = fopen(filePath, "w");
    //FILE *filePointer = fopen("results.txt", "w");
    //char filepath[256]; //buffer
    //char sourceFilepath[256];
    int num_nodes = start_nodes;
    int multi = num_nodes*MULTIPLIER;

    //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./singlegraphresults/swap/results%d-a%d.csv", start_nodes, (int) hybrid);
    FILE *swapPointer = fopen(filePath, "w");
    //init EDGES COUNT
    snprintf(filePath, sizeof(filePath), "./singlegraphresults/results%d-a%d-edges.csv", start_nodes, (int) hybrid);
    FILE *numOfEdgesPointer = fopen(filePath, "w");

    Graph *graph0 = newArrGraph();
    for(int i = 0; i < n; i++) {

        int research_iter_num;
        int *parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;
        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        if(generator)
            generateSources(filePath, num_nodes, research_iter_num);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);

        if( ImportUndirectedGraph(graph0, filePath) == 1){
            perror("main: out of memory arrgraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }
 

        int source = 0, research_count = 0, visited_edges = 0;
        
        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("aBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
            clock_t begin = clock();
         //   parentsArray = aBFSHybridSearch(graph0, source, hybrid);
             parentsArray = aBFSHybridSearchWithEdgesCount(graph0, source, hybrid, &visited_edges);
            clock_t end = clock();
            if(parentsArray == NULL) {
                perror("main: out of memory abfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
                fprintf(numOfEdgesPointer, "%d\n", visited_edges);
                visited_edges = 0;
            }
            fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC));     //inserisco nel file le ricerche
        }

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL) {
            freeArrGraph(graph0);
            break;
        }
     

        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeArrGraph(graph0);
    }
    fclose(filePointer);
    fclose(numOfEdgesPointer);
    fclose(swapPointer);
    free(graph0);


    num_nodes = start_nodes;

    snprintf(filePath, sizeof(filePath), "./singlegraphresults/results%d-l%d.csv", start_nodes, (int) hybrid);
    filePointer = fopen(filePath, "w");
    //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./singlegraphresults/swap/results%d-l%d.csv", start_nodes, (int) hybrid);
    swapPointer = fopen(filePath, "w");
    //init EDGES COUNT
    snprintf(filePath, sizeof(filePath), "./singlegraphresults/results%d-l%d-edges.csv", start_nodes, (int) hybrid);
    numOfEdgesPointer = fopen(filePath, "w");

    ListGraph *graph1 = newListGraph();
    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int *parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedListGraph(graph1, filePath) == 1){
            perror("main: out of memory listGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }


        int source = 0, research_count = 0, visited_edges = 0;
     
        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("\nLBFSHybridSearch, n: %d\n iter: %d, start_source: %d", num_nodes, ++research_count, source);
            //printListGraph(graph1);
               clock_t begin = clock();
            //parentsArray = LBFSHybridSearch(graph1, source, hybrid);
            parentsArray = LBFSHybridSearchWithEdgesCount(graph1, source, hybrid, &visited_edges);
             clock_t end = clock();
            if(parentsArray == NULL) {
                perror("main: out of memory lbfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
                fprintf(numOfEdgesPointer, "%d\n", visited_edges);
                visited_edges = 0;
            }
             fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC));     //inserisco nel file le ricerche
        }
       

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL){
            freeListGraph(graph1);
            break;
        }
       
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeListGraph(graph1);

        //printf("ok0\n");
    }
    fclose(filePointer);
    fclose(numOfEdgesPointer);
    fclose(swapPointer);
    free(graph1);

    num_nodes = start_nodes;

    snprintf(filePath, sizeof(filePath), "./singlegraphresults/results%d-m%d.csv", start_nodes, (int) hybrid);
    filePointer = fopen(filePath, "w");
        //init SWAP USAGE
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/swap/results%d-m%d.csv", start_nodes, (int) hybrid);
    swapPointer = fopen(filePath, "w");

    MatrixGraph *graph2 = newMtrxGraph();
    //printf("ok0\n");
    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int* parentsArray;
        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedMtrxGraph(graph2, filePath) == 1){
            perror("main: out of memory mtrxGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }

        int source = 0;
        int research_count = 0;

        while( fscanf(sourcesFile, "%d", &source) == 1) {
            printf("\n\nMBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
                  clock_t begin = clock();
            parentsArray = MBFSHybridSearch(graph2, source, hybrid);
                    clock_t end = clock();
            if(parentsArray == NULL) {
                perror("main: out of memory mbfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
            }
            fprintf(filePointer, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC));   
        }

        //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);
        if(parentsArray == NULL) {
            freeMtrxGraph(graph2);
            break;
        }
   
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        freeMtrxGraph(graph2);
    }
    fclose(filePointer);
    fclose(swapPointer);
    free(graph2);
    
    
}

void MinMaxSearch(char** argv, int start_nodes, int n, bool hybrid, bool generator) {

    printf("Ricerca tramite le 3 strutture dati su %d grafi con nodi iniziali=%d, crescente (%dn), Start!\n\n", n, start_nodes, MULTIPLIER);
    //printf("ok0\n");
    if(generator)     //cambiato, ho aggiunto un ulteriore input. GENERO LE SORGENTI E I GRAFI SOLO PER HYBRID = 0, COSI DA UTILIZZARE GLI STESSI ANCHE CON HYBRID=1, da fare lo steso con il parallelo
        nGenerateGraphs(argv[1], n, start_nodes);

    double min = (double)100;
    double max = (double)0;
    double curr_clock;
    //char repeatSources[256];
    char filePath[256]; //buffer      
    //char sourceFilepath[256];
    int num_nodes = start_nodes;
    int multi = start_nodes*MULTIPLIER;
                                                        //results%dn^%d-a%d-min.csv", EDGEFACTOR, POWER, (int)hybrid);
                                                        //results1/5nlongn-a%d-min.csv", EDGEFACTOR, POWER, (int)hybrid);

    FILE *filePointer_min;
    FILE *filePointer_max;
    FILE *filePointer_avg;
    FILE * swapPointer;

    Graph *graph0 = newArrGraph();

    if(LOG_TOGGLE == 0) {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-a%d-min.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-a%d-max.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-a%d-avg.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    } else {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-a%d-min.csv", (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-a%d-max.csv", (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-a%d-avg.csv", (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    }

        //init SWAP USAGE
    if(LOG_TOGGLE == 0) 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/results%dn^%d-a%d.csv", (int)EDGEFACTOR, POWER, (int) hybrid);
    else 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/resultsnlogn-a%d.csv", (int) hybrid);
    swapPointer = fopen(filePath, "w");


    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int *parentsArray;

        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;
        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        if(generator)
            generateSources(filePath, num_nodes, research_iter_num);

        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedGraph(graph0, filePath) == 1){
            perror("main: out of memory arrgraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }

        int source = 0;
        int research_count = 0;
        clock_t begin = clock();
        while( fscanf(sourcesFile, "%d", &source) == 1) {
            clock_t begin_in = clock();
            printf("aBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
            parentsArray = aBFSHybridSearch(graph0, source, hybrid);
            if(parentsArray == NULL) {
                perror("main: out of memory abfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
            }
            clock_t end_in = clock();
            curr_clock =  ((double)(end_in - begin_in) / CLOCKS_PER_SEC);
            if(curr_clock > max)
                max = curr_clock;
            if(curr_clock < min)
                min = curr_clock;
        }
        clock_t end = clock();

                //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);
        
        if(parentsArray == NULL) {
            freeArrGraph(graph0);
            break;
        }

        fprintf(filePointer_min, "%f\n", min);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_max, "%f\n", max);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_avg, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche

        min = (double)100;
        max = (double)0;

        //num_nodes += 1200;
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        
        freeArrGraph(graph0);
    }
    fclose(filePointer_min);
    fclose(filePointer_max);
    fclose(filePointer_avg);
    fclose(swapPointer);
    free(graph0);

//REIMPOSTO LE VARIABILI UTILI
    num_nodes = start_nodes;
    min = (double)100;
    max = (double)0;


        //init SWAP USAGE
    if(LOG_TOGGLE == 0) 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/results%dn^%d-l%d.csv", (int)EDGEFACTOR, POWER, (int) hybrid);
    else 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/resultsnlogn-l%d.csv", (int) hybrid);
    swapPointer = fopen(filePath, "w");

    ListGraph *graph1 = newListGraph();

    if(LOG_TOGGLE == 0) {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-l%d-min.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-l%d-max.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-l%d-avg.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    } else {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-l%d-min.csv", (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-l%d-max.csv", (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-l%d-avg.csv", (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    }


    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int *parentsArray;

        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedListGraph(graph1, filePath) == 1){
            perror("main: out of memory listGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }

        int source = 0;
        int research_count = 0;
        clock_t begin = clock();

        while( fscanf(sourcesFile, "%d", &source) == 1) {
            clock_t begin_in = clock();
            printf("\nLBFSHybridSearch, n: %d\n iter: %d, start_source: %d", num_nodes, ++research_count, source);
            parentsArray = LBFSHybridSearch(graph1, source, hybrid);
            if(parentsArray == NULL) {
                perror("main: out of memory lbfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
            }

            clock_t end_in = clock();
            curr_clock =  ((double)(end_in - begin_in) / CLOCKS_PER_SEC);
            if(curr_clock > max)
                max = curr_clock;
            if(curr_clock < min)
                min = curr_clock;
        }
        clock_t end = clock();

                //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL){
            freeListGraph(graph1);
            break;
        }

        fprintf(filePointer_min, "%f\n", min);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_max, "%f\n", max);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_avg, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche

        min = (double)100;
        max = (double)0;

        //num_nodes += 1200;
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 
        
        freeListGraph(graph1);
        //printf("ok0\n");
    }
    fclose(filePointer_min);
    fclose(filePointer_max);
    fclose(filePointer_avg);
    fclose(swapPointer);
    free(graph1);

//REIMPOSTO LE VARIABILI UTILI
    num_nodes = start_nodes;
    min = (double)100;
    max = (double)0;

            //init SWAP USAGE
    if(LOG_TOGGLE == 0) 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/results%dn^%d-m%d.csv", (int)EDGEFACTOR, POWER, (int) hybrid);
    else 
        snprintf(filePath, sizeof(filePath), "./minmaxresults/swap/resultsnlogn-m%d.csv", (int) hybrid);
    swapPointer = fopen(filePath, "w");

    MatrixGraph *graph2 = newMtrxGraph();

    if(LOG_TOGGLE == 0) {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-m%d-min.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-m%d-max.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/results%dn^%d-m%d-avg.csv", (int)EDGEFACTOR, POWER, (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    } else {
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-m%d-min.csv", (int)hybrid);
        filePointer_min = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-m%d-max.csv", (int)hybrid);
        filePointer_max = fopen(filePath, "w");
        snprintf(filePath, sizeof(filePath), "./minmaxresults/resultsnlogn-m%d-avg.csv", (int)hybrid);
        filePointer_avg = fopen(filePath, "w");
    }

    for(int i = 0; i < n; i++) {
        int research_iter_num;
        int *parentsArray;

        if(num_nodes < SOURCES_PER_SEARCH*2)                              //ricerco su 50 sorgenti differenti se il numero di nodi è maggiore o uguale a 50
            research_iter_num = num_nodes / 2;
        else 
            research_iter_num = SOURCES_PER_SEARCH;

        snprintf(filePath, sizeof(filePath), "../../outputs/sources/sources_%d.txt", num_nodes);
        FILE *sourcesFile = fopen(filePath, "r");

        snprintf(filePath, sizeof(filePath), "%sgraph_%d.txt", argv[1], num_nodes);
        if( ImportUndirectedMtrxGraph(graph2, filePath) == 1){
            perror("main: out of memory mtrxGraph, research stopped.\n");
            fclose(sourcesFile);
            break;
        }

        int source = 0;
        int research_count = 0;
        clock_t begin = clock();

        while( fscanf(sourcesFile, "%d", &source) == 1) {
            clock_t begin_in = clock();
            printf("\n\nMBFSHybridSearch, n: %d\n iter: %d", num_nodes, ++research_count);
            parentsArray = MBFSHybridSearch(graph2, source, hybrid);
            if(parentsArray == NULL) {
                perror("main: out of memory mbfsSearch!\n\n");
                break;
            } else {
                free(parentsArray);
            }

            clock_t end_in = clock();
            curr_clock =  ((double)(end_in - begin_in) / CLOCKS_PER_SEC);
            if(curr_clock > max)
                max = curr_clock;
            if(curr_clock < min)
                min = curr_clock;
        }
        clock_t end = clock();

                //CHECK SWAP USAGE
        checkSwap(swapPointer);

        fclose(sourcesFile);

        if(parentsArray == NULL) {
            freeMtrxGraph(graph2);
            break;
        }

        fprintf(filePointer_min, "%f\n", min);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_max, "%f\n", max);     //inserisco nel file la media delle ricerche
        fprintf(filePointer_avg, "%f\n", ((double)(end - begin) / CLOCKS_PER_SEC) / research_iter_num);     //inserisco nel file la media delle ricerche
        min = (double)100;
        max = (double)0;

        //num_nodes += 1200;
        if(i == 0)
            num_nodes = num_nodes * MULTIPLIER;
        else
            num_nodes = num_nodes + multi;      //avanza di 5n in 5n dopo di che 

        freeMtrxGraph(graph2);
    }

    fclose(filePointer_min);
    fclose(filePointer_max);
    fclose(filePointer_avg);
    fclose(swapPointer);
    free(graph2);

}