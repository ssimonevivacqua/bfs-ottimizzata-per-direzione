#include "../header.h"

void getGraphInfoLoc(char* location, char* buffer, size_t bufferSize) {
        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    snprintf(buffer, bufferSize, "%smeta", location);
}

int ImportArrayGraph(Graph *graph, char* string, bool undirected_graph) {
   // printf("ok1\n");
    //bool undirected_graph = true;

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointer = fileOpen(meta_data);
    //legge un intera riga e la mette in data come stringa
    char buffer[32];    //massimo numero di caratteri per riga
    
    //recupero numero dei nodi e numero di archi, solo se sono 2 nella prima riga
    int numVertices, numToRead;
    long numEdges;

    if(fgets(buffer, sizeof(buffer), filePointer) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    //printf("\n nodi %d, archi %ld \n\n", numVertices, numEdges);
    fclose(filePointer);
  //  printf("ok1,2\n");
    initArrGraph(graph, numVertices, !undirected_graph);
    //printf("ok1,4\n");
    initOutdegrees(graph, string);
    //printf("ok1,5\n");
    //printf("\nOUTEDGES: -------------------------------\n\n");
    //printfArrOffset(graph->out_degree, graph->numVertices);
    initOffsetArr(graph);
 //   printf("ok1,7\n");    
    //printf("\nOFFSET: -------------------------------\n\n");
    //printfArrOffset(graph->offset, graph->numVertices);
    initOutDegree(graph);

    //printf("ok2\n");
    if(undirected_graph)
        graph->numEdges = graph->numEdges*2;
    if (initAllOutEdges(graph) == 1)
        return 1;
    return 0;
}

int ImportUndirectedGraph(Graph *graph, char* string) {
    
    if( ImportArrayGraph(graph, string, true) == 1) {
        perror("GraphImporter, ImportUndirectedGraph: Runs out of memory!\n\n");
        return 1;
    }

    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);
      
    //legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);
        
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            doubleArrEdge(graph, node1, node2);
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            doubleArrEdge(graph, node1, node2);
        }
    }
    if(!feof(filePointer)) {
        //printf("qui?");
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    if(graph->numEdges != numEdges*2) {
        printf("\nErrore nel parsing! Numero di archi non corrisponde! %ld vs %ld\n\n", graph->numEdges, numEdges*2);
        exit(1);
    }

    return 0;
}


int ImportDirectedGraph(Graph *graph, char* string) {
    if( ImportArrayGraph(graph, string, false) == 1) {
        perror("GraphImporter, ImportDirectedGraph: Runs out of memory!\n\n");
        return 1;
    }


    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);

    //legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);

        
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            arrEdge(graph, node1, node2);
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            arrEdge(graph, node1, node2);
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    if(graph->numEdges != numEdges) {
        perror("\nErrore nel parsing! Numero di archi non corrisponde!\n\n");
        exit(1);
    }
    return 0;
}

int ImportUndirectedListGraph(ListGraph *graph, char *string){
    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);

    //legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);
    initListGraph(graph, numVertices, 0);

        
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            if(doubleListEdge(graph, node1, node2) == 1){
                perror("GraphImporter, ImportUndirectedListGraph: Out of memory!\n\n");
                fclose(filePointer);
                return 1;
            }
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            if(doubleListEdge(graph, node1, node2) == 1){
                perror("GraphImporter, ImportUndirectedListGraph: Out of memory!\n\n");
                fclose(filePointer);
                return 1;
            }
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\numVerticesn");
        exit(1);
    }
    fclose(filePointer);
    //printf("graph edg: %ld, edg: %ld", graph->numEdges, numEdges);
    if(graph->numEdges != numEdges*2) {
        perror("\nErrore nel parsing! Numero di archi non corrisponde!\n\n");
        exit(1);
    }
 
    return 0;
}
int ImportDirectedListGraph(ListGraph *graph, char *string){
    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);

    //legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);
    initListGraph(graph, numVertices, 1);

        
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            if(listEdge(graph, node1, node2) == 1){
                perror("GraphImporter, ImportDirectedListGraph: Out of memory!\n\n");
                fclose(filePointer);
                return 1;
            }
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            if(listEdge(graph, node1, node2) == 1){
                perror("GraphImporter, ImportDirectedListGraph: Out of memory!\n\n");
                fclose(filePointer);
                return 1;
            }
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    if(graph->numEdges != numEdges*2) {
        perror("\nErrore nel parsing! Numero di archi non corrisponde!\n\n");
        exit(1);
    }
 
    return 0;
}

int ImportUndirectedMtrxGraph(MatrixGraph *graph, char *string){
    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);

    if( initMtrxGraph(graph, numVertices, 0) == 1) {
        perror("GraphImporter, ImportUndirectedMtrxGraph: Out of memory!\n\n");
        return 1;
    }

    //legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);
        
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            doubleMtrxEdge(graph, node1, node2);
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            doubleMtrxEdge(graph, node1, node2);
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);
    //printf("graph edg: %ld, edg: %ld", graph->numEdges, numEdges);
    if(graph->numEdges != numEdges*2) {
        perror("\nErrore nel parsing! Numero di archi non corrisponde!\n\n");
        exit(1);
    }
 
    return 0;
}
int ImportDirectedMtrxGraph(MatrixGraph *graph, char *string){
    int numVertices, numToRead;
    long numEdges;
    char buffer[32]; 

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointerMeta = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointerMeta) != NULL)
        if(sscanf(buffer, "%d %ld %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    printf("\n nodi %d, archi %ld , grafo non orientato\n\n", numVertices, numEdges);
    fclose(filePointerMeta);


    if( initMtrxGraph(graph, numVertices, 0) == 1) {
        perror("GraphImporter, ImportUndirectedMtrxGraph: Out of memory!\n\n");
        return 1;
    }
        
//legge due numeri, che siano 2 o 3 per riga
    int node1, node2;
    FILE * filePointer = fileOpen(string);

    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            mtrxEdge(graph, node1, node2);
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            printfAddOper(node1,node2);
            mtrxEdge(graph, node1, node2);
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    if(graph->numEdges != numEdges*2) {
        perror("\nErrore nel parsing! Numero di archi non corrisponde!\n\n");
        exit(1);
    }
 
    return 0;
}


FILE * fileOpen(char* string) {
    //aprire  un file 
    FILE *filePointer;
    filePointer = fopen(string,"r");
    //"../../../grafi/facebook_combined.txt"
    if (filePointer == NULL) {
        perror("Failed to open the file.\n");
        exit(1);
    }
    else 
        return filePointer;    
}

void initOutdegrees(Graph * graph, char* string) {
    //FILE * filePointer = fileOpen(string);
    char buffer[32]; 
    int numVertices, numEdges, numToRead;

    char meta_data[256];
    getGraphInfoLoc(string, meta_data, sizeof(meta_data)); 
    FILE * filePointer = fileOpen(meta_data);

    if(fgets(buffer, sizeof(buffer), filePointer) != NULL)
        if(sscanf(buffer, "%d %d %d", &numVertices, &numEdges, &numToRead) != 3 
        || (numToRead < 2 || numToRead > 3) ) 
            exit(1); 
    //printf("\n nodi %d, archi %d \n\n", numVertices, numEdges);
    fclose(filePointer);

    filePointer = fileOpen(string);
    int node1,node2;
    if(numToRead == 2) {
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            //printfAddOper(node1,node2);
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            doubleOutDegreeAdd(graph, node1, node2);
        }
    }
    if(numToRead == 3) {
        while( fscanf(filePointer, "%d %d %*d", &node1, &node2) == 3) {
            //printfAddOper(node1,node2);
            if(node1 >= graph->numVertices || node2 >= graph->numVertices || node1 < 0 || node2 < 0){
                perror("arco non valido, blocco.\n\n");
                continue;
            }
            doubleOutDegreeAdd(graph, node1, node2);
        }
    }
    if(!feof(filePointer)) {
        perror("Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);
}


void printfArrOffset(int* arrayoffset, int nodes) {
    for (int i = 0; i < nodes; i++)
        printf("\n %d : %d", i, arrayoffset[i]);
}

void printfAddOper(int node1, int node2) {
    //printf("\n aggiungo %d, %d", node1, node2);
    return ;
}