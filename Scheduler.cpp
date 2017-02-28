#include "Scheduler.h"
#include "Priorities.h"
#include "Enumerations.h"
#include "Semaphores.h"
#include "Statistics.h"
#include "vectormath.h"
#include "Scratch.h"
#include "APTMKernel.h"
#include "BWSKernel.h"
#include "StaticKernel.h"




#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <cmath>

#define _INFO 1
#define _DEBUG 0;

Scheduler::Scheduler(Pipeline * p, _schedule_kernel _sche_type, \
	 unsigned _id): TimedRunnable(_id){
	///  initialize member vairables
	pipeline                      = p;
	thread_type                   = scheduler;
	kernel_type                   = _sche_type;
	
	/// set relative scheduling time instances
	unsigned long adaption_period = Scratch::getAdaptionPeriod();
	int count                     = 1;
	if (adaption_period != 0 ){
		unsigned long sim_length = Scratch::getDuration();
		count                    = sim_length/adaption_period;
		count                    = count == 0? 1 : count; // at least one adaption at beginning
	}
	
	for (int i = 0; i < count; ++i)
		rl_scheduling_times.push_back((unsigned long) i*adaption_period);	
}

Scheduler::~Scheduler()
{
	delete kernel;
	// cout << "scheduler with id " << id << "is being destructed\n";
}


void Scheduler::init(){
	unsigned nstages = Scratch::getNstage();

	// calculate history aware arrival curves statically for APTM and BWS kernel
	vector<jobject> haAlpha;
	if (kernel_type == APTM || kernel_type == BWS){
		rtc::initialize();
		long period 	= (long)Scratch::getPeriod()/1000;
		double jitter 	= (double)Scratch::getJitter()/1000;
		long delay  	= (long)Scratch::getDistance()/1000;
		vector<double> rl_job_arrivals = Scratch::getArrivalTimes_ms();
		vector<double> tmp_scheduling_times = rl_scheduling_times/1000;

		haAlpha = rtc::staticHistoryAwareArrialCurves(rl_job_arrivals,
	 					tmp_scheduling_times, period, jitter, delay);
	}

	// get the kernel for different kernel types
	switch (kernel_type){
		case APTM : {
		APTMKernel *tempkernel = new APTMKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), Scratch::getBFactor(), kernel_type);

			tempkernel->setHaAlpha(haAlpha, Scratch::getRltDeadline_ms());
			tempkernel->setOfflineData(Scratch::getOfflineData());
			kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case BWS: {
			BWSKernel *tempkernel = new BWSKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), kernel_type);
			tempkernel->setHaAlpha(haAlpha, Scratch::getRltDeadline_ms());
			kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case PBOO:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			ptmspec p = Scratch::getPTMValues();
			tempkernel->setStaticScheme( p.tons, p.toffs);
			kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case GE:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			ptmspec p = Scratch::getPTMValues();
			tempkernel->setStaticScheme( p.tons, p.toffs);
			kernel = (ScheduleKernel*) tempkernel;
			break;
		}

	}
	kernel -> setScheduler(this);
	
	// all initialized, post the semaphore to signal main thread scheduler is ready
	Semaphores::rtcinit_sem.post_sem();
}


void Scheduler::activate(){
	setPriority(Priorities::get_sched_pr(3));
}


void Scheduler::join(){
	join2();
}

void Scheduler::wrapper(){
	init();
	#if _INFO == 1
  	Semaphores::print_sem.wait_sem();
	cout << "Scheduler: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

	
  	//Wait until the simulation is initialized
	while( !Pipeline::isInitialized() );

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Scheduler: " << id << " wating for execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!Pipeline::isSimulating()){};

	timedRun();

	// if (kernel_type == APTM || kernel_type == BWS){
		cout << "Scheduler:: kernel max time expense: " << kernel->getMaxTimeExpense()<< endl;
		cout << "Scheduler:: kernel mean time expense: " << kernel->getMeanTimeExpense()<< endl;
	// }	
	

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Scheduler " << id << " exiting wrapper...\n";
	Semaphores::print_sem.post_sem();
  	#endif
}

void Scheduler::setAbsSchedulingTimes(unsigned long start_time){
	convertRelativeTimesToAbs(rl_scheduling_times, start_time);
}

void Scheduler::timedJob(unsigned _index){
	unsigned n = Scratch::getNstage() ;
	vector<unsigned long> ton, toff;
	vector<double> tons2;
	vector<double> toffs2;

	pipeinfo config;
	config.adaptionIndex = _index;
	kernel->getScheme(tons2, toffs2, config);
	
	if (tons2.size()!=n || toffs2.size()!= n){
		cerr << "Scheduler::timedJob:wrong scheduling scheme from the kernel " << endl;
		exit(2);
	}
	for (unsigned i = 0; i < n; ++i){
		ton.push_back(  (unsigned long)(tons2[i]*1000));
		toff.push_back(  (unsigned long)(toffs2[i]*1000));
	}

	pipeline->setPTMs(ton, toff);

}


/// Just an interface function needed by APTM and BWS kernel
void Scheduler::getPipelineInfo(pipeinfo& config,
const vector<double>& wcets, const vector<double>& TBET){

	pipeline->getInfo(config, wcets, TBET, kernel_type);
	
}
	
