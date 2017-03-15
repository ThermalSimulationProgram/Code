#ifndef _JOB_H
#define _JOB_H

#include <vector>
#include <semaphore.h>



class Job
{
protected:
	unsigned nstage;
	unsigned cstage;

	unsigned long releaseTime;
	unsigned long rltDeadline;
	unsigned long absDeadline;

	///This attribute indicates the real-cse execution time of each job, unit us 
	std::vector<unsigned long> rcet;
	std::vector<unsigned long> loads;
	//This variable stores the id of current job
	unsigned id;
	///This variable stores the time length for which current job has been executed, unit us 
	unsigned long abet;

	sem_t state_sem;

public:
	Job(unsigned, unsigned, unsigned long);

	void release(unsigned long );

	int joinStage(unsigned);

	int execute(unsigned long);

	bool isFinished();

	bool isFinalFinished();

	// set and get methods

	int setRCET(std::vector<unsigned long>, float);

	unsigned long getABET();

	unsigned getId();

	unsigned long getLoad();

	unsigned long getAbsDeadline();
};



#endif