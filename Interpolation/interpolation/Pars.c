#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <locale.h>
#include <math.h>

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


typedef struct {
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

// ��������� ��� BubbleCurve
typedef struct {
    double temperature;
    double pressure;
} BubbleCurvePoint;

// ��������� ��� CriticalPoint
typedef struct {
    double* temperature;
    double* pressure;
} CriticalPoint;

// ��������� ��� PhaseEnvelope
typedef struct {
    char id[50];
    CriticalPoint* criticalPoint;
    BubbleCurvePoint* bubbleCurvePoints;
    int bubbleCurveCount;
} PhaseEnvelope;

// ��������� ��� �������� ������
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
    int ptvTablePointsCount;
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


// ������� ��� �������� ����������
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

PtvTablePoint parsePtvTablePoint(cJSON* ptvTablePointJSON) {
    PtvTablePoint ptvTablePoint;
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
    ptvTablePoint.hg = cJSON_HasObjectItem(ptvTablePointJSON, "HG") ? cJSON_GetObjectItem(ptvTablePointJSON, "HG")->valuedouble : 0.0;
    ptvTablePoint.hhl = cJSON_HasObjectItem(ptvTablePointJSON, "HHL") ? cJSON_GetObjectItem(ptvTablePointJSON, "HHL")->valuedouble : 0.0;
    ptvTablePoint.hwt = cJSON_HasObjectItem(ptvTablePointJSON, "HWT") ? cJSON_GetObjectItem(ptvTablePointJSON, "HWT")->valuedouble : 0.0;
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

// ������� ��� �������� BubbleCurve
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

// ������� ��� �������� CriticalPoint
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

// ������� ��� �������� PhaseEnvelope 
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


// ������� ��� �������� ������ 
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
        int ptvTablePointsCount = cJSON_GetArraySize(ptvTablePointsJSON);
        if (ptvTablePointsJSON != NULL && cJSON_IsArray(ptvTablePointsJSON)) {
            data->ptvTablePointsCount = cJSON_GetArraySize(ptvTablePointsJSON);
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


PtvTablePoint* InterpolateLinearPoint(PtvTablePoint point1, PtvTablePoint point2, double x, int type) {
    PtvTablePoint* resultinter = (PtvTablePoint*)malloc(sizeof(PtvTablePoint));
    if (type == 1) {
        resultinter->tm = x;
        resultinter->tm = point1.tm;
        resultinter->rog = ((x - point1.tm) * (point1.rog - point2.rog)) / (point1.tm - point2.tm) + point1.rog;
        resultinter->rohl = ((x - point1.tm) * (point1.rohl - point2.rohl)) / (point1.tm - point2.tm) + point1.rohl;
        resultinter->rowt = ((x - point1.tm) * (point1.rowt - point2.rowt)) / (point1.tm - point2.tm) + point1.rowt;
        resultinter->drogdp = ((x - point1.tm) * (point1.drogdp - point2.drogdp)) / (point1.tm - point2.tm) + point1.drogdp;
        resultinter->drohldp = ((x - point1.tm) * (point1.drohldp - point2.drohldp)) / (point1.tm - point2.tm) + point1.drohldp;
        resultinter->drowtdp = ((x - point1.tm) * (point1.drowtdp - point2.drowtdp)) / (point1.tm - point2.tm) + point1.drowtdp;
        resultinter->drogdt = ((x - point1.tm) * (point1.drogdt - point2.drogdt)) / (point1.tm - point2.tm) + point1.drogdt;
        resultinter->drohldt = ((x - point1.tm) * (point1.drohldt - point2.drohldt)) / (point1.tm - point2.tm) + point1.drohldt;
        resultinter->drowtdt = ((x - point1.tm) * (point1.drowtdt - point2.drowtdt)) / (point1.tm - point2.tm) + point1.drowtdt;
        resultinter->rs = ((x - point1.tm) * (point1.rs - point2.rs)) / (point1.tm - point2.tm) + point1.rs;
        resultinter->rsw = ((x - point1.tm) * (point1.rsw - point2.rsw)) / (point1.tm - point2.tm) + point1.rsw;
        resultinter->visg = ((x - point1.tm) * (point1.visg - point2.visg)) / (point1.tm - point2.tm) + point1.visg;
        resultinter->vishl = ((x - point1.tm) * (point1.vishl - point2.vishl)) / (point1.tm - point2.tm) + point1.vishl;
        resultinter->viswt = ((x - point1.tm) * (point1.viswt - point2.viswt)) / (point1.tm - point2.tm) + point1.viswt;
        resultinter->cpg = ((x - point1.tm) * (point1.cpg - point2.cpg)) / (point1.tm - point2.tm) + point1.cpg;
        resultinter->cphl = ((x - point1.tm) * (point1.cphl - point2.cphl)) / (point1.tm - point2.tm) + point1.cphl;
        resultinter->cpwt = ((x - point1.tm) * (point1.cpwt - point2.cpwt)) / (point1.tm - point2.tm) + point1.cpwt;
        resultinter->hg = ((x - point1.tm) * (point1.hg - point2.hg)) / (point1.tm - point2.tm) + point1.hg;
        resultinter->hhl = ((x - point1.tm) * (point1.hhl - point2.hhl)) / (point1.tm - point2.tm) + point1.hhl;
        resultinter->hwt = ((x - point1.tm) * (point1.hwt - point2.hwt)) / (point1.tm - point2.tm) + point1.hwt;
        resultinter->tcg = ((x - point1.tm) * (point1.tcg - point2.tcg)) / (point1.tm - point2.tm) + point1.tcg;
        resultinter->tchl = ((x - point1.tm) * (point1.tchl - point2.tchl)) / (point1.tm - point2.tm) + point1.tchl;
        resultinter->tcwt = ((x - point1.tm) * (point1.tcwt - point2.tcwt)) / (point1.tm - point2.tm) + point1.tcwt;
        resultinter->sigghl = ((x - point1.tm) * (point1.sigghl - point2.sigghl)) / (point1.tm - point2.tm) + point1.sigghl;
        resultinter->siggwt = ((x - point1.tm) * (point1.siggwt - point2.siggwt)) / (point1.tm - point2.tm) + point1.siggwt;
        resultinter->sighlwt = ((x - point1.tm) * (point1.sighlwt - point2.sighlwt)) / (point1.tm - point2.tm) + point1.sighlwt;
        resultinter->seg = ((x - point1.tm) * (point1.seg - point2.seg)) / (point1.tm - point2.tm) + point1.seg;
        resultinter->sehl = ((x - point1.tm) * (point1.sehl - point2.sehl)) / (point1.tm - point2.tm) + point1.sehl;
        resultinter->sewt = ((x - point1.tm) * (point1.sewt - point2.sewt)) / (point1.tm - point2.tm) + point1.sewt;

        return resultinter;
    }
    if (type == 2) {
        resultinter->pt = x;
        resultinter->tm = point1.tm;
        resultinter->rog = ((x - point1.pt) * (point1.rog - point2.rog)) / (point1.pt - point2.pt) + point1.rog;
        resultinter->rohl = ((x - point1.pt) * (point1.rohl - point2.rohl)) / (point1.pt - point2.pt) + point1.rohl;
        resultinter->rowt = ((x - point1.pt) * (point1.rowt - point2.rowt)) / (point1.pt - point2.pt) + point1.rowt;
        resultinter->drogdp = ((x - point1.pt) * (point1.drogdp - point2.drogdp)) / (point1.pt - point2.pt) + point1.drogdp;
        resultinter->drohldp = ((x - point1.pt) * (point1.drohldp - point2.drohldp)) / (point1.pt - point2.pt) + point1.drohldp;
        resultinter->drowtdp = ((x - point1.pt) * (point1.drowtdp - point2.drowtdp)) / (point1.pt - point2.pt) + point1.drowtdp;
        resultinter->drogdt = ((x - point1.pt) * (point1.drogdt - point2.drogdt)) / (point1.pt - point2.pt) + point1.drogdt;
        resultinter->drohldt = ((x - point1.pt) * (point1.drohldt - point2.drohldt)) / (point1.pt - point2.pt) + point1.drohldt;
        resultinter->drowtdt = ((x - point1.pt) * (point1.drowtdt - point2.drowtdt)) / (point1.pt - point2.pt) + point1.drowtdt;
        resultinter->rs = ((x - point1.pt) * (point1.rs - point2.rs)) / (point1.pt - point2.pt) + point1.rs;
        resultinter->rsw = ((x - point1.pt) * (point1.rsw - point2.rsw)) / (point1.pt - point2.pt) + point1.rsw;
        resultinter->visg = ((x - point1.pt) * (point1.visg - point2.visg)) / (point1.pt - point2.pt) + point1.visg;
        resultinter->vishl = ((x - point1.pt) * (point1.vishl - point2.vishl)) / (point1.pt - point2.pt) + point1.vishl;
        resultinter->viswt = ((x - point1.pt) * (point1.viswt - point2.viswt)) / (point1.pt - point2.pt) + point1.viswt;
        resultinter->cpg = ((x - point1.pt) * (point1.cpg - point2.cpg)) / (point1.pt - point2.pt) + point1.cpg;
        resultinter->cphl = ((x - point1.pt) * (point1.cphl - point2.cphl)) / (point1.pt - point2.pt) + point1.cphl;
        resultinter->cpwt = ((x - point1.pt) * (point1.cpwt - point2.cpwt)) / (point1.pt - point2.pt) + point1.cpwt;
        resultinter->hg = ((x - point1.pt) * (point1.hg - point2.hg)) / (point1.pt - point2.pt) + point1.hg;
        resultinter->hhl = ((x - point1.pt) * (point1.hhl - point2.hhl)) / (point1.pt - point2.pt) + point1.hhl;
        resultinter->hwt = ((x - point1.pt) * (point1.hwt - point2.hwt)) / (point1.pt - point2.pt) + point1.hwt;
        resultinter->tcg = ((x - point1.pt) * (point1.tcg - point2.tcg)) / (point1.pt - point2.pt) + point1.tcg;
        resultinter->tchl = ((x - point1.pt) * (point1.tchl - point2.tchl)) / (point1.pt - point2.pt) + point1.tchl;
        resultinter->tcwt = ((x - point1.pt) * (point1.tcwt - point2.tcwt)) / (point1.pt - point2.pt) + point1.tcwt;
        resultinter->sigghl = ((x - point1.pt) * (point1.sigghl - point2.sigghl)) / (point1.pt - point2.pt) + point1.sigghl;
        resultinter->siggwt = ((x - point1.pt) * (point1.siggwt - point2.siggwt)) / (point1.pt - point2.pt) + point1.siggwt;
        resultinter->sighlwt = ((x - point1.pt) * (point1.sighlwt - point2.sighlwt)) / (point1.pt - point2.pt) + point1.sighlwt;
        resultinter->seg = ((x - point1.pt) * (point1.seg - point2.seg)) / (point1.pt - point2.pt) + point1.seg;
        resultinter->sehl = ((x - point1.pt) * (point1.sehl - point2.sehl)) / (point1.pt - point2.pt) + point1.sehl;
        resultinter->sewt = ((x - point1.pt) * (point1.sewt - point2.sewt)) / (point1.pt - point2.pt) + point1.sewt;

        return resultinter;
    }
}

PtvTablePoint* Interpolate(PtvTablePoint point1, PtvTablePoint point2, double by, int type, double dop) {
    double midPT, midTM;
    PtvTablePoint* result = (PtvTablePoint*)malloc(sizeof(PtvTablePoint));
    if (type == 1 || type == 3) {
        if (type == 1) {
            midPT = point1.pt;
            midTM = by;
        }
        else {
            midPT = by;
            midTM = dop;
            by = dop;
        }
        result = InterpolateLinearPoint(point1, point2, by, 1);
        result->pt = midPT;
        result->tm = midTM;
    }
    else {
        if (type == 2) {
            midPT = by;
            midTM = point1.tm;
        }
        else {
            midPT = dop;
            midTM = by;
            by = dop;
        }
        result = InterpolateLinearPoint(point1, point2, by, 2);
        result->pt = midPT;
        result->tm = midTM;
    }
    return result;
}

PtvTablePoint* CalculatePtvTablePoint(double Temperature, double Pressure, PtvTablePoint* PtvTable, int count) {
    PtvTablePoint* ptvResultPoint = (PtvTablePoint*)malloc(sizeof(PtvTablePoint));
    //ptvResultPoint.point = malloc(sizeof(PtvTablePoint));
    int presD_set = 0, presT_set = 0;
    int tempL_set = 0, tempR_set = 0;
    double presD = 0, presT = 0;
    double tempL = 0, tempR = 0;
    char* type;
    if (Temperature < PtvTable[0].tm) {
        tempL = PtvTable[0].tm;
        tempR = PtvTable[0].tm;
    }
    else if (Temperature > PtvTable[count - 1].tm) {
        tempL = PtvTable[count - 1].tm;
        tempR = PtvTable[count - 1].tm;
    }
    if (Pressure < PtvTable[0].pt) {
        presD = PtvTable[0].pt;
        presT = PtvTable[0].pt;
    }
    else if (Pressure > PtvTable[count - 1].pt) {
        presD = PtvTable[count - 1].pt;
        presT = PtvTable[count - 1].pt;
    }
    for (int i = 0; i < count; i++) {
        if (i > 0 && PtvTable[i].pt > Pressure && !presD_set) {
            presD = PtvTable[i - 1].pt;
            presT = PtvTable[i].pt;
            presD_set = 1;
            presT_set = 1;
        }
        if (PtvTable[i].tm > Temperature && !tempL_set) {
            tempL = PtvTable[i - 1].tm;
            tempR = PtvTable[i].tm;
            tempL_set = 1;
            tempR_set = 1;
        }
        if (presD_set && presT_set && tempL_set && tempR_set) {
            break;
        }
    }
    //double presD = 621052, presT = 1142100;
    //double tempL = 0, tempR = 3.1579;
    //char* type;

    int countTop = 0;
    int countDown = 0;

    if (presD == PtvTable[count - 1].pt && presT == PtvTable[count - 1].pt && tempL != 0 && tempR != 0) {
        PtvTablePoint ptvTop[2];
        for (int i = 0; i < count; i++) {
            if ((PtvTable[i].pt == presD && PtvTable[i].tm == tempR) || (PtvTable[i].pt == presD && PtvTable[i].tm == tempL)) {
                ptvTop[countTop++] = PtvTable[i];
            }
        }
        ptvResultPoint = Interpolate(ptvTop[0], ptvTop[1], Pressure, 3, Temperature);
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else if (presD == PtvTable[0].pt && presT == PtvTable[0].pt && tempL != 0 && tempR != 0) {
        PtvTablePoint ptvTop[2];
        for (int i = 0; i < count - 1; i++) {
            if ((PtvTable[i].pt == presD && PtvTable[i].tm == tempR) || (PtvTable[i].pt == presD && PtvTable[i].tm == tempL)) {
                ptvTop[countTop++] = PtvTable[i];
            }
        }
        ptvResultPoint = Interpolate(ptvTop[0], ptvTop[1], Pressure, 3, Temperature);
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else if (presD != 0 && presT != 0 && tempL == PtvTable[count - 1].tm && tempR == PtvTable[count - 1].tm) {
        PtvTablePoint ptvDown[2];
        for (int i = 0; i < count; i++) {
            if ((PtvTable[i].pt == presD && PtvTable[i].tm == tempL) || (PtvTable[i].pt == presT && PtvTable[i].tm == tempR)) {
                ptvDown[countDown++] = PtvTable[i];
            }
        }
        ptvResultPoint = Interpolate(ptvDown[0], ptvDown[1], Temperature, 4, Pressure);
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else if (presD != 0 && presT != 0 && tempL == PtvTable[0].tm && tempR == PtvTable[0].tm) {
        PtvTablePoint ptvDown[2];
        for (int i = 0; i < count; i++) {
            if ((PtvTable[i].pt == presD && PtvTable[i].tm == tempL) || (PtvTable[i].pt == presT && PtvTable[i].tm == tempR)) {
                ptvDown[countDown++] = PtvTable[i];
            }
        }
        ptvResultPoint = Interpolate(ptvDown[0], ptvDown[1], Temperature, 4, Pressure);
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else if (presD == PtvTable[count - 1].pt && presT == PtvTable[count - 1].pt && tempL == PtvTable[count - 1].tm && tempR == PtvTable[count - 1].tm) {
        PtvTablePoint* ptvRes = (PtvTablePoint*)malloc(sizeof(PtvTable[count - 1]));
        ptvRes->pt = Pressure;
        ptvRes->tm = Temperature;
        ptvResultPoint = ptvRes;
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else if (presD == PtvTable[0].pt && presT == PtvTable[0].pt && tempL == PtvTable[0].tm && tempR == PtvTable[0].tm) {
        PtvTablePoint* ptvRes = (PtvTablePoint*)malloc(sizeof(PtvTable[0]));
        ptvRes->pt = Pressure;
        ptvRes->tm = Temperature;
        ptvResultPoint = ptvRes;
        //*ptvResultPoint.message = "there may be an error in the calculation";
    }
    else {
        PtvTablePoint ptvDown[2];
        PtvTablePoint ptvTop[2];
        for (int i = 0; i < count; i++) {
            if ((PtvTable[i].pt == presD && PtvTable[i].tm == tempL) || (PtvTable[i].pt == presD && PtvTable[i].tm == tempR)) {
                ptvDown[countDown] = PtvTable[i];
                countDown++;
            }
            if ((PtvTable[i].pt == presT && PtvTable[i].tm == tempR) || (PtvTable[i].pt == presT && PtvTable[i].tm == tempL)) {
                ptvTop[countTop] = PtvTable[i];
                countTop++;
            }
        }
        PtvTablePoint* ptvDTPoint[2];
        ptvDTPoint[0] = Interpolate(ptvDown[0], ptvDown[1], Temperature, 1, 0);
        ptvDTPoint[1] = Interpolate(ptvTop[0], ptvTop[1], Temperature, 1, 0);
        ptvResultPoint = Interpolate(*ptvDTPoint[0], *ptvDTPoint[1], Pressure, 2, 0);
        //*ptvResultPoint.message = "all good";
    }

    return ptvResultPoint;
}

int main(int argc, char* argv[]) {
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

    double PT = 999998;
    double TM = 3.1556679;

    PtvTablePoint* resulty = CalculatePtvTablePoint(TM, PT, data->ptvTablePoints, data->ptvTablePointsCount);

    printf("PT: %f\n", resulty->pt);
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
    printf("RS: %f\n", resulty->rs);
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

    free(jsonString);
    free(data->ptvTablePoints);
    return 0;
}
