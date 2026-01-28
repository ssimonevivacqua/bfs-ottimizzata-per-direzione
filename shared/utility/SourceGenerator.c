#include "../header.h"

// gcc -o ../../executable/SourcesGenerator SourceGenerator.c ../datastruct/Bitset.c
//       ../../executable/SourcesGenerator 4800000 50
//                                          MAXNODE TOTALSOURCES
/*
int main(int argc, char **argv) {
    int max_value, total_values;

    if ( argc < 3) {
        perror("\nInvalid input!\n\n");
        exit(1);
    }
    else {
        if( !sscanf(argv[1],"%d", &max_value) || !sscanf(argv[2], "%d", &total_values) ) {
            perror("\nInvalid input integer for the search/graph type!\n\n");
            exit(1);
        }
    }

    Bitset *already_used = newBitSet(max_value);
    FILE *f = fopen("../../outputs/sources.txt", "w");
    if(!f) {
        perror("fopen\n");
        exit(1);
    }
    srandom(time(NULL));

    for (int i = 0; i < total_values; i++) {
        int rand = random() % max_value;
        if( !is_set(already_used, rand)) {
            set_bit(already_used, rand);
            fprintf(f, "%d\n", rand);
        } else 
            i--;
    }

    fclose(f);
    printf("File con %d sorgenti scritto correttamente.\n", total_values);
    free(already_used);
    return 0;
}
*/
int generateSources(char* filepath, int max_value, int total_values) {

    Bitset *already_used = newBitSet(max_value);
    FILE *f = fopen(filepath, "w");
    if(!f) {
        perror("fopen\n");
        exit(1);
    }
    //srandom(time(NULL));
     init_genrand((unsigned long)time(NULL)); // inizializza con il tempo

    for (int i = 0; i < total_values-3; i++) {
        int rand = random_between(0, max_value-1);
        if( !is_set(already_used, rand)) {
            set_bit(already_used, rand);
            fprintf(f, "%d\n", rand);
        } else 
            i--;
    }

    fprintf(f, "%d\n", 0);      //aggungo 0 come sorgente fissa, mi serve per il diametro
    fprintf(f, "%d\n", 10);      //aggungo 10 come sorgente fissa, mi serve per il diametro
    fprintf(f, "%d\n", 20);      //aggungo 20 come sorgente fissa, mi serve per il diametro
    fclose(f);
    printf("File con %d sorgenti scritto correttamente.\n", total_values);
    deallocBitset(already_used);
    free(already_used);
    return 0;
}