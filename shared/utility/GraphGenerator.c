#include "../header.h"

#define GRAPHTYPE (2)
#define STARTNODES_BARABASI (3)         //classica, m0 = m = 4
#define EDGEPERNODES_BARABASI (3)
#define K_EDGESPERNODES_WATTS (10)      //ogni nodo parte con k/2 nodi a sx e a dx
#define PROBABILITY (0.25)
#define PROBABILITY_WATTS (0.07)        //P 10 ALLA meno 2
#define TOBE_COMPLETED (0)

typedef struct {
    int *u;
    int *v;
    int size;
    int capacity;
} EdgeList;

//stima il numero di archi 
uint32_t estimate_edges(int N, int m0, int m, double p) {
    double E_clique = m0 * (m0 - 1) / 2.0;
    double E_BA = m * (N - m0);
    double E_triad = p * E_BA;
    return (uint32_t)ceil(E_clique + E_BA + E_triad);
}

//stima il numero di archi e l'aumenta per avere meno collissioni nella capacity dell hash
uint32_t compute_hash_capacity(int N, int m0, int m, double p) {
    uint32_t E_total = estimate_edges(N,m0,m,p);
    return 2 * E_total; // moltiplicatore per ridurre collisioni
}

//stima il numero degli archi da usare nel buffer degli archi recenti edgelist
uint32_t compute_buffer_capacity(int N, int m0, int m) {
    return 2 * (uint32_t)m * (N - m0); // buffer circolare per triad formation
}



EdgeList* initEdgeBuffer(int n){
    EdgeList *elist = malloc(sizeof(EdgeList));
    int capacity = compute_buffer_capacity(n, STARTNODES_BARABASI, EDGEPERNODES_BARABASI);
    elist->capacity = capacity;
    elist->size = 0;
    elist->u = malloc(sizeof(int)*capacity);
    elist->v = malloc(sizeof(int)*capacity);
    if(elist->u == NULL || elist->v == NULL){
        perror("GraphGenerator: initEdgeList: out of memory!\n\n");
        return NULL;
    }
    return elist;
}

EdgeList* initEdgeList(int n){
    EdgeList *elist = malloc(sizeof(EdgeList));
    int capacity = estimate_edges(n, STARTNODES_BARABASI, EDGEPERNODES_BARABASI, 0.1);
    elist->capacity = capacity;
    elist->size = 0;
    elist->u = malloc(sizeof(int)*capacity);
    elist->v = malloc(sizeof(int)*capacity);
    if(elist->u == NULL || elist->v == NULL){
        perror("GraphGenerator: initEdgeList: out of memory!\n\n");
        return NULL;
    }
    return elist;
}

//mi serve solo per tenere traccia degli archi gia inseriti cosi da poterne scegliere uno casualmente per le triadi in barabasi holm
void add_edge(EdgeList *elist, int u, int v){
    if(elist->size == elist->capacity)
        elist->size = 0;    //buffer circolare, sovrascrivo archi più vecchi
    elist->u[elist->size] = u;
    elist->v[elist->size] = v;
    elist->size++;
}

//seleziono l'archo casuale per la barabasi holm 
void pick_random_edge(EdgeList *elist, int *u, int *v) {
    int idx = random() % elist->size;
    //printf("Size: %d, rand %d, (%d,%d)\n", elist->size, idx, elist->u[idx], elist->v[idx]);
    *u = elist->u[idx];
    *v = elist->v[idx];

}



// gcc -o GraphGen ../header.h GraphGenerator.c ../datastruct/Bitset.c ../datastruct/ArrBitset.c
//      ./GraphGen ../../../grafi/generati/undirected_graph0.txt 50 10 1


/*
int main(int argc, char **argv) {
    int num_nodes, power;
    double edge_factor;
    if ( argc < 4) {
        perror("\nInvalid input!\n\n");
        exit(1);
    }
    else {
        if( !sscanf(argv[2],"%d", &num_nodes) || !sscanf(argv[3], "%lf", &edge_factor) || !sscanf(argv[4], "%d", &power)) {
            perror("\nInvalid input integer for the search/graph type!\n\n");
            exit(1);
        }
    }

    printf("Generatore di grafi, Start!\n  n_nodes %d, edge_factor %lf, power %d\n\n", num_nodes, edge_factor, power);
    generateRandUndirectedGraph(argv[1], num_nodes, edge_factor, power);

    //bitset tester
    /*Bitset *used_edges = newArrBitset(num_nodes);
    set_bit_arrDouble(used_edges, 0, 4, num_nodes);
    printf("\n è settato? %d\n", is_set_arr(used_edges, 4, 0, num_nodes));
    printBitset(used_edges);* /
    return 0;

}
*/

int powe(int x, int y) {
    int toReturn = x;
    for(int i = 1; i < y; i++) {
        toReturn = toReturn * x;
    }
    return toReturn;
}

// Funzione per selezionare un nodo con probabilità proporzionale al grado
int choose_node(int *degrees, int total_degree, int n) {
   // printf("random between %d - %d", 0, total_degree-1);
    //int r = random_between(0, total_degree-1);
    int r = random() % total_degree;
    int c = 0;
    for (int i = 0; i < n; i++) {
        c += degrees[i];
        if (r < c) return i;
    }
    return n - 1; // fallback
}

/*
void generateHolmKimUndirectedGraph(char *location, int nodes) {

    int edge_count = 0;
    double p = PROBABILITY;
    int N = nodes;   // numero totale di nodi
    int m0 = (int)STARTNODES_BARABASI;  // numero iniziale di nodi
    int m  = (int)EDGEPERNODES_BARABASI;  // numero di connessioni per ogni nuovo nodo
    if (m > m0 || m0 >= N) {
        printf("Errore: richiedi m <= m0 e m0 < N.\n");
        return ;
    }

    EdgeHash *new_hash = malloc(sizeof(EdgeHash));
    hash_init(new_hash, compute_hash_capacity(N, m0, m, p) );

    // Max archi = N*m (upper bound)
    //Edge *edges = malloc(sizeof(Edge) * N * m);
    //int edge_count = 0;       //numEdges sul ArrGraph
    int *degrees = calloc(N, sizeof(int));        //out_edges sul ArrGraph
        //Graph *used_edges = newArrGraph();
        //initArrGraph(used_edges, N, true);
    EdgeList *new_elist = initEdgeList(N);
    if(new_elist == NULL) {
        perror("GraphGen: genBarabasi: out of memory!!\n\n");
        exit(1);
    }

    srandom(time(NULL));
    //init_genrand((unsigned long)time(NULL)); // inizializza con il tempo

    printf("Generando grafo BA...\n\n");
    // Inizializza grafo di partenza come clique di m0 nodi
    for (int i = 0; i < m0; i++) {
        for (int j = i + 1; j < m0; j++) {
            add_edge(new_elist, i, j);
            uint32_t key = encode_edge(i,j);
            hash_insert(new_hash, key);
            edge_count += 2;
            degrees[i]++;
            degrees[j]++;
        }
    }

    // Aggiunta dei nuovi nodi
    for (int new_node = m0; new_node < N; new_node++) {
        int added = 0;
        while (added < m) {
            int chosen = choose_node(degrees, edge_count, new_node);
            // Evita self-loop e duplicati
            if( new_node == chosen) continue;
            uint32_t key = encode_edge(new_node, chosen);
            if ( hash_lookup(new_hash, key)) continue;

            // Aggiungi arco
            add_edge(new_elist, new_node, chosen);
            hash_insert(new_hash, key);
            edge_count += 2;
            degrees[new_node]++;
            degrees[chosen]++;
            added++;
        }
    }
    // Stampa il grafo in formato lista di archi
  //  printf("Grafo BA con %d nodi e %d archi:\n", N, edge_count);
  //  printArrGraph(used_edges);

    free(new_hash->keys);
    free(new_hash);


    Graph *barabasi_graph = newArrGraph();
    initArrGraph(barabasi_graph, N, false);
    initOffsetArr(barabasi_graph);
    barabasi_graph->numEdges = edge_count;
    initOffsetArr(barabasi_graph);

    int size = new_elist->size;
    for(int i = 0; i < size; i++) {
        doubleArrEdge(new_elist->u[i], new_elist->v[i]);
    }

    free(degrees);
    free(new_elist->u);
    free(new_elist->v);
    free(new_elist);


    FILE *filePointer = fopen(location, "w");
    if(!filePointer) {
        perror("fopen\n");
        exit(1);
    }


  // --- Holme–Kim: triad formation con probabilità p ---
            if ((((double) random() / RAND_MAX ) < p) && new_elist->size > 0) {
            // scegli un vicino casuale di "chosen"
                int u_rand = -1, v_rand =-1, w =-1;
                const int MAX_TRIES = 16;    //volte che prova a cercare un arco di chosen
                for(int i = 0; i < MAX_TRIES; i++) {
                    pick_random_edge(new_elist, &u_rand, &v_rand);
                    if(u_rand == chosen) { w = v_rand; break; }
                    if(v_rand == chosen) { w = u_rand; break; }
                }
                
                if(w >= 0) {
                    // evita self-loop e duplicati
                    if( new_node == w) continue;
                    uint32_t key2 = encode_edge(new_node, w);
                    if ( hash_lookup(new_hash, key2)) continue;

                // Aggiungo arco
                    add_edge(new_elist, new_node, w);
                    hash_insert(new_hash, key2);
                    edge_count += 2;
                    degrees[new_node]++;
                    if( w < 0 || w>=N) {
                        printf("GraphGenerator: generateBarabasi: w=%d fuori range [0, %d)!\n", w, N);
                        exit(1);
                    }
                    degrees[w]++;
                    fprintf(filePointer, "%d %d\n", new_node, w);
                    added++;
                }
            }
    fclose(filePointer);
        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    char meta_data[256];
    snprintf(meta_data, sizeof(meta_data), "%smeta", location);
    filePointer = fopen(meta_data, "w");
     //4039 88234 2
    fprintf(filePointer, "%d %d %d\n", N, edge_count/2, GRAPHTYPE);     //2 non direzionato, 1 direzionato
    fclose(filePointer);
    //deallocBitset(used_edges);
    printf("File con %d archi scritto correttamente.\n", edge_count);
    return ;
}
    */


void generateWattsUndirectedGraphs(char *location, int nodes) {
    int edge_count = 0;
    double p = PROBABILITY_WATTS;
    int N = nodes;   // numero totale di nodi
    int k = K_EDGESPERNODES_WATTS;
    
    if (k % 2 != 0 || k < 2 || k >= N) {
        printf("Errore: richiedi k pari, k >= 2 e k < N.\n");
        return ;
    }

    FILE *filePointer = NULL;
    EdgeHash *new_hash = malloc(sizeof(EdgeHash));
    hash_init(new_hash, compute_hash_capacity(N, 0, k, p+0.1) );

    filePointer = fopen(location, "w");
    if(!filePointer) {
        perror("fopen\n");
        //free(all) da implementare
        exit(1);
    }

        //prima aggiungo solo legami clockwise , senso orario successivo (quindi legami siccessivi al nodo (k/2))
    printf("Generando grafo NW...\n\n");
    for(int i = 0; i < N; i++) {
        for(int d = 1; d <= k/2; d++) {
            int j = (i + d) % N;
            uint64_t key = encode_edge(i,j);
            hash_insert(new_hash, key);
            edge_count+=2;
            fprintf(filePointer, "%d %d\n", i, j);
        }
    }

    //const long M0 = (long)N * (k/2); //calcolo archi lattice attesi
    const long S = llround(p * (double)N * (double)(k/2));        //calcolo  scorciatoie attese
    const long MAX_ATTEMPS = S ? (50*S + 1000) : 0;
    int added = 0, attemps = 0;
    srandom(time(NULL));
    

        //aggiungo scorciatoie randomiche
    while(added < S) {
        if(attemps++ > MAX_ATTEMPS) {
            perror("Raggiungto limite aggiunta scorciatoie!!!\n");
            break;
        }
        int u = random() % N;
        int v = random() % N;
        if (u == v) continue;
        
        uint64_t key = encode_edge(u,v);
        if(!hash_lookup(new_hash, key)) {
            hash_insert(new_hash, key);
            fprintf(filePointer, "%d %d\n", u, v);
            edge_count += 2;
            added++;
        }
    }
    /*   //aggiungo scorciatoie randomiche
    for(int i = 0; i < N; i++) {
        for(int j = i+1; j < k/2; j++) {
            uint64_t key = encode_edge(i,j);
            if(!hash_lookup(new_hash, key)) {
                hash_insert(new_hash, key);
                edge_count+=2;
                fprintf(filePointer, "%d %d\n", i, j);
            }
        }
    }*/
 
    free(new_hash->keys);
    free(new_hash);
    fclose(filePointer);
        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    char meta_data[256];
    snprintf(meta_data, sizeof(meta_data), "%smeta", location);
    filePointer = fopen(meta_data, "w");
     //4039 88234 2
    fprintf(filePointer, "%d %d %d\n", N, edge_count/2, GRAPHTYPE);     //2 non direzionato, 1 direzionato
    fclose(filePointer);
    //deallocBitset(used_edges);
    printf("File con %d archi scritto correttamente.\n", edge_count);
    return ;

}

//GEnera grafi di barabasi con probabilità settata a 0, altrimenti fa triangolazioni generando grafi di Holme--Kim
        //Inoltre può generare un grafo con più run del programma se TOBE_COMPLETED settato a 1
void generateBarabasiUndirectedGraph(char *location, int nodes) {

    int edge_count = 0;
    double p = PROBABILITY;
    int N = nodes;   // numero totale di nodi
    int m0 = (int)STARTNODES_BARABASI;  // numero iniziale di nodi
    int m  = (int)EDGEPERNODES_BARABASI;  // numero di connessioni per ogni nuovo nodo
    if (m > m0 || m0 >= N) {
        printf("Errore: richiedi m <= m0 e m0 < N.\n");
        return ;
    }
    EdgeList *new_elist = initEdgeBuffer(N);
    if(new_elist == NULL) {
        perror("GraphGen: genBarabasi: out of memory!!\n\n");
        exit(1);
    }


    EdgeHash *new_hash = malloc(sizeof(EdgeHash));
    hash_init(new_hash, compute_hash_capacity(N, m0, m, p) );
    int *degrees = calloc(N, sizeof(int));        //out_edges sul ArrGraph
    FILE *filePointer = NULL;
    int last_used_node = 0;

    if(TOBE_COMPLETED) {
        filePointer = fopen(location, "r");
        if(!filePointer) {
            perror("fopen\n");
            //free(all) da implementare
            exit(1);
        }

        int node1,node2;
        while( fscanf(filePointer, "%d %d", &node1, &node2) == 2) {
            add_edge(new_elist, node1, node2);
            uint64_t key = encode_edge(node1,node2);
            hash_insert(new_hash, key);
            edge_count += 2;
            degrees[node1]++;
            degrees[node2]++;
            last_used_node = node1;
        }
        fclose(filePointer);
        filePointer = fopen(location, "a");        //apre in append il file
        printf("Grafo Rigenerato correttamente.\n");
    }
    else {
        filePointer = fopen(location, "w");
        if(!filePointer) {
            perror("fopen\n");
            //free(all) da implementare
            exit(1);
        }
    }

    srandom(time(NULL));
    //init_genrand((unsigned long)time(NULL)); // inizializza con il tempo

    printf("Generando grafo BA/HK...\n\n");

    if(TOBE_COMPLETED != 1) {
        // Inizializza grafo di partenza come clique di m0 nodi
        for (int i = 0; i < m0; i++) {
            for (int j = i + 1; j < m0; j++) {
                add_edge(new_elist, i, j);
                uint64_t key = encode_edge(i,j);
                hash_insert(new_hash, key);
                edge_count += 2;
                degrees[i]++;
                degrees[j]++;
                fprintf(filePointer, "%d %d\n", i, j);
            }
        }
    }
    if(last_used_node != 0)     //quindi ho sicuramente rigenerato il grafo
        m0 = last_used_node+1;

    // Aggiunta dei nuovi nodi
    for (int new_node = m0; new_node < N; new_node++) {
        int added = 0;
        while (added < m) {
            int chosen = choose_node(degrees, edge_count, new_node);
            // Evita self-loop e duplicati
            if( new_node == chosen) continue;
            uint64_t key = encode_edge(new_node, chosen);
            if ( hash_lookup(new_hash, key)) continue;

            // Aggiungi arco
            add_edge(new_elist, new_node, chosen);
            hash_insert(new_hash, key);
            edge_count += 2;
            degrees[new_node]++;
            degrees[chosen]++;
            fprintf(filePointer, "%d %d\n", new_node, chosen);
            added++;

            // --- Holme–Kim: triad formation con probabilità p ---
            if ((((double) random() / RAND_MAX ) < p) && new_elist->size > 0) {
            // scegli un vicino casuale di "chosen"
                int u_rand = -1, v_rand =-1, w =-1;
                const int MAX_TRIES = 16;    //volte che prova a cercare un arco di chosen
                for(int i = 0; i < MAX_TRIES; i++) {
                    pick_random_edge(new_elist, &u_rand, &v_rand);
                    if(u_rand == chosen) { w = v_rand; break; }
                    if(v_rand == chosen) { w = u_rand; break; }
                }
                
                if(w >= 0) {
                    // evita self-loop e duplicati
                    if( new_node == w) continue;
                    uint64_t key2 = encode_edge(new_node, w);
                    if ( hash_lookup(new_hash, key2)) continue;

                // Aggiungo arco
                    add_edge(new_elist, new_node, w);
                    hash_insert(new_hash, key2);
                    edge_count += 2;
                    degrees[new_node]++;
                    if( w < 0 || w>=N) {
                        printf("GraphGenerator: generateBarabasi: w=%d fuori range [0, %d)!\n", w, N);
                        exit(1);
                    }
                    degrees[w]++;
                    fprintf(filePointer, "%d %d\n", new_node, w);
                    added++;
                }
            }
        }
    }
    // Stampa il grafo in formato lista di archi
  //  printf("Grafo BA con %d nodi e %d archi:\n", N, edge_count);
  //  printArrGraph(used_edges);
    free(degrees);
    free(new_hash->keys);
    free(new_hash);
    free(new_elist->u);
    free(new_elist->v);
    free(new_elist);
    fclose(filePointer);
        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    char meta_data[256];
    snprintf(meta_data, sizeof(meta_data), "%smeta", location);
    filePointer = fopen(meta_data, "w");
     //4039 88234 2
    fprintf(filePointer, "%d %d %d\n", N, edge_count/2, GRAPHTYPE);     //2 non direzionato, 1 direzionato
    fclose(filePointer);
    //deallocBitset(used_edges);
    printf("File con %d archi scritto correttamente.\n", edge_count);
    return ;

}



void generateRandUndirectedGraph(char *location, int n_nodes, double edge_factor, int log_toggle, int power){                   //crea un grafo non orientato generico
    FILE *filePointer = fopen(location, "w");
    if(!filePointer) {
        perror("fopen\n");
        exit(1);
    }
    //int edge_factor_int = (int )edge_factor;
    //printf("Edgefactore int: %d\n", edge_factor_int);
    int n_edges;
    if(log_toggle == 0){
        n_edges = powe(n_nodes, power) * edge_factor;
    } else {
        n_edges = (powe(n_nodes, power) * log2(n_nodes)) * edge_factor;
    }
//    printf("ok0.1\n");
    //Bitset *used_edges = newArrBitset(n_nodes); //NO, DEVO TROVARE COME POSSO VERIFICARE GLI ARCHI GIA INSERITI SENZA PESARE TROPPO IN MEMORIA!!!!
    Graph *used_edges = newArrGraph();
    initArrGraph(used_edges, n_nodes, true);
    //printBitset(used_edges);
    if (!used_edges) {
        perror("malloc used_edges Random Graph Gen\n");
        return;
    }

    //srandom(time(NULL));
    init_genrand((unsigned long)time(NULL)); // inizializza con il tempo


    long edge_count = 0;
    int edges_per_node = n_edges / n_nodes;
    printf("Edges per node %d, n_nodes %d, edge_factor %lf, logn multipied %d, power %d, n_Edges %d\n", edges_per_node, n_nodes, edge_factor, log_toggle, power, n_edges);
    /*
    while (edge_count < n_edges) {
        int u = random() % n_nodes;
        int v = random() % n_nodes;

        if (u == v) continue;  // evita self-loop
        if (is_set_arr(used_edges, edge_count, u, v)) continue; // evita duplicati

        edges[edge_count].u = u;
        edges[edge_count].v = v;
        edge_count++;
    }
        */
       

    if(edges_per_node < 1) {
        for(int index = 0; index < n_nodes; index++) {
            //if(edge_count < n_edges) break;
                int node_used_count = random_between(0, 1) + edges_per_node;
                while(node_used_count > 0){
                    int rand_node = random_between(0, n_nodes-1);
                    if( index == rand_node) continue;
                    if ( existsEdgeXY(used_edges, index, rand_node)) continue;
                    //if ( is_set_arr(used_edges, index, rand_node, n_nodes)) continue;
                    fprintf(filePointer, "%d %d\n", index, rand_node);
                    doubleArrEdge(used_edges, index, rand_node);
                    //set_bit_arrDouble(used_edges, index, rand_node, n_nodes);
                    edge_count++;
                    //break;
                    node_used_count--;
                }
        }
    }
    else {
        if(edges_per_node > 6)
            edges_per_node -= 2;
        else 
            edges_per_node -= 0;
        for(int index = 0; index < n_nodes; index++) {
            int edges_per_node_added;
            if(edges_per_node > 7)
                 edges_per_node_added = 0 - random_between(0, 4);
             else 
                 edges_per_node_added = 0 - random_between(0, 2);
            while(edges_per_node_added < edges_per_node) {
                int rand_node = random_between(0, n_nodes-1);
                if( index == rand_node) continue;
                //printf("Trying to add %d - %d...\n", index, rand_node);
                //printf("is set? %d\n" ,is_set_arr(used_edges, index, rand_node, n_nodes));
                if ( existsEdgeXY(used_edges, index, rand_node)) continue;
                //if ( is_set_arr(used_edges, index, rand_node, n_nodes)) continue;
                //printf("\nok\n");
                fprintf(filePointer, "%d %d\n", index, rand_node);
                doubleArrEdge(used_edges, index, rand_node);
                //set_bit_arrDouble(used_edges, index, rand_node, n_nodes);
                edges_per_node_added++;
                edge_count++;
            }
        }
    }
    //printBitset(used_edges);
    freeArrGraph(used_edges);
    free(used_edges);
    fclose(filePointer);

        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    char meta_data[256];
    snprintf(meta_data, sizeof(meta_data), "%smeta", location);
    filePointer = fopen(meta_data, "w");
     //4039 88234 2
    fprintf(filePointer, "%d %ld %d\n", n_nodes, edge_count, GRAPHTYPE);     //2 non direzionato, 1 direzionato
    fclose(filePointer);

    //deallocBitset(used_edges);
    printf("File con %ld archi scritto correttamente.\n", edge_count);
    return ;
}

int randEdgesNumber(int graph_vertices_n) {
    return rand() % ((graph_vertices_n/2) +1);       //tutti i nodi possono avere al massimo la metà dei nodi totali come archi
}

int randNode(int graph_vertices_n, int n) {
    srand(time(NULL));
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

void generateRandUndirectedGraphWithDDiameter(char *location, int n_nodes, double edge_factor, int power, int diameter){                   //crea un grafo non orientato generico
    int returned_value = 1;
    while(returned_value != 0) {
        returned_value = generateRandUndirectedGraphWithDDiameterCore(location,n_nodes,edge_factor, power, diameter);
    }
    return ;
}
int generateRandUndirectedGraphWithDDiameterCore(char *location, int n_nodes, double edge_factor, int power, int diameter){                   //crea un grafo non orientato generico
    FILE *filePointer = fopen(location, "w");
    if(!filePointer) {
        perror("fopen\n");
        exit(1);
    }
    //int edge_factor_int = (int )edge_factor;
    //printf("Edgefactore int: %d\n", edge_factor_int);
    int n_edges = powe(n_nodes, power) * edge_factor;
//    printf("ok0.1\n");
    //Bitset *used_edges = newArrBitset(n_nodes); //NO, DEVO TROVARE COME POSSO VERIFICARE GLI ARCHI GIA INSERITI SENZA PESARE TROPPO IN MEMORIA!!!!
    Graph *used_edges = newArrGraph();
    initArrGraph(used_edges, n_nodes, true);
    //printBitset(used_edges);
    if (!used_edges) {
        perror("malloc used_edges Random Graph Gen\n");
        return 1;
    }

    //srandom(time(NULL));
    init_genrand((unsigned long)time(NULL)); // inizializza con il tempo

    long edge_count = 0;
    int edges_per_node = n_edges / n_nodes;
    printf("Edges per node %d, n_nodes %d, edge_factor %lf, power %d, n_Edges %d, Diameter %d\n", edges_per_node, n_nodes, edge_factor, power, n_edges, diameter);
    /*
    while (edge_count < n_edges) {
        int u = random() % n_nodes;
        int v = random() % n_nodes;

        if (u == v) continue;  // evita self-loop
        if (is_set_arr(used_edges, edge_count, u, v)) continue; // evita duplicati

        edges[edge_count].u = u;
        edges[edge_count].v = v;
        edge_count++;
    }
        */
     
    for (int i = 0; i < diameter; i++) {
        fprintf(filePointer, "%d %d\n", i, i+1);
        doubleArrEdge(used_edges, i, i+1);
    }    


    if(edges_per_node < 1) {
        for(int index = 0; index < n_nodes; index++) {
            //if(edge_count < n_edges) break;
                int node_used_count = random_between(0, 1) + edges_per_node;
                while(node_used_count > 0){
                    int rand_node = random_between(0, n_nodes-1);
                    if( (index < diameter || rand_node < diameter)) continue; 
                    if( index == rand_node) continue;
                    if ( existsEdgeXY(used_edges, index, rand_node)) continue;
                    //if ( is_set_arr(used_edges, index, rand_node, n_nodes)) continue;
                    fprintf(filePointer, "%d %d\n", index, rand_node);
                    doubleArrEdge(used_edges, index, rand_node);
                    //set_bit_arrDouble(used_edges, index, rand_node, n_nodes);
                    edge_count++;
                    //break;
                    node_used_count--;
                }
        }
        fclose(filePointer);
    }
    else {
        edges_per_node -= 2;
        for(int index = 0; index < n_nodes; index++) {
            int edges_per_node_added = 0 - random_between(0, 4);
            while(edges_per_node_added < edges_per_node) {
                int rand_node = random_between(0, n_nodes-1);
          //      if( (index < diameter || rand_node < diameter)) continue;     //no, poco elegante
                if( index == rand_node) continue;
                //printf("Trying to add %d - %d...\n", index, rand_node);
                //printf("is set? %d\n" ,is_set_arr(used_edges, index, rand_node, n_nodes));
                if ( existsEdgeXY(used_edges, index, rand_node)) continue;
                //if ( is_set_arr(used_edges, index, rand_node, n_nodes)) continue;
                //printf("\nok\n");
                fprintf(filePointer, "%d %d\n", index, rand_node);
                doubleArrEdge(used_edges, index, rand_node);
                //set_bit_arrDouble(used_edges, index, rand_node, n_nodes);
                edges_per_node_added++;
                edge_count++;
            }
        }
        fclose(filePointer);
        VerticesDepth *newDepth = initNewDepth(used_edges->numVertices, 0);
        aBFSHybridSearch_verify(used_edges, 0, 1, newDepth);
        if(newDepth->depth != diameter) {
            free(newDepth->array_depth);
            free(newDepth);
            freeArrGraph(used_edges);
            free(used_edges);
        return 1;              //PERROR "GRAFO NON VALIDO, DIAMETRO NON RISPETTATO!"
        }
        free(newDepth->array_depth);
        free(newDepth);
    }
    //printBitset(used_edges);
//    fclose(filePointer);


        //creo un file contenente le informazioni principali del grafo (numero di nodi, archi e tipologia -> direzionato o non)
    char meta_data[256];
    snprintf(meta_data, sizeof(meta_data), "%smeta", location);
    filePointer = fopen(meta_data, "w");
     //4039 88234 2
    fprintf(filePointer, "%d %ld %d\n", n_nodes, edge_count, GRAPHTYPE);     //2 non direzionato, 1 direzionato
    fclose(filePointer);

    //deallocBitset(used_edges);
    freeArrGraph(used_edges);
    free(used_edges);
    printf("File con %ld archi scritto correttamente.\n", edge_count);

    return 0;
}