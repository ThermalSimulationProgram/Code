#include "BWSKernel.h"

#include "vectormath.h"
#include "structdef.h"
#include "Statistics.h"


using namespace std;

#define _PROFILE 0

BWSKernel::BWSKernel(unsigned _nstages, const vector<double>& _wcets,
	const vector<double>& _tbet, enum _schedule_kernel kernel,
	vector<unsigned long>& rl_scheduling_times):
AdaptiveKernel(_nstages, _wcets, _tbet, kernel, rl_scheduling_times){
	
}

BWSKernel::~BWSKernel(){
	
}

int BWSKernel::getScheme(vector<double> & tons, vector<double>& toffs){

	unsigned long part1_timein = Statistics::getRelativeTime();

	AdaptInfo config;
	#if _PROFILE == 1
	vector<double> partTimes = vector<double> (10, 0);
	double timein = (double)Statistics::getRelativeTime_ms();
	#endif

	getAdaptInfo(config);

	unsigned long part1_time = Statistics::getRelativeTime() - part1_timein;

	#if _PROFILE == 1
	double timeout = (double)Statistics::getRelativeTime_ms();
	double time1 = timeout - timein;
	#endif

	#if _PROFILE == 1
	timein = (double)Statistics::getRelativeTime_ms();
	#endif
	calcBWS(tons, toffs, config);
	#if _PROFILE == 1
	timeout = (double)Statistics::getRelativeTime_ms();
	double time2 = timeout - timein;
	if (time1 + time2 > 2){
		partTimes[0] += time1;
		partTimes[1] += time2;
		cout << "getScheduleScheme:time1:  "<<partTimes[0] << 
		"   getScheduleScheme:time2:  " << partTimes[1] << endl;
	}
	#endif

	return (int) part1_time;

}

void BWSKernel::calcBWS(vector<double> & tons, 
	vector<double>& toffs, const AdaptInfo& config){
	
	int nactiveStage 		= config.activeSet.size();
	vector<int> Q1_Active 	= vectorExtract(config.Q,
		config.activeSet);


	vector<double> tau0 (nstages, (double)0);
	vector<double> taue = tau0;
	

	vectorAssign(tau0, config.sleepSet, config.ccs);
	vector<double> lambda_bar = reduceM(config.activeSet,
		config.sleepSet, config.dcs, config.ccs, nstages);

	vector<double> phi;
	if(lambda_bar.size()>0){
		vector<double> partTBET   = vectorExtract(TBET, config.activeSet);
		vector<double> divideTBET = lambda_bar/ partTBET;
		vector<double> tmpnum     = vecFloor(divideTBET);

		 phi = minElement( tmpnum, integerVector(1, nactiveStage) );
	}

	vector<int> tempActiveStages = config.activeSet;
	int nactiveStage1            = nactiveStage;
	vector<int> resultActiveSet;
	vector<int> resultSleepSet   = config.sleepSet;

	while(nactiveStage1 > 0){
		int nstage2minphi;
		double minphi = minElement(phi, &nstage2minphi);

		vector<int> stageIndex2minphi 	= integerVector(
			nactiveStage1 - nstage2minphi - 1, nactiveStage1-1);
		vector<int> tempsetIndex 		= vectorExtract(tempActiveStages, 
			stageIndex2minphi);

		if (minphi > 0){
			vector<int> Q0 		= vectorExtract(Q1_Active, 
				stageIndex2minphi);

			vector<int> index 	= sort_indexes(Q0);

			if (nstage2minphi + 1 > (int)minphi){
				vector<int> partIndex 	= vectorExtract(
					index, integerVector((int)minphi, index.size()-1));
				vector<int> aptmSet 	= vectorExtract(
					tempsetIndex, partIndex);
				resultActiveSet.insert(resultActiveSet.end(), 
					aptmSet.begin(), aptmSet.end());
			}

			vector<int> partIndex 	= vectorExtract(
				index, integerVector(0, (int)minphi-1));
			vector<int> tmpsleepset = vectorExtract(
				tempsetIndex, partIndex);
			resultSleepSet.insert(resultSleepSet.end(),
				tmpsleepset.begin(), tmpsleepset.end());

			vectorAssign(tau0, tmpsleepset, 
				vectorExtract(TBET, tmpsleepset));

		}else{
			resultSleepSet.insert(resultSleepSet.end(),
				tempsetIndex.begin(), tempsetIndex.end());
		}



		vector<double> newphi     = vectorExtract(phi, 
		integerVector(nstage2minphi+1, phi.size()-1));

		phi                       = newphi;
		nactiveStage1             = nactiveStage1 - nstage2minphi - 1;
		vector<int> newtempactive = vectorExtract(tempActiveStages, 
		integerVector(0, nactiveStage1-1));

		tempActiveStages          = newtempactive;
		if (phi.empty())
			break;
		phi = phi - minphi;
		phi = minElement(phi, integerVector(1, nactiveStage1));
		
	}

	int ASS_N = resultActiveSet.size();
	int SSS_N = resultSleepSet.size();
	std::sort(resultActiveSet.begin(), resultActiveSet.end());
	std::sort(resultSleepSet.begin(), resultSleepSet.end());
	if(SSS_N>0){
		int ASS_N1       = ASS_N;
		int SSS_N1       = SSS_N;
		vector<int> SSS1 = resultSleepSet;
		vector<int> ASS1 = resultActiveSet;

		vector<double> lambda_bar_e = reduceM(SSS1, ASS1, config.dcs, 
			vector<double>(ASS_N1, 0), nstages);
		vector<double> parttau0     = vectorExtract(tau0, SSS1);
		vector<double> sumparttau0;
		for (int i = 0; i < SSS_N1; ++i){
			sumparttau0.push_back(vecsum(parttau0, 0, i));
		}

		lambda_bar_e = lambda_bar_e - sumparttau0;

		while( SSS_N1 > 0){
			int lambda_index;
			double minlambda         = minElement(lambda_bar_e, &lambda_index);
			vector<int> tempsetIndex = vectorExtract(
				SSS1, integerVector(SSS_N1 - lambda_index - 1, SSS_N1-1));
			
			vectorAssign(taue, tempsetIndex, minlambda/(lambda_index+1));

			vector<int> tmpids             = integerVector(lambda_index+1, SSS_N1-1);
			vector<double> newlambda_bar_e = vectorExtract(lambda_bar_e, tmpids) - minlambda;
			lambda_bar_e                   = newlambda_bar_e;
			SSS_N1                         = SSS_N1 - lambda_index - 1;
			vector<int> newSSS1            = vectorExtract(SSS1, integerVector(0, SSS_N1-1));
			SSS1                           = newSSS1;
		}

		toffs = tau0 + taue - 1;
	}else{
		toffs = vector<double>(nstages, 0);
		
	}
	for (int i = 0; i < (int) toffs.size(); ++i)
	{
		if (toffs[i] < 0){
			toffs[i] = 0;
		}
	}
	tons = vector<double>(nstages, 100000);	
}



void BWSKernel::getAdaptInfo(AdaptInfo& config){
	// string callfun = "BWSKernel::getPipelineInfo";
	// int index      = (int)config.adaptionIndex;
	// scheduler->getPipelineInfo(config,  wcets, TBET);

	getRawAdaptInfo(config);
	int index = config.adaptionIndex;

	config.dcs     = vector<double>(nstages, 0);
	config.rho     = vector<double>(nstages, 0);
	jobject alpha_f;
	jobject alpha_d;
	
	double bmax = 0;
	for (int i = 0; i < (int)nstages; ++i){
		double minDSC = minDSCs[i];
		double k      = minALLKs[i];
		

		if (i == 0){
			alpha_f       = rtc::Curve(config.FIFOcurveData[i] );
			alpha_d = rtc::plus(haAlpha[index], alpha_f );
			vector<double> alpha_d_data = rtc::segementsData(alpha_d, 10000);

			if (rtc::eqZero(alpha_d_data)){
				config.dcs[nstages-i-1] = std::numeric_limits<double>::infinity();
				bmax = std::numeric_limits<double>::infinity();
			}else{
				bmax = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, k);
			} 
			

			/*if (rtc::eq(alpha_d, curvezero )){
			 	config.dcs[nstages-i-1] = std::numeric_limits<double>::infinity();
			 	bmax = std::numeric_limits<double>::infinity();
			}else{
				bmax = rtc::minbdf_BSF(alpha_d, relativeDeadline, k);
			}*/
			
		}
		else{
			/*alpha_d = alpha_f;
			vector<double> alpha_d_data = rtc::segementsData(alpha_d, 10000);*/
			vector<double> alpha_d_data  = config.FIFOcurveData[i];

			if (rtc::eqZero(alpha_d_data)){
				bmax = std::numeric_limits<double>::infinity();
			}else{
				bmax = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, k);
			}

			/*if (rtc::eq(alpha_d, curvezero )){
			 	bmax = std::numeric_limits<double>::infinity();
			}else{
				bmax = rtc::minbdf_BSF(alpha_d, relativeDeadline, k);
			}*/
			
		}

		double interQ = 0;
		for (int j = i+1; j <(int)nstages; ++j ){
			interQ = interQ + config.Q[j]/minALLKs[j];
		}	
		if (!std::isinf(config.dcs[nstages-i-1])){
			double dcs0             = bmax - minDSC - interQ;
			config.dcs[nstages-i-1] = dcs0 < 0 ? 0 : dcs0;
		}
		config.K.push_back(config.rho[i]*wcets[i]);
	}

}
