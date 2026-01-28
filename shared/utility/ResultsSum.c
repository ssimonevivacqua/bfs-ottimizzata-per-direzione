#include "../header.h"

#define MAX_ARR_LENGTH (300)
#define NUM_VALUE_PER_TYPE (10)
//#define NUM_OF_FILES (8)
//#define SEARCH_TYPE (1000)
#define NUMBER_OF_SEARCHES (3)      //array graph, list graph, matrix graph
//#define NUM_OF_EACH_SEARCH (20)     //numero delle ricerche per ogni tipologia


// gcc -o sum ResultsSum.c

int ResultSum(int search_type, int num_of_each_search, int num_of_files, bool hybrid) {
    char filePath[256]; //buffer
    //int values[MAX_ARR_LENGTH];
    double * values = (double*) calloc(MAX_ARR_LENGTH, sizeof(double));

    printf("Programma che calcola la media di più ricerce effettuate sui grafi.\nStart!\n");
    for(int i = 0; i < num_of_files; i++) {
        int count = 0;
        snprintf(filePath, sizeof(filePath), "./avgresults/temp/results%d-%d-%d.csv", search_type, (int) hybrid, i);
        //printf("tryng to read from: %s", filePath);
        FILE *filePointer = fopen(filePath, "r");
        if(filePointer == NULL){
            perror("Il file non esiste!!\n\n");
            exit(1);
        }
        double value;
        for( int i2 = 0; i2 < NUMBER_OF_SEARCHES; i2++){
            while( fscanf(filePointer, "%lf", &value) == 1) {
                //printf("adds: %lf \n", value);
                values[count++] += value;
            }
            //count++;
        }
        fclose(filePointer);
    }
    //printf("end read\n");

    FILE ** filePointer = malloc(sizeof(FILE)*NUMBER_OF_SEARCHES);
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/results%d-a%d.csv", search_type, (int) hybrid);
    filePointer[0] = fopen(filePath, "w");
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/results%d-l%d.csv", search_type, (int) hybrid);
    filePointer[1] = fopen(filePath, "w");
    snprintf(filePath, sizeof(filePath), "./avgresults/sum/results%d-m%d.csv", search_type, (int) hybrid);
    filePointer[2] = fopen(filePath, "w");
    if(filePointer == NULL){
            perror("Il file non esiste!!\n\n");
            exit(1);
        }
    int count = 0;
    for(int i = 0; i < NUMBER_OF_SEARCHES; i++) {
        if(filePointer[i] == NULL){
            perror("Il file non esiste!!\n\n");
            exit(1);
        }
        int count_single_search = 0;
        while(count_single_search < num_of_each_search && values[count] != 0) {
            fprintf(filePointer[i], "%f\n", ((double)(values[count++]) / num_of_files));     //inserisco nel file la media delle ricerche
            count_single_search++;
        }
        fclose(filePointer[i]);
    }

    //fclose(filePointer);
    free(values);
    return 0;
}