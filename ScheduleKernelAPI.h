#ifndef SCHEDULEKERNELAPI_H
#define SCHEDULEKERNELAPI_H 

#include <vector>

// #include "Scheduler.h"
// #include "ScheduleKernel.h"
// #include "Scratch.h"
#include "ScheduleKernelEnumerations.h"
#include "structdef.h"

class ScheduleKernel;
class Scheduler;
class Pipeline;

class ScheduleKernelAPI{

private:
	ScheduleKernel* _kernel;

	Scheduler* _scheduler;

	std::vector<std::vector<double> > timeExpense;

	std::vector<std::vector<double> > allschemes;
	// std::vector<std::vector<double> > alltoffs;

public:
	ScheduleKernelAPI(Scheduler* ,enum _schedule_kernel, std::vector<unsigned long>&);

	void runKernel(std::vector<double>&, std::vector<double>&);

	double getMaxTimeExpense();

	double getMeanTimeExpense();

	std::vector<std::vector<double> > getKernelTimeExpenseLog();

	void getPipelineInfo(PipelineInfo& pinfo);

	void getNewWorkerInfo(std::vector<newWorkerInfo>& info);

	std::vector<std::vector<double> > getAllSchemes();

};



#endif