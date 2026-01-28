#include "../header.h"

// Struttura per archi
typedef struct {
    int u, v;
} Edge;

// Funzione per selezionare un nodo con probabilità proporzionale al grado
int choose_node(int *degrees, int total_degree, int n) {
    int r = rand() % total_degree;
    int c = 0;
    for (int i = 0; i < n; i++) {
        c += degrees[i];
        if (r < c) return i;
    }
    return n - 1; // fallback
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: %s <N_nodi> <m0> <m>\n", argv[0]);
        return 1;
    }

    // converte stringa caratteri in intero
    int N = atoi(argv[1]);   // numero totale di nodi
    int m0 = atoi(argv[2]);  // numero iniziale di nodi
    int m  = atoi(argv[3]);  // numero di connessioni per ogni nuovo nodo

    if (m > m0 || m0 >= N) {
        printf("Errore: richiedi m <= m0 e m0 < N.\n");
        return 1;
    }

    // Max archi = N*m (upper bound)
    Edge *edges = malloc(sizeof(Edge) * N * m);
    int edge_count = 0;

    int *degrees = calloc(N, sizeof(int));

    srand(time(NULL));

    // Inizializza grafo di partenza come clique di m0 nodi
    for (int i = 0; i < m0; i++) {
        for (int j = i + 1; j < m0; j++) {
            edges[edge_count].u = i;
            edges[edge_count].v = j;
            edge_count++;
            degrees[i]++;
            degrees[j]++;
        }
    }

    int total_degree = 2 * edge_count;  // Siccome non direzionato

    // Aggiunta dei nuovi nodi
    for (int new_node = m0; new_node < N; new_node++) {
        int added = 0;
        while (added < m) {
            int chosen = choose_node(degrees, total_degree, new_node);
            
            // Evita self-loop e duplicati
            int exists = 0;
            for (int e = 0; e < edge_count; e++) {
                if ((edges[e].u == new_node && edges[e].v == chosen) ||
                    (edges[e].v == new_node && edges[e].u == chosen)) {
                    exists = 1;
                    break;
                }
            }
            if (chosen == new_node || exists) continue;

            // Aggiungi arco
            edges[edge_count].u = new_node;
            edges[edge_count].v = chosen;
            edge_count++;
            degrees[new_node]++;
            degrees[chosen]++;
            total_degree += 2;
            added++;
        }
    }

    // Stampa il grafo in formato lista di archi
    printf("Grafo BA con %d nodi e %d archi:\n", N, edge_count);
    for (int i = 0; i < edge_count; i++) {
        printf("%d %d\n", edges[i].u, edges[i].v);
    }

    free(edges);
    free(degrees);
    return 0;
}