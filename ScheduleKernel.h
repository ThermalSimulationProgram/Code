#ifndef _SCHEDULEKERNEL_H
#define _SCHEDULEKERNEL_H 

#include <vector>

#include "structdef.h"
#include "Scheduler.h"

using namespace std;


class ScheduleKernel{
protected:
	//static data
	unsigned nstages;
	
	enum _schedule_kernel kernel_type;

	Scheduler *scheduler;

	vector<double> timeExpense;

public:
	ScheduleKernel(unsigned n, enum _schedule_kernel kernel_type);
	
	void setScheduler(Scheduler*);

	void getScheme(vector<double> &, vector<double>&, pipeinfo&);
	virtual void getScheduleScheme(vector<double> &, vector<double>&, pipeinfo&) = 0;


	double getMaxTimeExpense();
	double getMeanTimeExpense();
	
};


#endif