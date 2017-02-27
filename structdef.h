#ifndef _STRUCTDEF_H
#define _STRUCTDEF_H 

#include <vector>

using namespace std;

#include "warmingCurve.h"
#include "Enumerations.h"

typedef struct offlinedata{
	vector<vector<double>> coolbreakToffs;
	vector<vector<double>> coolslopes;
	vector<int> numValidData;
	vector<warmingCurves> allStageCurves;
}thermalProp;

typedef struct pipeinfo
{
	vector<int>   Q;
	vector<int> activeSet;
	vector<int> sleepSet;
	vector<double> ccs;
	vector<double> dcs;
	vector<double> rho;
	vector<double> K;
	vector<vector<double>> FIFOcurveData;
	vector<double> allT;
	unsigned adaptionIndex;
} pipeinfo;

void pipeinfo_print(const pipeinfo &);

typedef struct ptmspec{
	vector<double> tons;
	vector<double> toffs;
}ptmspec;


typedef struct dynamicinfo
{
	vector<double> rho;
	vector<vector<double>> breakToffs;
	vector<vector<double>> slopes;
	vector<int> numValidData;
	vector<linearSegs> warmingcurves;
	vector<double> K;
	
}dynamicdata;


typedef struct{
	int stageId;
	int nFIFOJobs;
	double executed;
	enum _worker_state state;
	double sleepTime;
	int onGoEventId;
	vector<double> allEventAbsDeadlines;
} workerinfo;




#endif


