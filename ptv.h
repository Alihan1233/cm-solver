#ifndef ptv_h
#define ptv_h

#ifdef __cplusplus
    extern "C" {
#endif

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
} PtvTablePoint;


typedef struct {
    PtvTablePoint* ptvTablePoints;
    int ptvTablePointsCount;
} Data;

typedef struct {
    PtvTablePoint* point;
    char* message;
} PtvTablePointNoGood;

Data parseData(const char* jsonString);
void freeData(Data data);

PtvTablePoint* computePoint(double Temperature, double Pressure, Data data);
void freePoint(PtvTablePoint* point);

#ifdef __cplusplus
    }
#endif

#endif