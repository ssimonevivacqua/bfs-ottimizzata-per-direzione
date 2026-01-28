#include "shared/header.h"

/*
    gcc -o executable/aBFS main.c shared/utility/MultipleSourceSearch.c DO-BFS_core.c ArrayDO-BFS.c ListDO-BFS.c MatrixDO-BFS.c shared/header.h shared/Frontier.c shared/Parents.c shared/utility/GraphImporter.c shared/datastruct/graphs/ArrayGraph.c shared/datastruct/graphs/ListGraph.c shared/datastruct/graphs/MatrixGraph.c shared/datastruct/ArrQueue.c shared/datastruct/LinkedList.c shared/datastruct/Bitset.c

gcc -o executable/aBFSp -fopenmp main.c shared/utility/MultipleSourceSearch.c DO-BFS_core_p.c ArrayDO-BFS.c ListDO-BFS.c MatrixDO-BFS.c shared/header.h shared/Frontier.c shared/Parents.c shared/utility/GraphImporter.c shared/datastruct/graphs/ArrayGraph.c shared/datastruct/graphs/ListGraph.c shared/datastruct/graphs/MatrixGraph.c shared/datastruct/ArrQueue.c shared/datastruct/LinkedList.c shared/datastruct/Chunk.c shared/datastruct/Bitset.c

input sample:
    ./executable/aBFS ../grafi/soc-LiveJournal1.txt 0 2 1

*/


int main( int argc, char **argv) {
    int source = 0;
    int graph_type, search_type, hybrid_int;
    bool hybrid;
    char *string;
   

    if ( argc < 5) {
        perror("\nInvalid input!\n\n");
        exit(1);
    }
    else {
        if( !sscanf(argv[2],"%d", &search_type) || !sscanf(argv[3], "%d", &graph_type) || !sscanf(argv[4], "%d", &hybrid_int) ) {
            perror("\nInvalid input integer for the search/graph type!\n\n");
            exit(1);
        }
        if( hybrid_int == 0 || hybrid_int == 1) 
            hybrid = hybrid_int;
        else {
            perror("\nInvalid hybrid choice!\n\n");
            exit(1);
        }
    }
    //visita in hybrid bfs con array di adiacenza
    int * parents;
    int size;
        printf("Main start!\n");
    //search type:
        //case 0 -> ricarca con array based list
        //case 1 -> ricerca con lista di adiacena
        //case 2 -> ricerca con matrice di adiacenza

        //graph_type:
            //case 0 -> genera un grafo randomico non orientato
            //case 1 -> genera un grafo randomico orientato
            //case 2 -> importa un grafo non orientato
            //case 3 -> importa un grafo orientato

    switch(search_type){

        case 0:
            Graph *graph0 = newArrGraph();
            switch (graph_type) {
                case 0:
                    //generateRandUndirectedGraph(graph0, graph_vertices);
                    break;
                case 1:
                    //generateRandDirectedGraph..
                    break;
                case 2:
                    ImportUndirectedGraph(graph0, argv[1]);
                    //printArrGraph(graph0);
                    break;
                case 3:
                    ImportDirectedGraph(graph0, argv[1]);
                    break;
                default:
                    perror("\nInvalid choice!\n\n");
                    exit(1);
            }
            parents = aBFSHybridSearch(graph0, source, hybrid);
            size = graph0->numVertices;
            //OPTIONAL----depth verifica ---//
                        /*VerticesDepth *newDepth = initNewDepth(graph0->numVertices, source);
                        parents = aBFSHybridSearch_verify(graph0, source, hybrid, newDepth);
                        printArr(newDepth->array_depth, size);*/
            //OPTIONAL2----ricerca con multiple sorgenti ---//
                        //parents = BFSResearch_arr(graph0, "./outputs/sources.txt", hybrid);
            break;

        case 1:
            ListGraph *graph1= newListGraph();
            switch (graph_type) {
                case 0:
                    //generateRandListGraph(graph1, graph_vertices);
                    break;
                case 1:
                    //generateRandDirectedGraph..
                    break;
                case 2:
                    ImportUndirectedListGraph(graph1, argv[1]);
                    break;
                case 3:
                    ImportDirectedListGraph(graph1, argv[1]);
                    break;
                default:
                    perror("\nInvalid choice!\n\n");
                    exit(1);
            }
            parents = LBFSHybridSearch(graph1, source, hybrid);
            //parents = BFSResearch_list(graph1, "./outputs/sources.txt", hybrid);
            size = graph1->numVertices;
            
            break;

        case 2:
            MatrixGraph *graph2 = newMtrxGraph();
            switch (graph_type) {
                case 0:
                    //generateRandListGraph(graph1, graph_vertices);
                    break;
                case 1:
                    //generateRandDirectedGraph..
                    break;
                case 2:
                    ImportUndirectedMtrxGraph(graph2, argv[1]);
                    //printMtrxGraph(graph2);
                    break;
                case 3:
                    ImportDirectedMtrxGraph(graph2, argv[1]);
                    break;
                default:
                    perror("\nInvalid choice!\n\n");
                    exit(1);
            }
            parents = MBFSHybridSearch(graph2, source, hybrid);      //DA FARE!
            size = graph2->numVertices;
            break;

        default:
            perror("\nInvalid search type!\n\n");
            exit(1);
    }

    //printArr(parents, size);
    //free(parents);
    return 0;
}

int readInput(char **argv) {

}

void deallocMain(Graph *graph, int *parents) {
    freeArrGraph(graph);
    free(parents);
    return ;
}

void printArr(int *a, int size) {
    int i = 0;
    printf("\n\nparents: ");
    for(i = 0; i < size; i++) {
        printf("%d ", a[i]);
    }
    printf("\n\n");
}

