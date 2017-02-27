#ifndef _SCRATCH_H
#define _SCRATCH_H

#include "Enumerations.h"
#include "structdef.h"
#include "rtc.h"

#include <vector>
#include <string>
#include <semaphore.h>
using namespace std;


class Scratch{
public:

	static void initialize(int, unsigned long, unsigned long, unsigned long,
		unsigned long, vector<unsigned long>, vector<unsigned long>,
		enum _schedule_kernel, unsigned long, string);


	static void 					setAdaptionPeriod(unsigned long);
	static void 					setPTMValues(ptmspec);
	static void 					setOfflineData(thermalProp);
	static void 					setExeFactor(double);
	static void 					setBFactor(double f);
	static void 					setName(string);
	

	static unsigned long 			getAdaptionPeriod();
	static thermalProp 				getOfflineData();
	static ptmspec 					getPTMValues();
	static string 					getName();
	static int 						getNstage();
	static unsigned long 			getPeriod();
	static unsigned long 			getJitter();
	static unsigned long 			getDistance();
	static unsigned long 			getRltDeadline();
	static double 					getRltDeadline_ms();
	static vector<unsigned long> 	getWcets();
	static vector<double> 			getDwcets();
	static vector<unsigned long> 	getArrivalTimes();
	static vector<double> 			getArrivalTimes_ms();
	static enum _schedule_kernel 	getKernel();
	static unsigned long 			getDuration();
	static double 					getExeFactor();
	static double 					getBFactor();
	static void 					print();


private:
	
	static string 					name;
	static int 						nstage;
	static unsigned long 			period;
	static unsigned long 			jitter;
	static unsigned long 			distance;
	static unsigned long 			rltDeadline;
	static double 					rltDeadline_ms;
	static vector<unsigned long> 	wcets;
	static vector<double> 			dwcets;
	static vector<unsigned long> 	arrival_times;
	static enum _schedule_kernel 	kernel;
	static unsigned long 			duration;

	static double 					exefactor;
	static double 					bfactor;

	static unsigned long 			adaption_period;
	static ptmspec 					ptm;
	static thermalProp 				offlinedata;

	static sem_t 					access_sem;
};


#endif 