#include "Worker.h"

#include <iostream>
#include <math.h>
#include <iterator>
#include <stdlib.h>

#include "Enumerations.h"
#include "Semaphores.h"
#include "TimeUtil.h"
#include "Operators.h"
#include "Statistics.h"
#include "Priorities.h"
#include "Scratch.h"
#include "Pipeline.h"


using namespace std;

#define _INFO 1
#define _DEBUG 0



Worker::Worker(int _stageId, int _id) : Thread(_id), load(Scratch::getBenchmarkName()),
coolshaper(this){
	stageId     = _stageId; 
	next        = NULL;
	thread_type = worker;
	current_job = NULL;
	state 		= _active;

	base        = 100;
	ton         = 100000; // default value, unit us 
	toff        = 0; // default value, unit us 
	
	sem_init(&FIFO_sem, 0, 1);
	sem_init(&schedule_sem, 0, 0);
	sem_init(&ptm_sem, 0, 1);
	sem_init(&state_sem, 0, 1);
	sem_init(&job_sem, 0, 1);


	latestSleep = TimeUtil::Millis(0);

	isInterruptValid = false;
	sleepLength = 0;
}

Worker::~Worker(){
	 // std::cout << "worker with id " << id << " is being destructed\n";
}

// void Worker::init(){
// 	trigger();
// 	initialized = true;
// }

bool Worker::isInitialized(){
	return initialized;
}

void Worker::setNext(Worker* _next){
	next = _next;
}


void Worker::join(){
	sem_post(&FIFO_sem);
	sem_post(&FIFO_sem);
	sem_post(&FIFO_sem);
	sem_post(&schedule_sem);
	sem_post(&schedule_sem);
	sem_post(&ptm_sem);
	sem_post(&ptm_sem);
	sem_post(&state_sem);
	disableInterrupt();
	join2();
}

void Worker::getNewInfo(newWorkerInfo& ret){
	// cout << "getNewInfo 1" << endl;
	ret.allEventAbsDeadlines = getAllAbsDeadline_ms();

	// cout << "getNewInfo 2" << endl;
	ret.allEventLoads = getAllLoads_ms();
	// cout << "getNewInfo 3" << endl;
}

void Worker::getAllInfo(double now, WorkerInfo & ret){
	ret.stageId = stageId;
	struct timespec tmp;
	
	sem_wait(&state_sem);
	ret.state = state;
	if (state == _sleep)
		tmp = latestSleep;
	sem_post(&state_sem);

	#if _DEBUG==1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " already got its state\n";
	Semaphores::print_sem.post_sem();
	#endif
	
	if (ret.state == _sleep){
		unsigned long rlLatestSleep = TimeUtil::convert_us(
			tmp - Statistics::getStart());
		ret.sleepTime = (now - (((double)rlLatestSleep)/1000));
	}
	else
		ret.sleepTime = 0;
	
	ret.allEventAbsDeadlines = getAllAbsDeadline_ms();
	#if _DEBUG==1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " already got its job deadlines\n";
	Semaphores::print_sem.post_sem();
	#endif

	ret.nFIFOJobs            =  (int)ret.allEventAbsDeadlines.size();
	
	sem_wait(&job_sem);
	if (current_job != NULL){
		ret.onGoEventId = (int)current_job->getId();
		ret.executed    = ((double)current_job->getABET())/1000;
	}
	else{
		ret.onGoEventId = 0;
		ret.executed    = 0;
	}
	sem_post(&job_sem);
}

unsigned long Worker::getExecuted(){
	unsigned long ret = 0;
	sem_wait(&job_sem);
	if (current_job != NULL){
		ret = current_job->getABET();
	}
	sem_post(&job_sem);
	return ret;
}

vector<unsigned long> Worker::getAllAbsDeadline(){
	vector<unsigned long> ret;
	sem_wait(&job_sem);
	if (current_job != NULL){
		ret.push_back(current_job->getAbsDeadline());
	}
	sem_post(&job_sem);

	sem_wait(&FIFO_sem);
	for (list<Job*>::iterator it = FIFO.begin(); it !=  FIFO.end(); it++)
	{
		ret.push_back((*it)->getAbsDeadline());
	}
	sem_post(&FIFO_sem);
	return ret;
}

vector<double> Worker::getAllLoads_ms(){
	vector<double> ret;
	sem_wait(&job_sem);
	if (current_job != NULL){
		double maxLoad = current_job->getCurrentWCET() - current_job->getABET();
		ret.push_back((double)maxLoad/1000);
	}
	sem_post(&job_sem);
	sem_wait(&FIFO_sem);
	for (list<Job*>::iterator it = FIFO.begin(); it !=  FIFO.end(); it++)
	{
		ret.push_back((double) ( (*it)->getCurrentWCET() - (*it)->getABET()  )/1000);
	}
	sem_post(&FIFO_sem);
	return ret;
}

vector<double> Worker::getAllAbsDeadline_ms(){
	vector<double> ret;
	sem_wait(&job_sem);
	if (current_job != NULL){
		ret.push_back((double)current_job->getAbsDeadline()/1000);
	}
	sem_post(&job_sem);
	sem_wait(&FIFO_sem);
	for (list<Job*>::iterator it = FIFO.begin(); it !=  FIFO.end(); it++)
	{
		ret.push_back((double) ((*it)->getAbsDeadline())/1000);
	}
	sem_post(&FIFO_sem);
	return ret;
}

void Worker::activate(){
	setPriority(Priorities::get_active_pr());
}

void Worker::wrapper(){

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

  	//Wait until the simulation is initialized
	//while( !Pipeline::isInitialized() );
	sem_wait(&Pipeline::init_sem);
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for simulation start\n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	//while(!Pipeline::isSimulating()){};
	sem_wait(&Pipeline::running_sem);

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " begining execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

  	if (Scratch::getKernel() == CS){
  		coolshaper.init();
  		coolshaper.runShaper();
  		while(Pipeline::isSimulating()){
  			switch (nextAction) {
  				case _run:{
  					runTask( (unsigned long) (coolshaper.getWunit() * 1000));
  					break;
  				}
  				case _waiting:{
  					waiting();
  					break;
  				}
  				case _idle:{
  					idle(sleepLength);
  					break;
  				}
  			}
  		}
  		
  		


  	}else{

  		unsigned long exedSlice;
  		unsigned long start;
  		unsigned long end;
  		unsigned long total_exed;

  		int sleep_counter = 0;
  	
	// unsigned int seed;

  		while(Pipeline::isSimulating())
	// while(1)
  		{
  			if  (toff >= 100 && sleep_counter >= 0){
  				sem_wait(&state_sem);
  				latestSleep = TimeUtil::getTime();
  				state = _sleep;
  				sem_post(&state_sem);
			#if _DEBUG == 1
  				Semaphores::print_sem.wait_sem();
  				cout << "Worker: " << id << " waiting for sem_ptm\n";
  				Semaphores::print_sem.post_sem();
  			#endif

  				sem_wait(&ptm_sem);
  				struct timespec sleepEnd = latestSleep + TimeUtil::Micros(toff);
  				sem_post(&ptm_sem);

			// try sleep toff us
  				Statistics::addTrace(thread_type, id, sleep_start);
				if (sem_timedwait(&schedule_sem, &sleepEnd) == 0) // unblocked by the schedule signal
				{
				//cout<<"receives a schedule signal, break from sleep\n";
					sleep_counter = -1;
					Statistics::addTrace(thread_type, id, sleep_end);
					continue; // if current job is sleeping when it receives a schedule signal, quit PTM to continue next PTM with new ton and toff
				}

			}
		
			if (ton >= 100){	
				sem_wait(&state_sem);
				latestSleep = TimeUtil::Millis(0);
				state = _active;
				sem_post(&state_sem);

			//Then be active for ton us, waste base time slice with 100 us length.
			//after consuming 100us, update the execution info if is handling any job.
				start = TimeUtil::convert_us(TimeUtil::getTime());
				total_exed = 0;
				bool stop = false;
			// Statistics::addTrace(thread_type, id, active_start);
				do //ton loop
				{
					if (sem_trywait(&schedule_sem) == 0)//successfully read a schedule singal, break immediately
					{
					//cout<<"receives a schedule signal, break from active\n";
					// Statistics::addTrace(thread_type, id, active_end);
						sleep_counter = -1;
						break;
					}
				// load.consume_us(base);
				// load.consume_us_rand(base);
				// if (current_job != NULL){
				// 	load.consume_us_benchmarks(base);
				// }else{
				// 	load.consume_us_idle(base);
				// }

					load.consume_us_benchmarks(base);
					end = TimeUtil::convert_us(TimeUtil::getTime());
					exedSlice = end - start;
					total_exed = total_exed + exedSlice;
					start = end;

					sem_wait(&job_sem);
					if (current_job != NULL){
						if (current_job->execute(exedSlice) == 1){
							finishedJob();
						}
					}
					else{
						current_job = popFrontJob();;
					}
					sem_post(&job_sem);

					sem_wait(&ptm_sem);
					if (total_exed > ton)
						stop = true;
					sem_post(&ptm_sem);
				} while ((!stop) && Pipeline::isSimulating() );	
				Statistics::addTrace(thread_type, id, active_end); 
			}
		++sleep_counter;
		}

}



  #if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker " << id << " exiting wrapper...\n";
	Semaphores::print_sem.post_sem();
  #endif
}

// 

void Worker::newJob(Job * j)
{	
	bool preempted = false;

	sem_wait(&job_sem);
	if ( current_job!=NULL && current_job->getAbsDeadline() > j->getAbsDeadline() ){
		insertJobToQueue(current_job);
		current_job = j;
		preempted = true;
	}
	sem_post(&job_sem);

	if (!preempted){
		insertJobToQueue(j);
	}	
	if (j->joinStage(stageId))
	{
		Semaphores::print_sem.wait_sem();
		cout << "Worker " << id << " add new job error...\n";
		Semaphores::print_sem.post_sem();
	}
}


void Worker::finishedJob()
{

	#if _DEBUG == 1
  	Semaphores::print_sem.wait_sem();
  	cout << "Worker: " << id << " is finishing a job: " << current_job->getId() << endl;
  	Semaphores::print_sem.post_sem();
  	#endif
   //	pipeline->finishedJob(current_job);

	if (next != NULL)
		next->newJob(current_job);	
	else{
		pipeline->finishedJob(current_job);
	}

	current_job = popFrontJob();
}


void Worker::setPipeline(Pipeline * p)
{
	pipeline = p;
}


void Worker::setPTM(unsigned long _ton, unsigned long _toff)
{
	if (_ton < 100 && _toff < 100 )
	{
		return;
	}
	sem_wait(&ptm_sem);
	ton = _ton;
	toff = _toff;
	sem_post(&schedule_sem);
	sem_post(&ptm_sem);

}


void Worker::insertJobToQueue(Job* job){
	unsigned long thisDeadline = job->getAbsDeadline();
	bool found = false;
	list<Job*>::iterator insert_it;

	sem_wait(&FIFO_sem);
	for (list<Job*>::iterator it = FIFO.begin(); it !=  FIFO.end(); it++)
	{
		if ((*it)->getAbsDeadline() <= thisDeadline){
			continue;
		}else{
			insert_it = it;
			found = true;
			break;
		}
	}
	if (found){
		FIFO.insert(insert_it, job);
	}else{
		FIFO.push_back(job);
	}
	
	sem_post(&FIFO_sem);
}

Job* Worker::popFrontJob(){
	Job* ret;
	sem_wait(&FIFO_sem);
	if (FIFO.size() < 1){
		ret = NULL;
	}else{
		ret = FIFO.front();
		FIFO.pop_front();
	}
	sem_post(&FIFO_sem);
	return ret;
}

/***************Cool Shaper Related*****************/

int Worker::hasTask(){
	int ret = 0;
	sem_wait(&job_sem);
	if(current_job != NULL){
		++ret;
	}
	sem_post(&job_sem);

	sem_wait(&FIFO_sem);
	ret += (int) FIFO.size();
	sem_post(&FIFO_sem);

	return ret;
}


void Worker::runTask(unsigned long Wunit){

	unsigned long exedSlice;
	unsigned long start;
	unsigned long end;
	unsigned long total_exed;

	
			//Then be active for ton us, waste base time slice with 100 us length.
			//after consuming 100us, update the execution info if is handling any job.
	start = TimeUtil::convert_us(TimeUtil::getTime());

	total_exed = 0;
	bool stop = false;
			// Statistics::addTrace(thread_type, id, active_start);
	do //ton loop
	{

		load.consume_us_benchmarks(base);
		end = TimeUtil::convert_us(TimeUtil::getTime());
		exedSlice = end - start;
		total_exed = total_exed + exedSlice;
		start = end;

		sem_wait(&job_sem);
		if (current_job != NULL){
			if (current_job->execute(exedSlice) == 1){
				finishedJob();
			}
		}
		
		if (current_job == NULL){
			current_job = popFrontJob();
			if (current_job == NULL){
				stop = true;
			}
			
		}
		
		sem_post(&job_sem);

		if (total_exed > Wunit){
			stop = true;
		}

	} while ((!stop) && Pipeline::isSimulating() );	


	coolshaper.decreaseLeakyBucketf(((double)total_exed)/1000);
	coolshaper.runShaper();
}

void Worker::waiting(){

	while(isInterruptValid){
		if (hasTask()){
			disableInterrupt();
			runTask( (unsigned long)(coolshaper.getWunit()*1000) );
		}
		load.consume_us_benchmarks(100);
	}

}

void Worker::idle(unsigned long tsleep){
	unsigned long time = TimeUtil::getTimeUSec();
	struct timespec sleepEnd = TimeUtil::Micros(time + tsleep);
	sem_timedwait(&schedule_sem, &sleepEnd);
	enableInterrupt();
	setNextAction(_waiting);
}


void Worker::enableInterrupt(){
	isInterruptValid = true;
}

void Worker::disableInterrupt(){
	isInterruptValid = false;
}


void Worker::setNextAction(enum _worker_next_action na){
	nextAction = na;
}

void Worker::setIdleLength(unsigned long tsleep){
	sleepLength = tsleep;
}


std::vector<unsigned long> Worker::getShapingExpense(){
	return coolshaper.getExpense();
}
