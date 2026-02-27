# Algoritmo di Visita in Ampiezza ottimizzata per direzione con confronto tra implementazioni di strutture dati
La Breadth-First Search (BFS) è uno degli algoritmi fondamentali per l’esplorazione dei grafi ed è ampiamente utilizzata in ambiti come reti, analisi di grafi sociali, sistemi distribuiti e motori di ricerca.

Quando si lavora con grafi di grandi dimensioni, l’efficienza dell’algoritmo dipende fortemente dalla struttura dati utilizzata e dalla gestione della memoria. In questo progetto viene analizzata una variante ottimizzata per direzione, in cui la strategia di visita si adatta dinamicamente in base alle caratteristiche del grafo e alla dimensione della frontiera.


Implementazione in C della Breadth-First Search (BFS) ottimizzata per direzione, progettata per analizzare e confrontare diverse rappresentazioni dei grafi e valutarne l’impatto su prestazioni e utilizzo della memoria.

Il progetto include:

Confronto tra diverse strutture dati per la rappresentazione dei grafi (liste di adiacenza, matrici di adiacenza, matrici compresse).

Implementazione sequenziale e versione parallela tramite OpenMP.

Generazione di grafi casuali (regolari e modello Barabási-Albert).

Implementazione di strutture ausiliarie ottimizzate (code, bitset, ecc.).

Misurazione e analisi delle performance in termini di tempo di esecuzione e consumo di memoria.

Il repository è adatto a scopi accademici, studio di algoritmi su grafi e analisi comparativa delle prestazioni.


Punti salienti:

- Generatore di grafi randomici regolari e non (GraphGenerator.c).
- Generatore di sorgenti randomico (si appoggia a mt19937ar) (SourceGenerator.c).
- Import di grafi in C (GraphImporter.c).
- Gestione dei flussi di output.

- Strutture dati per la rappresentazione dei grafi implementate:
  - frontiere, 
  - liste di adiacenza,
  - matrici di adiacenza,
  - matrici compresse.
 
- Strutture dati ausiliari e utility implementate: 
  - queue ad array,
  - chunk,
  - bitset.


Obiettivi del progetto

Valutare l’impatto delle diverse strutture dati sulle prestazioni della BFS.

Confrontare implementazioni sequenziali e parallele.

Analizzare i trade-off tra consumo di memoria e velocità di esecuzione.

Studiare il comportamento dell’algoritmo su diverse tipologie di grafi.

