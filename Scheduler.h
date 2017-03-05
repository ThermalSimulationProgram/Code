#ifndef _SCHEDULER_H
#define _SCHEDULER_H


#include "Pipeline.h"
#include "Thread.h"
#include "Enumerations.h"
#include "TimeUtil.h"
#include "TimedRunnable.h"
#include "TimeSim.h"
#include "rtc.h"
#include "structdef.h"




#include <vector>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

class ScheduleKernel;

class Scheduler : public TimedRunnable
{
protected:

	/////////////////////////////////////////////
	vector<unsigned long> rl_scheduling_times;
	
	Pipeline * pipeline;

	enum _schedule_kernel kernel_type;
	
	ScheduleKernel *kernel;

public:
	Scheduler(Pipeline *, _schedule_kernel,  unsigned);
	
	~Scheduler();

	void init();
	// bool isInitialized();
	void activate();

	void wrapper();

	void setAbsSchedulingTimes(unsigned long);
	void join();

	void timedJob(unsigned);

	void getPipelineInfo(pipeinfo&, const vector<double>&, const vector<double>&);

	vector<double> getKernelTime();

};






#endif