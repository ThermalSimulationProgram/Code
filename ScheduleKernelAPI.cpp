#include "ScheduleKernelAPI.h"


#include "Scheduler.h"
#include "ScheduleKernel.h"
#include "Scratch.h"
#include "vectormath.h"
#include "Statistics.h"
#include "Pipeline.h"
#include "APTMKernel.h"
#include "SAPTMKernel.h"
#include "BWSKernel.h"
#include "StaticKernel.h"


using namespace std;
ScheduleKernelAPI::ScheduleKernelAPI(Scheduler* scheduler, 
	enum _schedule_kernel kernel_type,  std::vector<unsigned long>& rl_scheduling_times){
	_scheduler = scheduler;
	timeExpense.reserve(10000);
	allschemes.reserve(10000);
	// alltoffs.reserve(10000);

	unsigned nstages = Scratch::getNstage();
	// get the kernel for different kernel types
	switch (kernel_type){
		case APTM : {
		APTMKernel *tempkernel = new APTMKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), Scratch::getBFactor(), kernel_type,
				rl_scheduling_times);

			tempkernel->setOfflineData(Scratch::getOfflineData());
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case BWS: {
			BWSKernel *tempkernel = new BWSKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), kernel_type, rl_scheduling_times);
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case PBOO:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
			
		case GE:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			// tempkernel->setStaticScheme(vector<double>(nstages, 10000), vector<double>(nstages, 0));
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}

		case SAPTM:{
			SAPTMKernel *tempkernel = new SAPTMKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), rl_scheduling_times);
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}

	}

	_kernel->setScheduleAPI(this);
}


void ScheduleKernelAPI::runKernel(vector<double>& tons, 
	vector<double>& toffs){
	double part1_time = 0;
	double timein = (double)Statistics::getRelativeTime();
	part1_time = (double) _kernel->getScheme( tons,  toffs);
	double timeout = (double)Statistics::getRelativeTime();
	vector<double> tmp;
	tmp.insert(tmp.end(), tons.begin(), tons.end());
	tmp.insert(tmp.end(), toffs.begin(), toffs.end());
	allschemes.push_back(tmp);
	// alltoffs.push_back(toffs);
	vector<double> tmp2{part1_time, timeout - timein};
	
	timeExpense.push_back(tmp2);
}


double ScheduleKernelAPI::getMaxTimeExpense(){
	double maxtime = -1;
	for (int i = 0; i < (int)timeExpense.size(); ++i)
	{
		if (timeExpense[i][1] > maxtime){
			maxtime = timeExpense[i][1];
		}
	}
	return maxtime;
}

double ScheduleKernelAPI::getMeanTimeExpense(){
	double sum = 0;

	for (int i = 0; i < (int)timeExpense.size(); ++i)
	{
		sum += timeExpense[i][1];
	}

	double avg = sum/timeExpense.size();
	return avg;
}

void ScheduleKernelAPI::getPipelineInfo(PipelineInfo& pinfo){
	_scheduler->getPipelinePointer()->getAllPipelineInfo(pinfo);
}

void ScheduleKernelAPI::getNewWorkerInfo(std::vector<newWorkerInfo>& info){
	_scheduler->getPipelinePointer()->getAllNewWorkerInfo(info);
}

vector<vector<double> > ScheduleKernelAPI::getAllSchemes(){
	return allschemes;
}

std::vector<std::vector<double> > ScheduleKernelAPI::getKernelTimeExpenseLog(){
	return timeExpense;
}