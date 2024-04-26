#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ptv.h"

int main(int argc, char* argv[]) {
    // ptvtest data.json
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* jsonString = (char*)malloc(file_size + 1);
    if (jsonString == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return 1;
    }

    fread(jsonString, 1, file_size, file);
    jsonString[file_size] = '\0';

    fclose(file);

    Data data = parseData(jsonString);

    double PT = 999998;
    double TM = 3.1556679;

    PtvTablePoint* resulty = computePoint(TM, PT, data);

    printf("PT: %d\n", resulty->pt);
    printf("TM: %f\n", resulty->tm);
    printf("ROG: %e\n", resulty->rog);
    printf("ROHL: %e\n", resulty->rohl);
    printf("ROWT: %e\n", resulty->rowt);
    printf("DROGDP: %e\n", resulty->drogdp);
    printf("DROHLDP: %e\n", resulty->drohldp);
    printf("DROWTDP: %e\n", resulty->drowtdp);
    printf("DROGDT: %e\n", resulty->drogdt);
    printf("DROHLDT: %e\n", resulty->drohldt);
    printf("DROWTDT: %e\n", resulty->drowtdt);
    printf("RS: %e\n", resulty->rs);
    printf("RSW: %e\n", resulty->rsw);
    printf("VISG: %e\n", resulty->visg);
    printf("VISHL: %e\n", resulty->vishl);
    printf("VISWT: %e\n", resulty->viswt);
    printf("CPG: %e\n", resulty->cpg);
    printf("CPHL: %e\n", resulty->cphl);
    printf("CPWT: %e\n", resulty->cpwt);
    printf("HG: %e\n", resulty->hg);
    printf("HHL: %e\n", resulty->hhl);
    printf("HWT: %e\n", resulty->tcg);
    printf("TCHL: %e\n", resulty->tchl);
    printf("TCWT: %e\n", resulty->tcwt);
    printf("SIGGHL: %e\n", resulty->sigghl);
    printf("SIGGWT: %e\n", resulty->siggwt);
    printf("SIGHLWT: %e\n", resulty->sighlwt);
    printf("SEG: %e\n", resulty->seg);
    printf("SEHL: %e\n", resulty->sehl);
    printf("SEWT: %e\n", resulty->sewt);
    //printf("Message: %s\n", resulty.message);
    freePoint(resulty);
    free(jsonString);
    freeData(data);
    return 0;
} 