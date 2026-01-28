# visita_in_ampiezza_ottimizzata_per_direzione
Algoritmo di Visita in Ampiezza basato sul modello Barabási-Albert, con confronto tra le differenti implementazioni delle strutture dati per la rappresentazione dei grafi, e tra la versione sequenziale e multi-thread, con particolare attenzione all'ottimizzazione della memoria.


Punti salienti:

- Generatore di grafi randomici regolari e non (GraphGenerator.c).
- Generaore di sorgenti randomico (si appoggia a mt19937ar) (SourceGenerator.c).
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

- Il progetto si basa soprattutto sull'ottimizazione in memoria ed il confronto delle varie implementazioni, per questo motivo la versione multi-thread è stata implementata tramite l'uso di una libreria OpenMP in modo da semplificarne il processo.

