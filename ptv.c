#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include "cJSON.h"
#include "ptv.h"

static void safeStrCpy(char* destination, size_t size, const char* source) {
    #if defined _MSC_VER || defined __MINGW32__ || defined __MINGW64__
        if (strcpy_s(destination, size, source) != 0) {
            perror("Error copying string");
            exit(1);
        }
    #else
        strncpy(destination, source, size);
    #endif
}


static PtvTablePoint parsePtvTablePoint(cJSON* ptvTablePointJSON) {
    PtvTablePoint ptvTablePoint;
    ptvTablePoint.pt = cJSON_GetObjectItem(ptvTablePointJSON, "PT")->valuedouble;
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
    ptvTablePoint.rs = cJSON_GetObjectItem(ptvTablePointJSON, "RS")->valuedouble;
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
    return ptvTablePoint;
}

Data parseData(const char* jsonString) {
    Data data;
    cJSON* root = cJSON_Parse(jsonString);

    if (root != NULL) {
        cJSON* ptvTablePointsJSON = cJSON_GetObjectItem(root, "PtvTablePoints");
        if (ptvTablePointsJSON != NULL && cJSON_IsArray(ptvTablePointsJSON)) {
            data.ptvTablePointsCount = cJSON_GetArraySize(ptvTablePointsJSON);
            data.ptvTablePoints = malloc(data.ptvTablePointsCount * sizeof(PtvTablePoint));
            int i = 0;
            cJSON* ptvTablePointJSON = NULL;
            cJSON_ArrayForEach(ptvTablePointJSON, ptvTablePointsJSON) {
                data.ptvTablePoints[i++] = parsePtvTablePoint(ptvTablePointJSON);
            }
        }
    }
    cJSON_Delete(root);
    return data;
}

void freeData(Data data) {
    free(data.ptvTablePoints);
}

static PtvTablePoint* InterpolateLinearPoint(PtvTablePoint point1, PtvTablePoint point2, double x, int type) {
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

static PtvTablePoint* Interpolate(PtvTablePoint point1, PtvTablePoint point2, double by, int type, double dop) {
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

PtvTablePoint* computePoint(double Temperature, double Pressure, Data data) {
    PtvTablePoint* ptvResultPoint = (PtvTablePoint*)malloc(sizeof(PtvTablePoint));
    PtvTablePoint* PtvTable = data.ptvTablePoints;
    int count = data.ptvTablePointsCount;
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
        if (PtvTable[i].pt > Pressure && !presD_set) {
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
        freePoint(ptvDTPoint[0]);
        freePoint(ptvDTPoint[1]);
        //*ptvResultPoint.message = "all good";
    }
    return ptvResultPoint;
}

void freePoint(PtvTablePoint* point) {
    free(point);
}