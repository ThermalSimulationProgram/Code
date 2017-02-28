#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <vector>
#include <time.h>
#include <pthread.h>
#include <string>

#include "Enumerations.h"
#include "Job.h"
#include "warmingCurve.h"
#include "structdef.h"

using namespace std;

class Worker;
class Dispatcher;
class Scheduler;
class TempWatcher;


class Pipeline{
protected:
	// This attribute indicates if the Pipeline is initialized
	static bool initialized;

	// This attribute indicates if the Pipeline is simulating
	static bool simulating;

	// number of pipeline stages
	int n_stages;

	// number of cpu cores in the  processor
	int n_cpus;

	// This vector stores the cpu core ids corresponding to each stage
	vector<unsigned> worker_cpu;

	// This vector stores pointers to workers. Each worker represents 
	// a stage.
	vector<Worker*> workers;

	// Dispatcher release jobs to Pipeline
	Dispatcher 	*dispatcher;

	// Scheduler manages the execution of the workers
	Scheduler 	*scheduler;

	// TempWatcher periodically reads the temperatures of the cpu cores and 
	// records them
	TempWatcher *tempwatcher;

	// auxiliary variable to set main thread priority
	struct sched_param param;

public:

	// Constructor needs the xml file path
	Pipeline(string);

	~Pipeline();

	/************** Simulation interface functions ************/
	// prepare for simulation, initialize statistics, dispatcher, scheduler
	void initialize();

	// explicitly set the CPUs to which the workers are attached 
	void setWorkerCPU(vector<unsigned>);

	// Start the simulation, the duration is loaded from Scratch class
	int simulate();

	// join other threads, wait them to finish
	void join_all();

	unsigned getNCPU();



	/************** Simulation functions ************/
	// This function is called by the dispatcher to release a new job
	void newJob(Job*, long);

	// call this function to stop the simulation
	void endSimulation();

	// Interface function to get member 'initialized'
	static bool isInitialized();

	// Interface function to get member 'simulating'
	static bool isSimulating();

	// This function is called by the end stage to announce a job is finished
	void finishedJob(Job*, unsigned long);

	// This function is called by the scheduler to apply new schedule scheme to 
	// each stage
	void setPTMs(vector<unsigned long>, vector<unsigned long>);

	/************** Functions for APTM and BWS kernels ************/
	// This function collects the dynamic information of the pipeline
	// in the simulation, and save it in config		
	void getInfo(pipeinfo&, const vector<double>&, const vector<double>&, 
		enum _schedule_kernel kernel);

	// This function is used for debugging, not used in real program
	static void loadInfoFromFile(pipeinfo&, const vector<double>&, 
		const vector<double>&, enum _schedule_kernel kernel);

	// get the curve data required to construct the arrival curve of the jobs
	// in ith FIFO
	static vector<double> getFIFODemands(const double&, 
		const vector<double>&, const int&, 
		const double&, const double&, enum _schedule_kernel kernel);
};




#endif