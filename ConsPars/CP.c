#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <locale.h>

typedef struct {
    double criticalPressure;
    double criticalTemperature;
    double criticalVolume;
    double acentricFactor;
    double boilerTemperature;
    double molecularWeight;
    double parohor;
    double density;
    double cp1;
    double cp2;
    double cp3;
    double cp4;
    double meltingTemperature;
    double meltingEnthalpy;
    double cPen;
    double cPenT;
    double aiSmall;
    double biSmall;
    double aiLarge;
    double biLarge;
    double aiiSmall;
    double biiSmall;
    double aiiLarge;
    double biiLarge;
    double ahSmall;
    double bhSmall;
    double ahHuge;
    double bhHuge;
} OverridableParameters;

typedef struct {
    char id[50];
    char compound[50];
    double mol;
    double molWeight;
    double density;
    OverridableParameters overridableParameters;
} Component;

// Структура для Ptv
typedef struct {
    char id[50];
    double pt;
    double tm;
    double rog;
    double rohl;
    double rowt;
    double drogdp;
    double drohldp;
    double drowtdp;
    double drogdt;
    double drohldt;
    double drowtdt;
    double rs;
    double rsw;
    double visg;
    double vishl;
    double viswt;
    double cpg;
    double cphl;
    double cpwt;
    double hg;
    double hhl;
    double hwt;
    double tcg;
    double tchl;
    double tcwt;
    double sigghl;
    double siggwt;
    double sighlwt;
    double seg;
    double sehl;
    double sewt;
    int order;
} PtvTablePoint;

// Структура для BubbleCurve
typedef struct {
    double temperature;
    double pressure;
} BubbleCurvePoint;

// Структура для CriticalPoint
typedef struct {
    double* temperature;
    double* pressure;
} CriticalPoint;

// Структура для PhaseEnvelope
typedef struct {
    char id[50];
    CriticalPoint* criticalPoint;
    BubbleCurvePoint* bubbleCurvePoints;
    int bubbleCurveCount;
} PhaseEnvelope;

// Структура для основных данных
typedef struct {
    char id[50];
    char name[50];
    int type;
    char reservoir[50];
    int phases;
    int componentsCount;
    Component* components;
    PtvTablePoint* ptvTablePoints;
    char equationOfState;
    double stdPressure;
    double stdTemperature;
    double stdGasDensity;
    double stdOilDensity;
    double stdLiqDensity;
    double stdWatDensity;
    double gor;
    double glr;
    double wc;
    double totWaterFraction;
    PhaseEnvelope phaseEnvelope;
} Data;

void safeStrCpy(char* destination, size_t size, const char* source) {
    if (source != NULL) {
        if (strcpy_s(destination, size, source) != 0) {
            perror("Error copying string");
            exit(1);
        }
    }
    else {
        destination[0] = '\0';
    }
}

// Функция для вывода OverridableParameters
void printOverridableParameters(OverridableParameters* overridableParameters) {
    printf("CriticalPressure: %f\n", overridableParameters->criticalPressure);
    printf("CriticalTemperature: %f\n", overridableParameters->criticalTemperature);
    printf("CriticalVolume: %f\n", overridableParameters->criticalVolume);
    printf("AcentricFactor: %f\n", overridableParameters->acentricFactor);
    printf("BoilerTemperature: %f\n", overridableParameters->boilerTemperature);
    printf("MolecularWeight: %f\n", overridableParameters->molecularWeight);
    printf("Parohor: %f\n", overridableParameters->parohor);
    printf("Density: %f\n", overridableParameters->density);
    printf("Cp1: %f\n", overridableParameters->cp1);
    printf("Cp2: %f\n", overridableParameters->cp2);
    printf("Cp3: %f\n", overridableParameters->cp3);
    printf("Cp4: %f\n", overridableParameters->cp4);
    printf("MeltingTemperature: %f\n", overridableParameters->meltingTemperature);
    printf("MeltingEnthalpy: %f\n", overridableParameters->meltingEnthalpy);
    printf("CPen: %f\n", overridableParameters->cPen);
    printf("CPenT: %f\n", overridableParameters->cPenT);
    printf("AISmall: %f\n", overridableParameters->aiSmall);
    printf("BISmall: %f\n", overridableParameters->biSmall);
    printf("AILarge: %f\n", overridableParameters->aiLarge);
    printf("BILarge: %f\n", overridableParameters->biLarge);
    printf("AIISmall: %f\n", overridableParameters->aiiSmall);
    printf("BIISmall: %f\n", overridableParameters->biiSmall);
    printf("AIILarge: %f\n", overridableParameters->aiiLarge);
    printf("BIILarge: %f\n", overridableParameters->biiLarge);
    printf("AHSmall: %f\n", overridableParameters->ahSmall);
    printf("BHSmall: %f\n", overridableParameters->bhSmall);
    printf("AHHuge: %f\n", overridableParameters->ahHuge);
    printf("BHHuge: %f\n", overridableParameters->bhHuge);
}

// Функция для вывода компонента
void printComponent(Component* component) {
    printf("Component Id: %s\n", component->id);
    printf("Compound: %s\n", component->compound);
    printf("Mol: %f\n", component->mol);
    printf("=== OverridableParameters ===\n");
    printOverridableParameters(&component->overridableParameters);
}

// Функция для вывода Ptv в консоль
void printPtvTablePoint(PtvTablePoint* ptvTablePoint) {
    printf("PtvTablePoint Id: %s\n", ptvTablePoint->id);
    printf("PT: %d\n", ptvTablePoint->pt);
    printf("TM: %f\n", ptvTablePoint->tm);
    printf("ROG: %f\n", ptvTablePoint->rog);
    printf("ROHL: %f\n", ptvTablePoint->rohl);
    printf("ROWT: %f\n", ptvTablePoint->rowt);
    printf("DROGDP: %e\n", ptvTablePoint->drogdp);
    printf("DROHLDP: %e\n", ptvTablePoint->drohldp);
    printf("DROWTDP: %e\n", ptvTablePoint->drowtdp);
    printf("DROGDT: %e\n", ptvTablePoint->drogdt);
    printf("DROHLDT: %e\n", ptvTablePoint->drohldt);
    printf("DROWTDT: %e\n", ptvTablePoint->drowtdt);
    printf("RS: %d\n", ptvTablePoint->rs);
    printf("RSW: %f\n", ptvTablePoint->rsw);
    printf("VISG: %e\n", ptvTablePoint->visg);
    printf("VISHL: %e\n", ptvTablePoint->vishl);
    printf("VISWT: %e\n", ptvTablePoint->viswt);
    printf("CPG: %e\n", ptvTablePoint->cpg);
    printf("CPHL: %e\n", ptvTablePoint->cphl);
    printf("CPWT: %e\n", ptvTablePoint->cpwt);
    printf("HG: %d\n", ptvTablePoint->hg);
    printf("HHL: %d\n", ptvTablePoint->hhl);
    printf("HWT: %d\n", ptvTablePoint->hwt);
    printf("TCG: %f\n", ptvTablePoint->tcg);
    printf("TCHL: %f\n", ptvTablePoint->tchl);
    printf("TCWT: %f\n", ptvTablePoint->tcwt);
    printf("SIGGHL: %f\n", ptvTablePoint->sigghl);
    printf("SIGGWT: %f\n", ptvTablePoint->siggwt);
    printf("SIGHLWT: %f\n", ptvTablePoint->sighlwt);
    printf("SEG: %f\n", ptvTablePoint->seg);
    printf("SEHL: %f\n", ptvTablePoint->sehl);
    printf("SEWT: %f\n", ptvTablePoint->sewt);
    printf("Order: %d\n", ptvTablePoint->order);
}

// Функция для вывода BubbleCurve в консоль
void printBubbleCurvePoint(BubbleCurvePoint* bubbleCurvePoint) {
    printf("Temperature: %f\n", bubbleCurvePoint->temperature);
    printf("Pressure: %f\n", bubbleCurvePoint->pressure);
}

// Функция для вывода CriticalPoint в консоль
void printCriticalPoint(CriticalPoint* criticalPoint) {
    if (criticalPoint == NULL) {
        printf("CriticalPoint: NULL\n");
    }
    else {
        printf("CriticalPoint:\n");
        printf("Temperature: %f\n", criticalPoint->temperature != NULL ? *criticalPoint->temperature : 0.0);
        printf("Pressure: %f\n", criticalPoint->pressure != NULL ? *criticalPoint->pressure : 0);
    }
}

// Функция для вывода PhaseEnvelope в консоль
void printPhaseEnvelope(PhaseEnvelope* phaseEnvelope) {
    printf("PhaseEnvelope Id: %s\n", phaseEnvelope->id);
    printCriticalPoint(phaseEnvelope->criticalPoint);

    printf("BubbleCurve:\n");
    for (int i = 0; i < phaseEnvelope->bubbleCurveCount; ++i) {
        printf("=== BubbleCurvePoint %d ===\n", i + 1);
        printBubbleCurvePoint(&phaseEnvelope->bubbleCurvePoints[i]);
    }
}

// Функция для вывода данных в консоль
void printData(Data data) {
    printf("Id: %s\n", data.id);
    printf("Name: %s\n", data.name);
    printf("Type: %d\n", data.type);
    printf("Reservoir: %s\n", data.reservoir);
    printf("Phases: %d\n", data.phases);

    printf("Components:\n");
    for (int i = 0; i < data.componentsCount; ++i) {
        printf("=== Component %d ===\n", i + 1);
        printComponent(&data.components[i]);
    }

    printf("PtvTablePoints:\n");
    for (int i = 0; i < 2 && data.ptvTablePoints[i].pt != 0; ++i) { //Количество значений Ptv для вывода в консоль (i < n)
        printf("=== PtvTablePoint %d ===\n", i + 1);
        printPtvTablePoint(&data.ptvTablePoints[i]);
    }

    printf("EquationOfState: %d\n", data.equationOfState);
    printf("StdPressure: %f\n", data.stdPressure);
    printf("StdTemperature: %f\n", data.stdTemperature);
    printf("StdGasDensity: %f\n", data.stdGasDensity);
    printf("StdOilDensity: %f\n", data.stdOilDensity);
    printf("StdLiqDensity: %f\n", data.stdLiqDensity);
    printf("StdWatDensity: %f\n", data.stdWatDensity);
    printf("Gor: %f\n", data.gor);
    printf("Glr: %f\n", data.glr);
    printf("Wc: %f\n", data.wc);
    printf("TotWaterFraction: %f\n", data.totWaterFraction);

    printf("PhaseEnvelope:\n");
    printPhaseEnvelope(&data.phaseEnvelope);
}


// Функция для парсинга компонента
Component parseComponent(cJSON* componentJSON) {
    Component component;
    safeStrCpy(component.id, sizeof(component.id), cJSON_GetObjectItem(componentJSON, "Id")->valuestring);
    safeStrCpy(component.compound, sizeof(component.compound), cJSON_GetObjectItem(cJSON_GetObjectItem(componentJSON, "Compound"), "Value")->valuestring);
    component.mol = cJSON_GetObjectItem(cJSON_GetObjectItem(componentJSON, "Mol"), "Value")->valuedouble;

    cJSON* overridableParametersJSON = cJSON_GetObjectItem(componentJSON, "OverridableParameters");
    if (overridableParametersJSON != NULL) {
        component.overridableParameters.criticalPressure = cJSON_GetObjectItem(overridableParametersJSON, "CriticalPressure")->valuedouble;
        component.overridableParameters.criticalTemperature = cJSON_GetObjectItem(overridableParametersJSON, "CriticalTemperature")->valuedouble;
        component.overridableParameters.criticalVolume = cJSON_GetObjectItem(overridableParametersJSON, "CriticalVolume")->valuedouble;
        component.overridableParameters.acentricFactor = cJSON_GetObjectItem(overridableParametersJSON, "AcentricFactor")->valuedouble;
        component.overridableParameters.boilerTemperature = cJSON_GetObjectItem(overridableParametersJSON, "BoilerTemperature")->valuedouble;
        component.overridableParameters.molecularWeight = cJSON_GetObjectItem(overridableParametersJSON, "MolecularWeight")->valuedouble;
        component.overridableParameters.parohor = cJSON_GetObjectItem(overridableParametersJSON, "Parohor")->valuedouble;
        component.overridableParameters.density = cJSON_GetObjectItem(overridableParametersJSON, "Density")->valuedouble;
        component.overridableParameters.cp1 = cJSON_GetObjectItem(overridableParametersJSON, "Cp1")->valuedouble;
        component.overridableParameters.cp2 = cJSON_GetObjectItem(overridableParametersJSON, "Cp2")->valuedouble;
        component.overridableParameters.cp3 = cJSON_GetObjectItem(overridableParametersJSON, "Cp3")->valuedouble;
        component.overridableParameters.cp4 = cJSON_GetObjectItem(overridableParametersJSON, "Cp4")->valuedouble;
        component.overridableParameters.meltingTemperature = cJSON_GetObjectItem(overridableParametersJSON, "MeltingTemperature")->valuedouble;
        component.overridableParameters.meltingEnthalpy = cJSON_GetObjectItem(overridableParametersJSON, "MeltingEnthalpy")->valuedouble;
        component.overridableParameters.cPen = cJSON_GetObjectItem(overridableParametersJSON, "CPen")->valuedouble;
        component.overridableParameters.cPenT = cJSON_GetObjectItem(overridableParametersJSON, "CPenT")->valuedouble;
        component.overridableParameters.aiSmall = cJSON_GetObjectItem(overridableParametersJSON, "AISmall")->valuedouble;
        component.overridableParameters.biSmall = cJSON_GetObjectItem(overridableParametersJSON, "BISmall")->valuedouble;
        component.overridableParameters.aiLarge = cJSON_GetObjectItem(overridableParametersJSON, "AILarge")->valuedouble;
        component.overridableParameters.biLarge = cJSON_GetObjectItem(overridableParametersJSON, "BILarge")->valuedouble;
        component.overridableParameters.aiiSmall = cJSON_GetObjectItem(overridableParametersJSON, "AIISmall")->valuedouble;
        component.overridableParameters.biiSmall = cJSON_GetObjectItem(overridableParametersJSON, "BIISmall")->valuedouble;
        component.overridableParameters.aiiLarge = cJSON_GetObjectItem(overridableParametersJSON, "AIILarge")->valuedouble;
        component.overridableParameters.biiLarge = cJSON_GetObjectItem(overridableParametersJSON, "BIILarge")->valuedouble;
        component.overridableParameters.ahSmall = cJSON_GetObjectItem(overridableParametersJSON, "AHSmall")->valuedouble;
        component.overridableParameters.bhSmall = cJSON_GetObjectItem(overridableParametersJSON, "BHSmall")->valuedouble;
        component.overridableParameters.ahHuge = cJSON_GetObjectItem(overridableParametersJSON, "AHHuge")->valuedouble;
        component.overridableParameters.bhHuge = cJSON_GetObjectItem(overridableParametersJSON, "BHHuge")->valuedouble;
    }

    return component;
}


// Функция для парсинга Ptv
PtvTablePoint parsePtvTablePoint(cJSON* ptvTablePointJSON) {
    PtvTablePoint ptvTablePoint;
    safeStrCpy(ptvTablePoint.id, sizeof(ptvTablePoint.id), cJSON_GetObjectItem(cJSON_GetObjectItem(ptvTablePointJSON, "Id"), "Value")->valuestring);
    ptvTablePoint.pt = cJSON_GetObjectItem(ptvTablePointJSON, "PT")->valueint;
    ptvTablePoint.tm = cJSON_GetObjectItem(ptvTablePointJSON, "TM")->valuedouble;
    ptvTablePoint.rog = cJSON_GetObjectItem(ptvTablePointJSON, "ROG")->valuedouble;
    ptvTablePoint.rohl = cJSON_GetObjectItem(ptvTablePointJSON, "ROHL")->valuedouble;
    ptvTablePoint.rowt = cJSON_GetObjectItem(ptvTablePointJSON, "ROWT")->valuedouble;
    ptvTablePoint.drogdp = cJSON_GetObjectItem(ptvTablePointJSON, "DROGDP")->valuedouble;
    ptvTablePoint.drohldp = cJSON_GetObjectItem(ptvTablePointJSON, "DROHLDP")->valuedouble;
    ptvTablePoint.drowtdp = cJSON_GetObjectItem(ptvTablePointJSON, "DROWTDP")->valuedouble;
    ptvTablePoint.drogdt = cJSON_GetObjectItem(ptvTablePointJSON, "DROGDT")->valuedouble;
    ptvTablePoint.drohldt = cJSON_GetObjectItem(ptvTablePointJSON, "DROHLDT")->valuedouble;
    ptvTablePoint.drowtdt = cJSON_GetObjectItem(ptvTablePointJSON, "DROWTDT")->valuedouble;
    ptvTablePoint.rs = cJSON_GetObjectItem(ptvTablePointJSON, "RS")->valueint;
    ptvTablePoint.rsw = cJSON_GetObjectItem(ptvTablePointJSON, "RSW")->valuedouble;
    ptvTablePoint.visg = cJSON_HasObjectItem(ptvTablePointJSON, "VISG") ? cJSON_GetObjectItem(ptvTablePointJSON, "VISG")->valuedouble : 0.0;
    ptvTablePoint.vishl = cJSON_HasObjectItem(ptvTablePointJSON, "VISHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "VISHL")->valuedouble : 0.0;
    ptvTablePoint.viswt = cJSON_HasObjectItem(ptvTablePointJSON, "VISWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "VISWT")->valuedouble : 0.0;
    ptvTablePoint.cpg = cJSON_HasObjectItem(ptvTablePointJSON, "CPG") ? cJSON_GetObjectItem(ptvTablePointJSON, "CPG")->valuedouble : 0.0;
    ptvTablePoint.cphl = cJSON_HasObjectItem(ptvTablePointJSON, "CPHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "CPHL")->valuedouble : 0.0;
    ptvTablePoint.cpwt = cJSON_HasObjectItem(ptvTablePointJSON, "CPWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "CPWT")->valuedouble : 0.0;
    ptvTablePoint.hg = cJSON_HasObjectItem(ptvTablePointJSON, "HG") ? cJSON_GetObjectItem(ptvTablePointJSON, "HG")->valueint : 0;
    ptvTablePoint.hhl = cJSON_HasObjectItem(ptvTablePointJSON, "HHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "HHL")->valueint : 0;
    ptvTablePoint.hwt = cJSON_HasObjectItem(ptvTablePointJSON, "HWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "HWT")->valueint : 0;
    ptvTablePoint.tcg = cJSON_HasObjectItem(ptvTablePointJSON, "TCG") ? cJSON_GetObjectItem(ptvTablePointJSON, "TCG")->valuedouble : 0.0;
    ptvTablePoint.tchl = cJSON_HasObjectItem(ptvTablePointJSON, "TCHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "TCHL")->valuedouble : 0.0;
    ptvTablePoint.tcwt = cJSON_HasObjectItem(ptvTablePointJSON, "TCWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "TCWT")->valuedouble : 0.0;
    ptvTablePoint.sigghl = cJSON_HasObjectItem(ptvTablePointJSON, "SIGGHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "SIGGHL")->valuedouble : 0.0;
    ptvTablePoint.siggwt = cJSON_HasObjectItem(ptvTablePointJSON, "SIGGWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "SIGGWT")->valuedouble : 0.0;
    ptvTablePoint.sighlwt = cJSON_HasObjectItem(ptvTablePointJSON, "SIGHLWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "SIGHLWT")->valuedouble : 0.0;
    ptvTablePoint.seg = cJSON_HasObjectItem(ptvTablePointJSON, "SEG") ? cJSON_GetObjectItem(ptvTablePointJSON, "SEG")->valuedouble : 0.0;
    ptvTablePoint.sehl = cJSON_HasObjectItem(ptvTablePointJSON, "SEHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "SEHL")->valuedouble : 0.0;
    ptvTablePoint.sewt = cJSON_HasObjectItem(ptvTablePointJSON, "SEWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "SEWT")->valuedouble : 0.0;
    ptvTablePoint.order = cJSON_GetObjectItem(ptvTablePointJSON, "Order")->valueint;

    return ptvTablePoint;
}

// Функция для парсинга BubbleCurve
BubbleCurvePoint parseBubbleCurvePoint(cJSON* bubbleCurvePointJSON) {
    BubbleCurvePoint bubbleCurvePoint;
    cJSON* temperatureJSON = cJSON_GetObjectItem(bubbleCurvePointJSON, "Temperature");
    if (temperatureJSON != NULL) {
        bubbleCurvePoint.temperature = temperatureJSON->valuedouble;
    }
    else {
        bubbleCurvePoint.temperature = 0.0;
    }
    cJSON* pressureJSON = cJSON_GetObjectItem(bubbleCurvePointJSON, "Pressure");
    if (pressureJSON != NULL) {
        bubbleCurvePoint.pressure = pressureJSON->valueint;
    }
    else {
        bubbleCurvePoint.pressure = 0;
    }
    return bubbleCurvePoint;
}

// Функция для парсинга CriticalPoint
CriticalPoint* parseCriticalPoint(cJSON* criticalPointJSON) {
    if (criticalPointJSON == NULL || cJSON_IsNull(criticalPointJSON)) {
        return NULL;
    }

    CriticalPoint* criticalPoint = malloc(sizeof(CriticalPoint));

    cJSON* temperatureJSON = cJSON_GetObjectItem(criticalPointJSON, "Temperature");
    if (temperatureJSON != NULL) {
        criticalPoint->temperature = malloc(sizeof(double));
        *criticalPoint->temperature = temperatureJSON->valuedouble;
    }
    else {
        criticalPoint->temperature = NULL;
    }

    cJSON* pressureJSON = cJSON_GetObjectItem(criticalPointJSON, "Pressure");
    if (pressureJSON != NULL) {
        criticalPoint->pressure = malloc(sizeof(int));
        *criticalPoint->pressure = pressureJSON->valueint;
    }
    else {
        criticalPoint->pressure = NULL;
    }

    return criticalPoint;
}

// Функция для парсинга PhaseEnvelope 
PhaseEnvelope parsePhaseEnvelope(cJSON* phaseEnvelopeJSON) {
    PhaseEnvelope phaseEnvelope;
    safeStrCpy(phaseEnvelope.id, sizeof(phaseEnvelope.id), cJSON_GetObjectItem(cJSON_GetObjectItem(phaseEnvelopeJSON, "Id"), "Value")->valuestring);
    cJSON* criticalPointJSON = cJSON_GetObjectItem(phaseEnvelopeJSON, "CriticalPoint");
    phaseEnvelope.criticalPoint = parseCriticalPoint(criticalPointJSON);

    cJSON* bubbleCurvePointsJSON = cJSON_GetObjectItem(phaseEnvelopeJSON, "BubbleCurve");
    if (bubbleCurvePointsJSON != NULL && cJSON_IsArray(bubbleCurvePointsJSON)) {
        phaseEnvelope.bubbleCurveCount = cJSON_GetArraySize(bubbleCurvePointsJSON);
        phaseEnvelope.bubbleCurvePoints = malloc(phaseEnvelope.bubbleCurveCount * sizeof(BubbleCurvePoint));
        int i = 0;
        cJSON* bubbleCurvePointJSON = NULL;
        cJSON_ArrayForEach(bubbleCurvePointJSON, bubbleCurvePointsJSON) {
            phaseEnvelope.bubbleCurvePoints[i++] = parseBubbleCurvePoint(bubbleCurvePointJSON);
        }
    }

    return phaseEnvelope;
}


// Функция для парсинга данных 
Data* parseData(const char* jsonString) {
    //Data data;
    Data* data = (Data*)malloc(sizeof(Data));
    cJSON* root = cJSON_Parse(jsonString);

    if (root != NULL) {
        safeStrCpy(data->id, sizeof(data->id), cJSON_GetObjectItem(cJSON_GetObjectItem(root, "Id"), "Value")->valuestring);
        safeStrCpy(data->name, sizeof(data->name), cJSON_GetObjectItem(cJSON_GetObjectItem(root, "Name"), "Value")->valuestring);
        data->type = cJSON_GetObjectItem(root, "Type")->valueint;
        safeStrCpy(data->reservoir, sizeof(data->reservoir), cJSON_GetObjectItem(cJSON_GetObjectItem(root, "Reservoir"), "Value")->valuestring);
        data->phases = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "Phases"), "Value")->valueint;
        data->equationOfState = cJSON_GetObjectItem(root, "EquationOfState")->valuestring;
        data->stdPressure = cJSON_GetObjectItem(root, "StdPressure")->valueint;
        data->stdTemperature = cJSON_GetObjectItem(root, "StdTemperature")->valuedouble;
        data->stdGasDensity = cJSON_GetObjectItem(root, "StdGasDensity")->valuedouble;
        data->stdOilDensity = cJSON_GetObjectItem(root, "StdOilDensity")->valuedouble;
        data->stdLiqDensity = cJSON_GetObjectItem(root, "StdLiqDensity")->valuedouble;
        data->stdWatDensity = cJSON_GetObjectItem(root, "StdWatDensity")->valuedouble;
        data->gor = cJSON_GetObjectItem(root, "Gor")->valueint;
        data->glr = cJSON_GetObjectItem(root, "Glr")->valuedouble;
        data->wc = cJSON_GetObjectItem(root, "Wc")->valueint;
        data->totWaterFraction = cJSON_GetObjectItem(root, "TotWaterFraction")->valuedouble;



        cJSON* componentsJSON = cJSON_GetObjectItem(root, "Components");
        if (componentsJSON != NULL && cJSON_IsArray(componentsJSON)) {
            data->componentsCount = cJSON_GetArraySize(componentsJSON);
            data->components = malloc(data->componentsCount * sizeof(Component));
            int i = 0;
            cJSON* componentJSON = NULL;
            cJSON_ArrayForEach(componentJSON, componentsJSON) {
                data->components[i++] = parseComponent(componentJSON);
            }
        }

        cJSON* ptvTablePointsJSON = cJSON_GetObjectItem(root, "PtvTablePoints");
        if (ptvTablePointsJSON != NULL && cJSON_IsArray(ptvTablePointsJSON)) {
            int ptvTablePointsCount = cJSON_GetArraySize(ptvTablePointsJSON);
            data->ptvTablePoints = malloc(ptvTablePointsCount * sizeof(PtvTablePoint));
            int i = 0;
            cJSON* ptvTablePointJSON = NULL;
            cJSON_ArrayForEach(ptvTablePointJSON, ptvTablePointsJSON) {
                data->ptvTablePoints[i++] = parsePtvTablePoint(ptvTablePointJSON);
            }
        }

        cJSON* phaseEnvelopeJSON = cJSON_GetObjectItem(root, "PhaseEnvelope");
        if (phaseEnvelopeJSON != NULL) {
            data->phaseEnvelope = parsePhaseEnvelope(phaseEnvelopeJSON);
        }

        cJSON_Delete(root);
    }

    return data;
}

int main() {
    char file_path[256];
    printf("Enter JSON path: ");
    fgets(file_path, sizeof(file_path), stdin);
    file_path[strcspn(file_path, "\n")] = '\0';

    FILE* file = fopen(file_path, "r");
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

    Data* data = parseData(jsonString);

    // Вывод данных в консоль
    printData(*data);

    free(jsonString);

    // Освобождение памяти
    free(data->components);
    free(data->ptvTablePoints);

    return 0;
}
