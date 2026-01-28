#include "../header.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------- Funzioni che eseguono la Visita in Ampiezza su un determinato algoritmo, usando molteplici sorgenti, memorizzando il tempo min,max,avg----//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//



int * BFSResearch_arr(Graph *g, char* sources, bool hybrid) {
    int * parents;
    Research* res = BFSResearch(sources);
    Iterations *iters = initIterations(res->source_num);
    for (int i = 0; i < res->source_num; i++) {
        clock_t begin = clock();

        parents = aBFSHybridSearch_study(g, res->sources[i], hybrid, iters);

        clock_t end = clock();
        res->exec_times[i] = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("exe time: %.2f.\n\n", (double)res->exec_times[i]);
        printCurrIteration(iters); 
        iters->curr_iter++;
        checkMinMax(res, i);
    }
    getAverage(res);

    printIterations(iters); 
    printResearchResults(res);
    //freeMultipleSourceSearch(res, iters);
    return parents;
}

int * BFSResearch_list(ListGraph *g, char* sources, bool hybrid) {
    int * parents;
    Research* res = BFSResearch(sources);
    Iterations *iters = initIterations(res->source_num);
    for (int i = 0; i < res->source_num; i++) {
        clock_t begin = clock();

        parents = LBFSHybridSearch_study(g, res->sources[i], hybrid, iters);

        clock_t end = clock();
        res->exec_times[i] = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("exe time: %.2f.\n\n", (double)res->exec_times[i]); 
        printCurrIteration(iters); 
        iters->curr_iter++;
        checkMinMax(res, i);
    }
    getAverage(res);

    printIterations(iters); 
    printResearchResults(res);
    //freeMultipleSourceSearch(res, iters);
    return parents;
}

int * BFSResearch_mtrx(MatrixGraph *g, char* sources, bool hybrid) {
    int * parents;
    Research* res = BFSResearch(sources);
    for (int i = 0; i < res->source_num; i++) {
        clock_t begin = clock();

        parents = MBFSHybridSearch(g, res->sources[i], hybrid);

        clock_t end = clock();
        res->exec_times[i] = (double)(end - begin) / CLOCKS_PER_SEC;
        //printf("exe time: %.2f.\n\n", (double)exec_time); 
        checkMinMax(res, i);
    }
    getAverage(res);
    printResearchResults(res);
    //freeResearch(res);
    return parents;
}
    
    

Research *BFSResearch(char* sources) {
    int n_sources = getSourceNum(sources);
    Research *res = initResearch(n_sources);
    res->source_num = n_sources;

    FILE *filePointer = fopen(sources, "r");
    int source;
    int index = 0;
    while( fscanf(filePointer, "%d", &source) == 1) {
        //printf("Leggo %d\n", source);
            res->sources[index] = source;
            index++;
        }
    if(!feof(filePointer)) {
        perror("2- Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    return res;
}

int getSourceNum(char* sources) {
    FILE *filePointer = fopen(sources, "r");
    int total = 0;
    int dummy;
    while( fscanf(filePointer, "%d", &dummy) == 1 ) {
            //printf("total: %d", total);
            total++;
        }
    if(!feof(filePointer)) {
        perror("1- Attenzione! Lettura interrotta prima della fine del file!\n");
        exit(1);
    }
    fclose(filePointer);

    return total;
}

void printResearchResults(Research *r){
    printf("Results:\n\n Min: %.3f, Max %.3f, Avg: %.3f\n\n", r->min, r->max, r->average);
    for(int i = 0; i < r->source_num; i++)
        printf("%.3f\n", r->exec_times[i]);
}

void printCurrIteration(Iterations *iters) {
    int curr_iter = iters->curr_iter;
    int top_down_step_count = iters->iter[curr_iter].top_down_step_count;
    int bottom_up_step_count = iters->iter[curr_iter].bottom_up_step_count;
    printf("Iterazioni totali: %d\nScelte: Top-down %d, Bottom-up %d\n", top_down_step_count+bottom_up_step_count, top_down_step_count, bottom_up_step_count);
}

void printIterations(Iterations *iters) {
    printf("\n\nTutte le iterazioni: \n\n");
    printf("---Top-Down\n");
    for(int i = 0; i < iters->curr_iter; i++)
        printf("%d\n", iters->iter[i].top_down_step_count);
    printf("\n--Bottom-Up\n");
    for(int i = 0; i < iters->curr_iter; i++)
        printf("%d\n", iters->iter[i].bottom_up_step_count);
    printf("\n----Totale----\n");
    for(int i = 0; i < iters->curr_iter; i++)
        printf("%d\n", iters->iter[i].top_down_step_count + iters->iter[i].bottom_up_step_count);
}

void getAverage(Research *r) {
    double total = 0;
    for(int i = 0; i < r->source_num; i++)
        total += r->exec_times[i];
    r->average = (double) total/r->source_num;

    return ;
}

void checkMinMax(Research* res, int i) {
    if(res->exec_times[i] <= res->min) {
            res->min = res->exec_times[i];
    }
        else {
            if(res->exec_times[i] > res->max)
                res->max = res->exec_times[i];
        }
    return ;
}

Research *initResearch(int n) {
    Research* r = malloc(sizeof(Research));
    r->min = n;
    r->max = -1;
    r->exec_times = malloc(sizeof(int) * n);
    r->sources = malloc(sizeof(int) * n);

    return r;
}

Iterations *initIterations(int n) {
    Iterations* i = malloc(sizeof(Iterations));
    i->iter = calloc(n, sizeof(Iteration));
    i->curr_iter = 0;
    return i;
}

void freeMultipleSourceSearch(Research * r, Iterations * i) {
    free(r->exec_times);
    free(r->sources);
    free(i->iter);
    free(i);
    free(r);
}