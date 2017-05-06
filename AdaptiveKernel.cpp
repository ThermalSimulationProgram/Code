#include "AdaptiveKernel.h"

#include "Scratch.h"
#include "vectormath.h"
#include "Pipeline.h"
#include "structdef.h"
#include "Enumerations.h"
#include "ScheduleKernelAPI.h"
#include "utils.h"

#include <iostream>


using namespace std;

AdaptiveKernel::AdaptiveKernel(unsigned n, vector<double> _wcets, 
		vector<double> _tbet, enum _schedule_kernel kernel,
		vector<unsigned long>& rl_scheduling_times):
ScheduleKernel(n, kernel){
	rtc::initialize();
	vector<double> threezeros(3,0);
	curvezero    = rtc::Curve(threezeros);
	wcets        = _wcets;
	allk         = 1/wcets;
	TBET         = _tbet;
	adaptCounter = 0;
	

	for (unsigned i = 0; i < wcets.size(); ++i){
		minDSCs.push_back(vecsum(wcets, i, nstages-1));
		minALLKs.push_back(minElement(allk, i, nstages-1));
	}


	// calculate history aware arrival curves statically 
	
	
	long period                         = (long)Scratch::getPeriod()/1000;
	double jitter                       = (double)Scratch::getJitter()/1000;
	long delay                          = (long)Scratch::getDistance()/1000;
	vector<double> rl_job_arrivals      = Scratch::getArrivalTimes_ms();
	vector<double> tmp_scheduling_times = rl_scheduling_times/1000;
		
	haAlpha = rtc::staticHistoryAwareArrialCurves(rl_job_arrivals,
		tmp_scheduling_times, period, jitter, delay);

	
	relativeDeadline = Scratch::getRltDeadline_ms();

	for (unsigned i = 0; i < haAlpha.size(); ++i){
		haAlpha[i] = rtc::affine(haAlpha[i], 1, relativeDeadline);
	}



	vector<vector<double> > all_rl_job_arrivals = Scratch::getAllArrivalTimes_ms();

	vector<double> allperiod 			= Scratch::getAllPeriod()/1000;
	vector<double> alljitter 			= Scratch::getAllJitter()/1000;
	vector<double> alldelay 			= Scratch::getAllDistance()/1000;
	vector<vector<double> > allwcets 			= Scratch::getAllDwcets();
	vector<double> allRelativeDeadlines = Scratch::getAllRltDeadline_ms();
	// for every task

	vector<vector<jobject>> all_multi_haAlpha;
	for (int i = 0; i < (int) all_rl_job_arrivals.size(); ++i)
	{
		vector<jobject> temp = rtc::staticHistoryAwareArrialCurves(all_rl_job_arrivals[i],
			tmp_scheduling_times, allperiod[i], alljitter[i], alldelay[i]);

		for (int j = 0; j < (int)temp.size(); ++j)
		{
			temp[i] = rtc::affine(temp[i], 1, allRelativeDeadlines[i]);
			temp[i] = rtc::times(temp[i], allwcets[i][0]);
		}

		all_multi_haAlpha.push_back(temp);
	}

	for (int i = 0; i < (int) tmp_scheduling_times.size(); ++i)
	{
		jobject allAlpha = all_multi_haAlpha[0][i];
		for (int j = 1; j < (int) all_multi_haAlpha.size(); ++j)
		{
			allAlpha = rtc::plus(allAlpha, all_multi_haAlpha[j][i]);
		}
		multi_haAlpha.push_back(allAlpha);
		multi_haAlpha_curve_data.push_back( rtc::segementsData(allAlpha, 3000) );
	}


	cout << "AdaptiveKernel: finish calculate history arrival curve" << endl;
}

AdaptiveKernel::~AdaptiveKernel(){
	
}





void AdaptiveKernel::setHaAlpha(vector<jobject> _haalpha, double rldeadline){
	relativeDeadline = rldeadline;
	haAlpha = _haalpha;
	// string callfun = "AdaptiveKernel::setHaAlpha";
	for (unsigned i = 0; i < haAlpha.size(); ++i){
		haAlpha[i] = rtc::affine(haAlpha[i], 1, relativeDeadline);
	}
}

vector<double> AdaptiveKernel::reduceM(const vector<int>& activeSet, 
	const vector<int>& sleepSet, vector<double> lambda, 
	const vector<double>& tau, unsigned m){
	unsigned ns = sleepSet.size();
	if (ns == 0)
		return lambda;

	unsigned na = activeSet.size();
	
	vector<double> B(na, 0);

	vector<double> fulltau(m, 0);
	
	vector<unsigned> isActiveStage(m, 1);
	
	for (unsigned i = 0; i < ns; ++i){
		fulltau[sleepSet[i]] = tau[i];
		isActiveStage[sleepSet[i]] = 0;
	}
	std::reverse(lambda.begin(), lambda.end());

	vector<double> fullright(m, 0);
	vector<unsigned> leftvarnum(m, 0);

	for (unsigned i = 0; i < m; ++i){
		fullright[i] = (lambda[i] - vecsum(fulltau, i, m-1));
		leftvarnum[i] = (vecsum(isActiveStage, i, m-1));
	}

	unsigned B_i = 1;
	while (B_i <= na){
		B[B_i-1] = maxORminCore(fullright, 
			equalIndex(leftvarnum, B_i), false);
		B_i++;
	}
	return B;
} 


void AdaptiveKernel::getRawAdaptInfo(AdaptInfo& adaptinfo){
	
	PipelineInfo pinfo;
	scheduleAPI->getPipelineInfo(pinfo);

	adaptinfo.Q.clear();
	adaptinfo.activeSet.clear();
	adaptinfo.sleepSet.clear();
	adaptinfo.ccs.clear();
	adaptinfo.FIFOcurveData.clear();
	adaptinfo.adaptionIndex = adaptCounter;

	// get current temperature
	adaptinfo.allT = pinfo.temperature;

	double curTime = pinfo.currentTime;
	for (int i = 0; i < pinfo.numstages; ++i)
	{
		WorkerInfo & tmp = pinfo.workerinfos[i];
		// number of jobs in the ith FIFO  
		adaptinfo.Q.push_back(tmp.nFIFOJobs);
		// if the ith stage is sleeping or working
		if (tmp.state == _sleep){
			adaptinfo.sleepSet.push_back(tmp.stageId);
			if (TBET[i] > tmp.sleepTime)
				adaptinfo.ccs.push_back(TBET[i] - tmp.sleepTime);
			else
				adaptinfo.ccs.push_back(0);
		}
		else
			adaptinfo.activeSet.push_back(tmp.stageId);

		// get the curve data required to construct the arrival curve of the jobs
		// in ith FIFO
		adaptinfo.FIFOcurveData.push_back(getFIFODemands(curTime,
			tmp.allEventAbsDeadlines, tmp.onGoEventId, tmp.executed,
			wcets[i], kernel_type));
	}

	adaptCounter++;
}


// get the curve data required to construct the arrival curve of the jobs
// in ith FIFO
vector<double> AdaptiveKernel::getFIFODemands(
	const double& curTime, 
	const vector<double>& allDeadlines, 
	const int& onGoEventId, 
	const double& abet, 
	const double& wcet,
	enum _schedule_kernel kernel){

	// unsigned Q = allDeadlines.size();
	vector<double> curvedata = {0.0, 0.0, 0.0};
	// cout << "curTime" << curTime << endl;
	double timeToDeadline0 = 0;
	double timeToDeadline;
	unsigned startId = 0;
	if (onGoEventId != 0){
		// cout << "Pipeline::getFIFODemands:has workload "<<endl;
		timeToDeadline = allDeadlines[0] - curTime;
		if (timeToDeadline  < timeToDeadline0){
			// cout << "timeToDeadline:  " << timeToDeadline << endl;
			// cout << "timeToDeadline0:  " << timeToDeadline0 << endl;
			// cout << "Pipeline::getFIFODemands: Warning: Current event deadline miss happened!"<<endl;
			// exit(1);
			curvedata[1] = 1 ;
		}else{
			if (abs(timeToDeadline - timeToDeadline0)<0.000001){
				curvedata[1] = curvedata[1] + 1;
				// cout << "Pipeline::getFIFODemands:add 1 "<<endl;
			}

			else{
				double remained;
				if (kernel == APTM){
					remained = (wcet - abet) / wcet;
				}else {
					remained = 1;
				}
				
				curvedata.push_back(timeToDeadline);
				curvedata.push_back(curvedata[1] + remained);
				curvedata.push_back(0);
				// cout << "Pipeline::getFIFODemands:update "<<endl;
				timeToDeadline0 = timeToDeadline;
			}	
		}
		startId ++;
	}

	// cout << "startId   "<<startId<<endl;
	for (unsigned i = startId; i < allDeadlines.size(); ++i){
		timeToDeadline = allDeadlines[i] - curTime;
		if (timeToDeadline  < timeToDeadline0){
			cout << "timeToDeadline:  " << timeToDeadline << endl;
			cout << "timeToDeadline0:  " << timeToDeadline0 << endl;
			cout << "Pipeline::getFIFODemands: Error: incorrect order of jobs in FIFO"<<endl;
			// exit(1);
		}else{
			unsigned numel = curvedata.size();
			if (abs(timeToDeadline - timeToDeadline0) < 0.000001){
				curvedata[numel-2] = curvedata[numel-2] + 1;
				// cout << "Pipeline::getFIFODemands:add 1 "<<endl;
			}
			else{
				curvedata.push_back(timeToDeadline);
				curvedata.push_back(curvedata[numel-2] + 1);
				curvedata.push_back(0);
				// cout << "Pipeline::getFIFODemands:update "<<endl;
				timeToDeadline0 = timeToDeadline;
			}
		}
	}
	return curvedata;
}


void pipeinfo_print(const AdaptInfo & config){
	cout << "pipeinfo.adaptionIndex:  " <<config.adaptionIndex << endl;
	displayvector(config.Q, "pipeinfo.Q");
	displayvector(config.activeSet, "pipeinfo.activeSet");
	displayvector(config.sleepSet, "pipeinfo.sleepSet");
	displayvector(config.ccs, "pipeinfo.ccs");
	displayvector(config.dcs, "pipeinfo.dcs");
	displayvector(config.rho, "pipeinfo.rho");
	displayvector(config.K, "pipeinfo.K");
	displayvector(config.allT, "pipeinfo.allT");

	for (unsigned i = 0; i < config.FIFOcurveData.size(); ++i){
		stringstream field;
		field << "pipeinfo.FIFOcurveData" << i+1;
		displayvector(config.FIFOcurveData[i], field.str());
	}
	
}
