#include "BWSKernel.h"

#include "Scratch.h"
#include "vectormath.h"
#include "Scheduler.h"
#include "Pipeline.h"
#include "Parser.h"

#define MYINFINITY (90000000)

BWSKernel::BWSKernel(unsigned _nstages, vector<double> _wcets,
	vector<double> _tbet, enum _schedule_kernel kernel):
AdaptiveKernel(_nstages, _wcets, _tbet, kernel){
	
}

void BWSKernel::getScheduleScheme(vector<double> & tons, 
	vector<double>& toffs, pipeinfo& config){

	getPipelineInfo(config);

	calcBWS(tons, toffs, config);
}

void BWSKernel::calcBWS(vector<double> & tons, 
	vector<double>& toffs, const pipeinfo& config){
	
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

		toffs = tau0 + taue;
	}else{
		toffs = vector<double>(nstages, 0);
		
	}
	tons = vector<double>(nstages, 10000);	
}



void BWSKernel::getPipelineInfo(pipeinfo& config){
	// string callfun = "BWSKernel::getPipelineInfo";
	int index      = (int)config.adaptionIndex;
	scheduler->getPipelineInfo(config,  wcets, TBET);
	config.dcs     = vector<double>(nstages, 0);
	config.rho     = vector<double>(nstages, 0);
	jobject alpha_f;
	jobject alpha_d;
	
	double bmax = 0;
	for (int i = 0; i < (int)nstages; ++i){
		double minDSC = minDSCs[i];
		double k      = minALLKs[i];
		alpha_f       = rtc::Curve(config.FIFOcurveData[i] );

		if (i == 0){
			alpha_d = rtc::plus(haAlpha[index], alpha_f );
			if (rtc::eq(alpha_d, curvezero ))
				 config.dcs[nstages-i-1] = std::numeric_limits<double>::infinity();
			bmax = rtc::minbdf_BSF(alpha_d, relativeDeadline, k);
		}
		else{
			alpha_d = alpha_f;
			if (rtc::eq(alpha_d, curvezero )){
				bmax = std::numeric_limits<double>::infinity();
			}else{
				bmax = rtc::minbdf_BSF(alpha_d, relativeDeadline, k);
			}
			
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
