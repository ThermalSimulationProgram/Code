#include "Pipeline.h"


#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

//components
#include "temperature.h"
#include "Dispatcher.h"
#include "Scheduler.h"
#include "Worker.h"
#include "Job.h"

//parser
#include "Parser.h"
//utilities
#include "Scratch.h"
#include "TimeUtil.h"
#include "Semaphores.h"
#include "Priorities.h"
#include "Statistics.h"
#include "Operators.h"
#include "FileOperator.h"



using namespace std;
#define _DEBUG 0
#define _INFO 0


bool Pipeline::initialized = false;
bool Pipeline::simulating  = false;
sem_t Pipeline::init_sem;
sem_t Pipeline::running_sem;

// Constructor needs the xml file path
Pipeline::Pipeline(string xml_path):Pipeline(xml_path, 0){

}

// Constructor needs the xml file path
Pipeline::Pipeline(string xml_path, int isAppendSaveFile):cpuUsageRecorder()
{
	Parser* p = new Parser(xml_path);
	if (p->parseFile()!=0){
		cerr << "Pipeline::Pipeline: Cannot parse given xml file!\n";
		exit(1);
	}
	delete p;
	#if _DEBUG == 1
	cout << "finished parse file\n";
	#endif
	thread_num = 0;

	// compare stage number with CPU number
	int _n_stages = Scratch::getNstage();
	n_cpus        = (int)sysconf(_SC_NPROCESSORS_ONLN);
	if (_n_stages <= 0 || _n_stages > n_cpus){
		cerr << "Invalid stage number, should be a positive integer which is less than n_cpus";
		pthread_exit(0);
	}
	
	n_stages        = _n_stages;
	// create Dispatchers 
	vector<vector<unsigned long> > allArrivalTimes =  Scratch::getAllArrivalTimes();

	for (int i = 0; i < (int) allArrivalTimes.size(); ++i)
	{
		Dispatcher* tempdispatcher      = new Dispatcher(this, allArrivalTimes[i], 100+i);
		dispatchers.push_back(tempdispatcher);
		thread_num++;
	}
	
	scheduler       = new Scheduler(this, Scratch::getKernel(), 99);
	thread_num++;
	
	// create n_stages workers and attach it to default CPU
	for (int i = 0; i < n_stages; ++i){
		Worker *t = new Worker(i, i);
		workers.push_back(t);
		worker_cpu.push_back(i);
		thread_num++;		
	}

	// give main thread the highest priority
	param.sched_priority = Priorities::get_main_pr(); //highest priority
	if ((sched_setscheduler(0, SCHED_FIFO, &param) != 0)) {
		cout << "Run with root\n";
		pthread_exit(0);
	}
	tempwatcher 	= new TempWatcher(200000, Scratch::getName(), 98);
	thread_num++;

	_isAppendSaveFile = isAppendSaveFile;

	sem_init(&init_sem, 0,0 );
	sem_init(&running_sem, 0, 0);
	// initialize();	
}

Pipeline::~Pipeline(){
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		delete dispatchers[i];
	}
	delete scheduler;
	delete tempwatcher;

	for (int i = 0; i < (int) workers.size();  ++i)
		delete workers[i];
	
}


// explicitly set the CPUs to which the works are attached 
void Pipeline::setWorkerCPU(vector<unsigned> order){
	if(order.size()!= (unsigned)n_stages){
		cout<<"Pipeline::setWorkerCPU: invalid input\n";
		return;
	}
	for (unsigned i = 0; i < order.size(); ++i){
		if (order[i]<0 || order[i]>= (unsigned)n_cpus){
			cout<<"Pipeline::setWorkerCPU: invalid input\n";
			return;
		}
	}
	worker_cpu = order;
}

// prepare for simulation, initialize statistics, dispatcher, scheduler
void Pipeline::initialize(){
	Statistics::initialize();

		// cout << "trigger scheduler" << endl;
	scheduler->trigger();
	scheduler->activate();
//sleep(30);
	
	vector<vector<unsigned long> > allwcets = Scratch::getAllWcets();
	vector<unsigned long> allRltDeadline = Scratch::getAllRltDeadline();
	// cout << "trigger dispatcher" << endl;
	for (int i = 0; i < (int)dispatchers.size(); ++i)
	{
		dispatchers[i]->createJobs(allwcets[i], Scratch::getExeFactor(), allRltDeadline[i]);
		dispatchers[i]->trigger();
		
	}
	



	// cout << "trigger tempwatcher" << endl;
	tempwatcher->trigger();

	// cout << "trigger Worker" << endl;
	Worker *current;
	for (int i = 0; i < n_stages; ++i){
		current = workers[i];
		if (i < n_stages-1)
			current->setNext(workers[i+1]);
		else
			current->setNext(NULL);
		current->setPipeline(this);
	}

	// cout << "finish trigger Worker" << endl;
	// Main thread waits for all threads initialized, especially the scheduler
	Semaphores::rtcinit_sem.wait_sem();

	// cout << "existing initialize1" << endl;
	// all threads initialized
	initialized = true;
	for(int i=0; i<thread_num; ++i){
		sem_post(&init_sem);
	}

	// cout << "existing initialize2" << endl;
}

// Start the simulation, the duration is loaded from Scratch class
double Pipeline::simulate(){
	Worker *t;
	// activate all the threads and attach them to their CPUs.
	for (unsigned i = 0; i < workers.size(); ++i)
	{
		t  = workers[i];
		t->trigger();
		t->activate();
		t->setCPU(worker_cpu[i]);
	}
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->activate();
		dispatchers[i]->setCPU(n_cpus-1);
	}
	
	
	
	scheduler->setCPU(n_cpus - 1);

	tempwatcher->activate();
	if(workers.size()==1){
		tempwatcher->setCPU(n_cpus - worker_cpu[0] - 1);
	}
	else
		tempwatcher->setCPU(n_cpus - 1);
		
	// record start time and send it to TimedRunnable objects 
	struct timespec rem;
	struct timespec duration = TimeUtil::Micros(Scratch::getDuration());
	Statistics::enable();
	Statistics::start();

	unsigned long tstart_us = TimeUtil::convert_us(Statistics::getStart());

	scheduler->setAbsSchedulingTimes(tstart_us);
	
	tempwatcher->setReadingTimes(tstart_us);
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->setAbsReleaseTimes(tstart_us);
	}

	// simulation starts now
	cpuUsageRecorder.startLoggingCPU();
	simulating = true;
	for(int i=0; i<thread_num; ++i){
		sem_post(&running_sem);
	}
	// main thread sleeps for duration time length
	nanosleep(&duration, &rem);
	simulating = false;
	// join other threads, wait them to finish
	join_all();
	// return the average temperature 

	cpuUsageRecorder.endLoggingCPU();

	if (Scratch::isSaveFile()){

		string tempSaveName = Scratch::getName() + "_result.csv";

		vector<string> beginOfData = vector<string>(1, "111111111111111111111111111111");
		

		if (_isAppendSaveFile > 0){
			appendContentToFile(tempSaveName, beginOfData);
		}else{
			saveContentToNewFile(tempSaveName, beginOfData);
		}
		
		appendToFile(tempSaveName, tempwatcher->getMeanTemp());
		double maxTemp = tempwatcher->getMaxTemp();
		appendToFile(tempSaveName, vector<double>(1, maxTemp));

		double MeanMaxTemp = tempwatcher->getMeanMaxTemp();
		appendToFile(tempSaveName, vector<double>(1, MeanMaxTemp));

		if (Scratch::getKernel() != CS){
			appendToFile(tempSaveName, scheduler->getKernelTime());
		}
		

		float total_cpu_usage = cpuUsageRecorder.getUsage();
		appendToFile(tempSaveName, vector<float>(1, total_cpu_usage));

		
		if (Scratch::getKernel() == CS){
			appendToFile(tempSaveName, workers[0]->getShapingExpense());
		}else{
			appendToFile(tempSaveName, scheduler->getAllSchemes());
			appendToFile(tempSaveName, scheduler->getKernelTimeExpenseLog());
		}

		if (Scratch::isSaveTemperatureTrace){
			vector<string> beginOfTrace = vector<string>(1, "22222222");
			vector<string> endOfTrace = vector<string>(1, "44444444");

			appendContentToFile(tempSaveName, beginOfTrace);
			appendToFile(tempSaveName, tempwatcher->getAllTempTrace());
			appendContentToFile(tempSaveName, endOfTrace);
		}



		appendContentToFile(tempSaveName, Statistics::getAllMissedDeadline());

		vector<string> endOfData = vector<string>(1, "999999999999999999999999999999");
		appendContentToFile(tempSaveName, endOfData);
		// appendToFile(tempSaveName, scheduler->getAllSchemes());
	}
	
	return tempwatcher->getMeanMaxTemp();
}


void Pipeline::join_all() {
	Worker* t;
   	#if _DEBUG==1
  		cout << "Joining all threads...\n";
    #endif

	for( unsigned i=0;i<workers.size();i++) {
		t = workers[i];
		if(t!=NULL) {
			t->join();
		}
	}
	
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->join();
	}

	scheduler->join();
	tempwatcher->join();
    #if _INFO==1
  		cout << "Joined all!\n";
    #endif
}


void Pipeline::getAllPipelineInfo(PipelineInfo& pinfo){

	pinfo.workerinfos.clear();
	pinfo.temperature = tempwatcher->getCurTemp();
	pinfo.numstages   = n_stages;

	// get current time from simulation start, unit ms
	double curTime = (double)Statistics::getRelativeTime()/1000;

	pinfo.currentTime = curTime;

	for (int i = 0; i < n_stages; ++i){
		WorkerInfo tmp;
		workers[i]->getAllInfo(curTime, tmp);
		pinfo.workerinfos.push_back(tmp);
	}	
}



// This function is called by the dispatcher to release a new job
void Pipeline::newJob(Job* j, long accurate_time){
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "New job with id: " << j->getId() << " released at time: " 
	  << (Statistics::getRelativeTime_ms()) << " microsecond! The accurate time is " 
	  << accurate_time << endl;
	Semaphores::print_sem.post_sem();
	#endif

	// Pipeline simply forwards the job to first stage
	if (workers.front()!= NULL){
	  	workers.front()->newJob(j);
	  }
	
}

// call this function to stop the simulation
void Pipeline::endSimulation(){
	simulating = false;
}

// Interface function to get member 'initialized'
bool Pipeline::isInitialized(){
	return initialized;
}
// Interface function to get member 'simulating'
bool Pipeline::isSimulating(){
	return simulating;
}


// This function is called by the end stage to announce a job is finished
void Pipeline::finishedJob(Job* j){
	// let the job self-check if it is real finished

	double now = Statistics::getRelativeTime_ms();
	double deadline = (double)j->getRltDeadline()/1000;

	double release = ((double)j->getRltReleaseTime())/1000;

	// if (now > deadline + 1){
	// 	cout << release <<  " A deadline miss happened!" << endl;
	// 	Statistics::addMissedDeadline(j->getId(), (unsigned long)release, 
	// 		(unsigned long)deadline, (unsigned long)now);
	// }
	if (j->isFinalFinished()){
		Semaphores::print_sem.wait_sem();
		// double now = Statistics::getRelativeTime_ms();
		// double deadline = (double)j->getRltDeadline()/1000;
		cout << "Task: " << j->getTaskId() << " " << j->getId() <<  "th job finishes at time: " 
		<< now << " millisecond "  << "with deadline: " << deadline << ". ";

		if (now > deadline+1){
			cout << "A deadline miss happened!";
		}

		cout << endl;
		Semaphores::print_sem.post_sem();
	}
	else{
		Semaphores::print_sem.wait_sem();
		cout << "Task: " << j->getTaskId() << " " << j->getId() <<  "th job finishes incorrectly!" 
		<< "at time: "<< (Statistics::getRelativeTime_ms()) << " millisecond!"  << endl;
		Semaphores::print_sem.post_sem();
	}
}

// This function is called by the scheduler to apply new schedule scheme to each stage
void Pipeline::setPTMs(vector<unsigned long> ton, vector<unsigned long> toff){
	// simply forwards the ton and toff to each stage
	for (unsigned i = 0; i < workers.size(); ++i)
		workers[i]->setPTM(ton[i], toff[i]);
}


unsigned Pipeline::getNCPU(){
	return n_cpus;
}

void Pipeline::getAllNewWorkerInfo(std::vector<newWorkerInfo>& info){
	info.clear();
	for (unsigned i = 0; i < workers.size(); ++i)
	{
		newWorkerInfo ni;
		info.push_back(ni);
		workers[i]->getNewInfo(info[i]);		
	}
}



// // This function is used for debugging, not used in real program
// void Pipeline::loadInfoFromFile(pipeinfo& config, 
// const vector<double>& wcets, const vector<double>& TBET,
// enum _schedule_kernel kernel){
// 	config.allT = loadVectorFromFile<double>("allT.csv");
// 	double curTime = loadDoubleFromFile("CurTime.csv");
// 	unsigned ustages = wcets.size();
// 	vector<workerinfo> allinfo =  loadWorkerInfo(ustages);
// 	for (unsigned i = 0; i < ustages; ++i){
		
// 		workerinfo tmp = allinfo[i];
// 		config.Q.push_back(tmp.nFIFOJobs);
// 		if (tmp.state == _sleep){
// 			config.sleepSet.push_back(tmp.stageId);
// 			if (TBET[i] > tmp.sleepTime)
// 				config.ccs.push_back(TBET[i] - tmp.sleepTime);
// 			else
// 				config.ccs.push_back(0);
// 		}
// 		else
// 			config.activeSet.push_back(tmp.stageId);

// 		config.FIFOcurveData.push_back(getFIFODemands(curTime,
// 			tmp.allEventAbsDeadlines, tmp.onGoEventId, tmp.executed,
// 			wcets[i], kernel));
// 	}
	
// }



// // This function collects the dynamic information of the pipeline
// // in the simulation, and save it in config
// void Pipeline::getInfo(pipeinfo& config, 
// const vector<double>& wcets, const vector<double>& TBET, 
// enum _schedule_kernel kernel){
// 	// fill structure config with dynamic information of pipeline.
// 	// return config satisfying:
// 	// 1. config.Q, config.FIFOcurveData, config.allT have size == nstages
// 	// 2. size of activeSet + size of sleepSet == nstages
// 	// 3. size of sleepSet == ccs;

// 	config.Q.clear();
// 	config.activeSet.clear();
// 	config.sleepSet.clear();
// 	config.ccs.clear();
// 	config.FIFOcurveData.clear();

// 	// get current temperature
// 	config.allT = tempwatcher->getCurTemp();
// 	#if _DEBUG == 1
// 	cout << "got temperature\n";
// 	#endif

// 	// get current time from simulation start, unit ms
// 	double curTime = (double)Statistics::getRelativeTime()/1000;
	
// 	for (int i = 0; i < n_stages; ++i)
// 	{
// 		workerinfo tmp;
// 		#if _DEBUG == 1
// 		cout << "getting worker info: " << i << endl;
// 		#endif
// 		workers[i]->getAllInfo(curTime, tmp);
// 		#if _DEBUG == 1
// 		cout << "finish getting worker info: " << i << endl;
// 		#endif
// 		// number of jobs in the ith FIFO  
// 		config.Q.push_back(tmp.nFIFOJobs);
// 		// if the ith stage is sleeping or working
// 		if (tmp.state == _sleep){
// 			config.sleepSet.push_back(tmp.stageId);
// 			if (TBET[i] > tmp.sleepTime)
// 				config.ccs.push_back(TBET[i] - tmp.sleepTime);
// 			else
// 				config.ccs.push_back(0);
// 		}
// 		else
// 			config.activeSet.push_back(tmp.stageId);

// 		// get the curve data required to construct the arrival curve of the jobs
// 		// in ith FIFO
// 		config.FIFOcurveData.push_back(getFIFODemands(curTime,
// 			tmp.allEventAbsDeadlines, tmp.onGoEventId, tmp.executed,
// 			wcets[i], kernel));
// 	}

// 	#if _DEBUG == 1
// 	bool noerror = true;
// 	if ((int)config.Q.size() !=  n_stages)
// 		noerror = false;
// 	if ((int)config.FIFOcurveData.size()!= n_stages || 
// 		(int)config.allT.size() != n_stages)
// 		noerror = false;

// 	if ((int)config.activeSet.size() + (int)config.sleepSet.size() 
// 		!= n_stages)
// 		noerror = false;

// 	if (!noerror){
// 		cerr << "Pipeline::getInfo: error config\n";
// 		exit(2);
// 	}
// 	#endif
	
// }
