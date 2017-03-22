#include "APTMKernel.h"

#include <iostream>


#include "vectormath.h"
#include "Semaphores.h"
#include "Statistics.h"
#include "utils.h"



using namespace std;

#define _DEBUG 0
#define _INFO 0
#define _PROFILE 0




APTMKernel::APTMKernel(unsigned _nstages, const vector<double>& _wcets,
	const vector<double>& _tbet, double _bcoef, enum _schedule_kernel kernel,
	vector<unsigned long>& rl_scheduling_times):
AdaptiveKernel(_nstages, _wcets, _tbet, kernel, rl_scheduling_times),
partTimes(10, 0){
	bcoef 	= _bcoef;
	//partTimes = vector<double>(10, 0);
	counter = 0;
	block = 20;
}

APTMKernel::~APTMKernel(){
	
}

void APTMKernel::setOfflineData(thermalProp t){
	offlineData = t;
}
void APTMKernel::setBcoef(double b){
	bcoef = b;
}


void APTMKernel::getScheme(vector<double> & tons, vector<double>& toffs){
	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "aptm kernel starts collecting info\n";
	Semaphores::print_sem.post_sem();
	#endif	

	#if _PROFILE == 1
	double timein = (double)Statistics::getRelativeTime_ms();
	#endif
	AdaptInfo config;
	getApdatInfo(config);
	#if _PROFILE == 1
	double timeout = (double)Statistics::getRelativeTime_ms();
	double time1 = timeout - timein;
	#endif
	// partTimes[0] += timeout - timein;

	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "aptm kernel starts calculating scheme\n";
	Semaphores::print_sem.post_sem();
	#endif

	#if _PROFILE == 1
	timein = (double)Statistics::getRelativeTime_ms();
	#endif
	calcAPTM(tons, toffs, config);
	#if _PROFILE == 1
	timeout = (double)Statistics::getRelativeTime_ms();
	double time2 = timeout - timein;
	if (time1 + time2 > 20){
		partTimes[0] += time1;
		partTimes[1] += time2;
		cout << "getScheduleScheme:time1:  "<<partTimes[0] << 
		"   getScheduleScheme:time2:  " << partTimes[1] << endl;
	}
	#endif
	
	

	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "aptm kernel finishes calculating scheme\n";
	Semaphores::print_sem.post_sem();
	#endif
}


void APTMKernel::calcAPTM(vector<double> & tonsin, vector<double>& toffsin, 
	const AdaptInfo& config){
	#if _DEBUG == 1
	cout << "Inside APTMKernel::calcAPTM \n";
	#endif
	int nactiveStage = (int) config.activeSet.size();
	// unsigned nsleepStage  = config.sleepSet.size();
	vector<double> activeSetT = vectorExtract(config.allT, config.activeSet);
	dynamicdata d;
	#if _DEBUG == 1
	cout << "APTMKernel::calcAPTM:starts allocating dynamicdata \n";
	#endif

	for (int i = 0; i < (int)nstages; ++i){
		d.warmingcurves.push_back(
			offlineData.allStageCurves[i].getCurve(config.K[i]));	
	}
	d.rho   = config.rho;
	d.K     = config.K;
	#if _DEBUG == 1
	cout << "APTMKernel::calcAPTM:finishs allocating dynamicdata \n";
	#endif

	vector<double> tau0 (nstages, (double)0);
	vector<double> taue = tau0;
	vector<double> tons = taue;

	vectorAssign(tau0, config.sleepSet, config.ccs);
	vector<double> lambda_bar = reduceM(config.activeSet,
		config.sleepSet, config.dcs, config.ccs, nstages);

	#if _DEBUG == 1
	cout << "APTMKernel::calcAPTM: after first reduceM\n";
	if (lambda_bar.size() != config.activeSet.size()){
		cerr << "APTMKernel::calcAPTM: wrong lambda_bar \n";
		exit(2);
	}
	#endif

	vector<double> phi;
	#if _DEBUG==1
	displayvector(lambda_bar,"lambda_bar");
	#endif
	if(lambda_bar.size()>0){
		vector<double> partTBET = vectorExtract(TBET, config.activeSet);
		#if _DEBUG==1
		displayvector(partTBET,"partTBET");
		
		if ((int)partTBET.size() != nactiveStage){
			cerr << "APTMKernel::calcAPTM: wrong tmpnum \n";
			exit(2);
		}
		#endif

		vector<double> divideTBET = lambda_bar/ partTBET;
		vector<double> tmpnum     = vecFloor(divideTBET);

		#if _DEBUG==1
		displayvector(divideTBET,"divideTBET");
		displayvector(tmpnum,"tmpnum");
		if ((int)tmpnum.size() != nactiveStage){
			cerr << "APTMKernel::calcAPTM: wrong tmpnum \n";
			exit(2);
		}
		#endif

		phi = minElement( tmpnum, integerVector(1, nactiveStage) );
	}

	vector<int> tempActiveStages = config.activeSet;
	int nactiveStage1            = nactiveStage;
	vector<int> resultActiveSet;
	resultActiveSet.clear();
	vector<int> resultSleepSet = config.sleepSet;

	

	while(nactiveStage1 > 0){
		#if _DEBUG==1
		cout << "nactiveStage1   " << nactiveStage1 << endl;
		displayvector(phi,"phi");
		#endif
		int nstage2minphi;
		double minphi = minElement(phi, &nstage2minphi);

		vector<int> stageIndex2minphi 	= integerVector(
			nactiveStage1 - nstage2minphi - 1, nactiveStage1-1);
		vector<int> tempsetIndex 		= vectorExtract(tempActiveStages, 
			stageIndex2minphi);

		if (minphi > 0){
			vector<double> T0 = vectorExtract(activeSetT, 
				stageIndex2minphi);

			vector<int> index = sort_indexes(T0);

			if (nstage2minphi + 1 > (int)minphi){
				vector<int> partIds = integerVector(
					(int)minphi, (int)(index.size())-1);


				vector<int> partIndex 	= vectorExtract(
					index, partIds);
				vector<int> aptmSet 	= vectorExtract(
					tempsetIndex, partIndex);
				resultActiveSet.insert(resultActiveSet.end(), 
					aptmSet.begin(), aptmSet.end());
			}

			vector<int> partIndex 	= vectorExtract(
				index, integerVector(0, ((int)minphi)-1));
			vector<int> tmpsleepset = vectorExtract(
				tempsetIndex, partIndex);
			resultSleepSet.insert(resultSleepSet.end(),
				tmpsleepset.begin(), tmpsleepset.end());

			vectorAssign(tau0, tmpsleepset, 
				vectorExtract(TBET, tmpsleepset));

		}else{
			resultActiveSet.insert(resultActiveSet.end(),
				tempsetIndex.begin(), tempsetIndex.end());
		}

		vector<double> newphi = vectorExtract(phi, 
			integerVector(nstage2minphi+1, (int)phi.size()-1));
		phi = newphi;
		nactiveStage1 = nactiveStage1 - nstage2minphi - 1;
		vector<int> newtempactive = vectorExtract(tempActiveStages, 
			integerVector(0, nactiveStage1-1));
		tempActiveStages = newtempactive;
		if (phi.empty())
			break;
		phi = phi - minphi;
		phi = minElement(phi, integerVector(1, nactiveStage1));
		
	}

	int ASS_N = resultActiveSet.size();
	int SSS_N = resultSleepSet.size();

	#if _DEBUG==1
	if (ASS_N + SSS_N != (int)nstages){
		pipeinfo_print(config);
		cerr << "APTMKernel::calcAPTM: wrong resultActiveSet \n";
		exit(2);
	}
	cout << "SSS_N  " << SSS_N  << endl;
	cout << "ASS_N   " << ASS_N << endl;
	#endif

	std::sort(resultActiveSet.begin(), resultActiveSet.end());
	std::sort(resultSleepSet.begin(), resultSleepSet.end());
	if(SSS_N>0){
		int ASS_N1 = ASS_N;
		int SSS_N1 = SSS_N;
		vector<int> SSS1 = resultSleepSet;
		vector<int> ASS1 = resultActiveSet;

		vector<double> lambda_bar_e = reduceM(SSS1, ASS1, config.dcs, 
			vector<double>(ASS_N1, 0), nstages);
		vector<double> parttau0 	= vectorExtract(tau0, SSS1);
		vector<double> sumparttau0;
		for (int i = 0; i < SSS_N1; ++i){
			sumparttau0.push_back(vecsum(parttau0, 0, i));
		}

		#if _DEBUG == 1
		if (lambda_bar_e.size() != sumparttau0.size()){
			displayvector(sumparttau0, "sumparttau0");
			displayvector(lambda_bar_e, "lambda_bar_e");
			displayvector(config.dcs, "config.dcs");
			displayvector(SSS1, "SSS1");
			displayvector(ASS1, "ASS1");
			cerr << "APTMKernel::calcAPTM: wrong lambda_bar_e or sumparttau0 \n";
			exit(2);
		}
		#endif

		lambda_bar_e = lambda_bar_e - sumparttau0;


		while( SSS_N1 > 0){
			#if _DEBUG==1
			cout << "SSS_N1  " << SSS_N1  << endl;
			cout << "ASS_N1   " << ASS_N1 << endl;
			displayvector(lambda_bar_e,"lambda_bar_e");
			#endif
			

			int lambda_index;
			double minlambda         = minElement(lambda_bar_e, &lambda_index);
			vector<int> tempsetIndex = vectorExtract(
				SSS1, integerVector(SSS_N1 - lambda_index - 1, SSS_N1-1));

			int ntmp = tempsetIndex.size();
			vector<double> tinvs(ntmp, (double)0);
			vector<double> tvlds(ntmp, (double)100);

			if (minlambda > 0){
				vector<int> tmpid = integerVector(0, offlineData.coolbreakToffs.size()-1);
				d.breakToffs      = vectorExtract(offlineData.coolbreakToffs,tmpid, tempsetIndex);
				d.slopes          = vectorExtract(offlineData.coolslopes,tmpid, tempsetIndex);
				d.numValidData    = vectorExtract(offlineData.numValidData,tempsetIndex);
				aPTM(minlambda, tempsetIndex, vectorExtract(tau0, tempsetIndex),
					d, tinvs, tvlds);
				#if _DEBUG==1
				displayvector(tvlds,"tvlds");
				displayvector(tinvs,"tinvs");
				#endif
			}
			
			vectorAssign(taue, tempsetIndex, tinvs);
			vectorAssign(tons, tempsetIndex, tvlds);
			#if _DEBUG==1
			displayvector(tvlds,"tvlds");
			displayvector(tinvs,"tinvs");
			#endif

			vector<int> tmpids = integerVector(lambda_index+1, SSS_N1-1);
			vector<double> newlambda_bar_e = vectorExtract(lambda_bar_e, tmpids) - minlambda;
			lambda_bar_e = newlambda_bar_e;
			SSS_N1 = SSS_N1 - lambda_index - 1;
			vector<int> newSSS1 = vectorExtract(SSS1, integerVector(0, SSS_N1-1));
			SSS1 = newSSS1;
		}

		#if _DEBUG == 1
		if (tau0.size() != taue.size()){
			cerr << "APTMKernel::calcAPTM: wrong tau0 and taue \n";
			exit(2);
		}
		#endif

		toffsin = tau0 + taue ;
		for(int i=0; i< (int)toffsin.size(); i++){
			if(toffsin[i]<0)
				toffsin[i] = 0;
		}
		tonsin = tons ;

		#if _DEBUG==1
		displayvector(tons,"tons");
		displayvector(toffsin,"toffsin");
		#endif


	}else{
		toffsin = vector<double>(nstages, 0);
		tonsin = vector<double>(nstages, 100);
	}
}





void APTMKernel::aPTM(double extBound, const vector<int>& index,
	const vector<double> &tau0, const dynamicdata &d, vector<double>& tinvs, 
	vector<double>& tvlds){
	vector<double> k                  = vectorExtract(d.K, index);
	vector<double> partwcets          = vectorExtract(wcets, index);
	vector<linearSegs> rewardfuncs    = vectorExtract(d.warmingcurves, index);
	double sumLambda 				  = vecsum(tau0);
	double upBound                    = extBound + sumLambda;
	vector<vector<double>> breakToffs = d.breakToffs;
	vector<vector<double>> slopes     = d.slopes;
	slopes                            = vecAbs(slopes);
	vector<int> numValidData          = d.numValidData;
	int numindex                      = index.size();
	vector<int> newnumValidData       = numValidData;

	if (vectorAny(k > 1) || vectorAny(k == 0)){
		cerr<<"APTM::APTM:wrong slopes input"<<endl;
		dumpMatrix(k);
		dumpMatrix(k>1);
		dumpMatrix(k==0);
		exit(1);
	}
	vector<double> basetinvs = partwcets * ((1-k)/k);
	double basesum           = vecsum(basetinvs);
	vector<int> N (partwcets.size(), 1);

	for (int i = 0; i < (int)numindex; ++i){
		for (int j = 0; j < (int)numValidData[i]; ++j){
			if (breakToffs[j][i] > basetinvs[i]){
				newnumValidData[i] = j + 1;
				vector<int> tmpIds = integerVector(j+1, breakToffs.size()-1);
				breakToffs[j][i] = basetinvs[i];
				vectorAssign(breakToffs, tmpIds , i, 0);
				vectorAssign(slopes, tmpIds, i, 0);
				break;
			}
		}
	}

	numValidData = newnumValidData;
	#if _DEBUG == 1
	if ((int)tau0.size() != numindex){
		cerr << "APTM::aPTM: wrong tau0 input\n";
		exit(1);
	}
	#endif

	vector<double> newtinvs;

	if (upBound < basesum){
		assignToffs(newtinvs, upBound,
			index, tau0, breakToffs, slopes, numValidData, sumLambda);
	}else{
		if ( abs(upBound-basesum) < 0.000000001)
			newtinvs = basetinvs;
		else{
			newtinvs = assignTons(upBound, 
				basetinvs,  rewardfuncs, N);
		}
	} 
	#if _DEBUG == 1
	if ((int)newtinvs.size() != numindex){
		cerr << "APTM::aPTM: wrong newtinvs calculated\n";
		exit(1);
	}
	#endif

	tinvs = newtinvs - tau0 + 1;
	tvlds = partwcets * N;
}

void APTMKernel::assignToffs(vector<double>& lambdaExt, double upBound, 
	const vector<int> &index, const vector<double>& tau0, 
	const vector<vector<double>>& breakToffs,
	const vector<vector<double>> &slopes, 
	const vector<int>& numValidData, double sumLambda){

	int nindex               = index.size();
	lambdaExt = tau0;

	if (nindex == 1){
		double tmp = maxElement(breakToffs);
		if (upBound < tmp)
			lambdaExt[0] = upBound;
		else
			lambdaExt[0] = tmp;
		lambdaExt = lambdaExt - tau0;
		return;
	}

	int nsegments          = breakToffs.size();
	vector<int> segementId = integerVector(0, nsegments*nindex-1, nsegments);
	#if _DEBUG == 1
	if (segementId.size() != numValidData.size() ){
		cerr << "APTM::assignToffs: wrong segementId calculated\n";
		exit(1);
	}
	#endif

	vector<int> segmentIdLim = (segementId + numValidData) - 1;
	for (int i = 0; i < nindex; ++i){
		unsigned rowid = 0;
		while((breakToffs[rowid][i] <= tau0[i]) &&
				(segementId[i] <= segmentIdLim[i])){
			segementId[i] = segementId[i] + 1;
			rowid++;
		}
	}
	#if _DEBUG == 1
	if (segementId.size() != segmentIdLim.size() ){
		cerr << "APTM::assignToffs: wrong segmentIdLim calculated\n";
		exit(1);
	}
	#endif
	vector<double> maxLambdaExt = vectorExtract(breakToffs, segmentIdLim);
	vector<bool> validId = vectorLess(lambdaExt, maxLambdaExt);
	vector<bool> validIdorg = validId;
	while (sumLambda < upBound){
		vector<double> nextBreakPoints = vectorExtract(breakToffs,
			segementId);

		vector<double> newbreakPoints = vectorExtract(nextBreakPoints, validId);
		nextBreakPoints               = newbreakPoints;
		vector<double> tmptmp         = vectorExtract(lambdaExt, validId);
		#if _DEBUG == 1
		if (nextBreakPoints.size() != tmptmp.size()){
			displayvector(tmptmp, "tmptmp");
			displayvector(nextBreakPoints, "nextBreakPoints");
			cerr << "APTM::assignToffs: wrong nextBreakPoints calculated\n";
			exit(1);
		}
		#endif
		vector<double> dist2nextBp  = nextBreakPoints - tmptmp;
		vector<double> currentSlope = vectorExtract(slopes, segementId);
		vector<double> newslopes    = vectorExtract(currentSlope, validId);
		
		if (vectorAny(newslopes == 0)){
			displayvector(slopes, "slopes");
			displayvector(validId, "validId");
			displayvector(lambdaExt, "lambdaExt");
			displayvector(segementId, "segementId");
			displayvector(segmentIdLim, "segmentIdLim");
			displayvector(breakToffs, "breakToffs");
			displayvector(maxLambdaExt, "maxLambdaExt");
			vector<bool> validId2 = vectorLess(lambdaExt, breakToffs[0]);
			displayvector(validId2, "validId2");
			displayvector(validIdorg, "validIdorg");
			displayvector(currentSlope, "currentSlope");
			cout << nindex << endl;
			cerr << "APTMKernel::assignToffs:currentSlope has zero element\n";
			exit(1);
		}
		currentSlope                = newslopes;
		vector<double> K            = vecsum(currentSlope) / currentSlope;
		vector<bool> tmpvalid       = vecAbs(dist2nextBp) > 0.001;
		
		if(tmpvalid.size()<1){
			displayvector(slopes, "slopes");
			displayvector(validId, "validId");
			displayvector(lambdaExt, "lambdaExt");
			displayvector(segementId, "segementId");
			displayvector(segmentIdLim, "segmentIdLim");
			displayvector(maxLambdaExt, "maxLambdaExt");
			displayvector(breakToffs, "breakToffs");
			displayvector(breakToffs[0], "breakToffs[0]");
			displayvector(currentSlope, "currentSlope");
			exit(1);
		}
		
		double validDist = minElement(
			vectorExtract(dist2nextBp, tmpvalid) * vectorExtract(K, tmpvalid));

		double diff2UB 				= upBound - sumLambda;

		if(validDist < diff2UB){
			vector<double> b            = (currentSlope/vecsum(currentSlope)) * validDist;
			vector<double> tmpLambda    = vectorExtract(lambdaExt, validId) + b;
			vectorAssign(lambdaExt, validId, tmpLambda) ;
			sumLambda                   = sumLambda + validDist;
			
			vector<bool> atBreakPointId = vecAbs(lambdaExt - vectorExtract(breakToffs, segementId)) < 0.001;
			
			vector<double> diff = maxLambdaExt - lambdaExt;
			
			vector<bool> feasible       = diff > 0.001;
			// vectorLess(lambdaExt, maxLambdaExt);
			
			vector<bool> changeId       = atBreakPointId & feasible;
			// vector<bool> toInfId        = atBreakPointId & vecNot(feasible);

			vectorAssign(segementId, changeId, 
				vectorExtract(segementId, changeId)+1);

			validId 					= feasible;
			// if (vectorAll(toInfId))
			// 	break;

		}
		else{
			vector<double> b         = (currentSlope/vecsum(currentSlope)) * diff2UB;
			vector<double> tmpLambda = vectorExtract(lambdaExt, validId) + b;
			vectorAssign(lambdaExt, validId, tmpLambda) ;
			break;
		}
	}
	// return lambdaExt;
}

vector<double> APTMKernel::assignTons(double upBound, 
	const vector<double> &ministep, const vector<linearSegs> & rewardfuncs ,
	vector<int> & N){

	vector<double> toffs = ministep;
	if(vectorAny(toffs <= 0.0000001)){
		cerr << "APTM::assignTons: wrong toffs\n";
		exit(1);
	}
	while(true){
		double sumtoff       = vecsum(toffs);
		double remained      = upBound - sumtoff;
		vector<int> changeId = vecFind( ministep <= remained );
		int numId            = changeId.size();
		if (numId == 0)
			break;

		vector<double> rewards;
		int toffid;
		for (int i = 0; i < (int) numId; ++i){
			toffid    = changeId[i];
			double v1 = rewardfuncs[toffid].getValue(toffs[toffid]);
			double v2 = rewardfuncs[toffid].getValue(toffs[toffid]+ministep[toffid]);
			rewards.push_back((v1-v2)/ministep[toffid]);
		}
		int id;
		maxElement(rewards, &id);
		toffid        = changeId[id];
		toffs[toffid] += ministep[toffid];
		N[toffid] ++;
	}
	return toffs;
}



void APTMKernel::getApdatInfo(AdaptInfo& config){
	// string callfun = "APTM::getPipelineInfo";
	

	#if _PROFILE == 1
	double timein = (double)Statistics::getRelativeTime_ms();
	#endif
	getRawAdaptInfo(config);
	int index = config.adaptionIndex;
	// scheduler->getPipelineInfo(config,  wcets, TBET);
	
	
	// Pipeline::getInfo2(config, wcets, TBET);
	#if _DEBUG==1
	pipeinfo_print(config);
	#endif

	config.dcs = vector<double>(nstages, 0);
	config.rho = vector<double>(nstages, 0);

	jobject alpha_f;
	jobject alpha_d;
	
	double bmax = 0;
	#if _PROFILE == 1
	double timeout  = (double)Statistics::getRelativeTime_ms();
	double time1    = timeout - timein;
	double timein2f = (double)Statistics::getRelativeTime_ms();
	double time21   = 0;
	double time22   = 0;
	double time23   = 0;
	double time211  = 0;
	double time212  = 0;
	double time213  = 0;
	#endif
	
	for (int i = 0; i < (int)nstages; ++i){
		double minDSC    = minDSCs[i];
		double k         = minALLKs[i];
		double maxPreRho = 0;
		if (i == 0){
			#if _PROFILE == 1
			timein = (double)Statistics::getRelativeTime_ms();
			#endif

			#if _DEBUG==1
			cout << "haAlpha[index]" << rtc::toString(haAlpha[index] );
			#endif
			alpha_f                     = rtc::Curve(config.FIFOcurveData[i] );
			alpha_d                     = rtc::plus(haAlpha[index], alpha_f );
			vector<double> alpha_d_data = rtc::segementsData(alpha_d, 2000);
			#if _PROFILE == 1
			timeout = (double)Statistics::getRelativeTime_ms();
			time211 = timeout - timein;
			#endif
			if (rtc::eqZero(alpha_d_data)){
				config.dcs[nstages-1] = std::numeric_limits<double>::infinity();
				config.rho[0] = 0;
			}else{
				#if _DEBUG==1
				cout << rtc::toString(alpha_f );
				cout << rtc::toString(alpha_d );
				#endif
				#if _PROFILE == 1
				timein = (double)Statistics::getRelativeTime_ms();
				#endif
				bmax = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, k);
				// double bmax1 = rtc::minbdf_BSF(alpha_d, relativeDeadline, k);
				// if (abs(bmax1-bmax) > 0.01){
				// 	cout << bmax << "    " << bmax1 << "    " << relativeDeadline
				// 	<< "    " << k <<endl;
				// 	cout << rtc::toString(alpha_d);
				// 	exit(1);
				// }
				#if _PROFILE == 1
				timeout = (double)Statistics::getRelativeTime_ms();
				time212 = timeout - timein;
				#endif

				bmax = minDSC + bcoef * (bmax - minDSC);

				#if _PROFILE == 1
				timein = (double)Statistics::getRelativeTime_ms();
				#endif
				config.rho[0] = rtc::minspeedbdfEDG(alpha_d_data, bmax);
				// double rho1 = rtc::minspeedbdfEDG(alpha_d, bmax);
				// if (abs(rho1-config.rho[0]) > 0.02){
				// 	cout << rho1 << "    " << config.rho[0] << "    " << bmax
				// 	<< endl;
				// 	cout << rtc::toString(alpha_d);
				// 	exit(1);
				// }
				#if _PROFILE == 1
				timeout = (double)Statistics::getRelativeTime_ms();
				time213 = timeout - timein;
				#endif
				#if _DEBUG==1
				cout << "config.rho[0]:"<<config.rho[0]<<endl;
				cout << "bmax:"<<bmax<<endl;
				cout << "k:"<<k<<endl;
				#endif
				config.rho[0] = (k < config.rho[0]) ? k : config.rho[0];

			}
			#if _PROFILE == 1
			timeout = (double)Statistics::getRelativeTime_ms();
			time21 = timeout - timein;
			#endif
		}
		else{
			#if _PROFILE == 1
			timein = (double)Statistics::getRelativeTime_ms();
			#endif
			maxPreRho = config.rho[i-1] > maxPreRho ? config.rho[i-1] : maxPreRho;
			// alpha_d   = alpha_f;
			vector<double> alpha_d_data = config.FIFOcurveData[i];
			double tmp = maxPreRho;
			// if (rtc::eq(alpha_d, curvezero )){
			if (rtc::eqZero(alpha_d_data)){
				config.dcs[nstages-i-1] = std::numeric_limits<double>::infinity();
				config.rho[i]           = tmp;
			}else{
				bmax = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, k);
				bmax = minDSC + bcoef * (bmax - minDSC);
				double minrho = rtc::minspeedbdfEDG(alpha_d_data, bmax);
				tmp           = minrho > tmp ? minrho : tmp;
				config.rho[i] = k < tmp ? k : tmp;
				bmax          = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, config.rho[i]);

				// double minrho = rtc::minspeedbdfEDG(alpha_d_data, minDSC);
				// tmp           = minrho > tmp ? minrho : tmp;
				// config.rho[i] = k < tmp ? k : tmp;
				// bmax          = rtc::minbdf_BSF(alpha_d_data, relativeDeadline, config.rho[i]);

			}
			#if _PROFILE == 1
			timeout = (double)Statistics::getRelativeTime_ms();
			time22 = time22 + timeout - timein;
			#endif
		}
		#if _PROFILE == 1
		timein = (double)Statistics::getRelativeTime_ms();
		#endif
		double interQ = 0;
		for (int j = i+1; j < (int)nstages; ++j ){
			interQ = interQ + config.Q[j]/minALLKs[j];
		}	
		if (!std::isinf(config.dcs[nstages-i-1])){
			double dcs0 = bmax - minDSC - interQ;
			config.dcs[nstages-i-1] = dcs0 < 0 ? 0 : dcs0;
		}
		config.K.push_back(config.rho[i]*wcets[i]);
		#if _PROFILE == 1
		timeout = (double)Statistics::getRelativeTime_ms();
		time23 = time23 + timeout - timein;
		#endif
	}
	#if _PROFILE == 1
	timeout = (double)Statistics::getRelativeTime_ms();
	double time2final = timeout - timein2f;

	if (time1 + time2final > 20){
		cout << "time1: "<< time1 << " time2final: " << time2final <<
		"  time21: "<< time21 << "  time22: " << time22 <<
		"   time23: " << time23 << "  time211: "<< time211 << "  time212: " << time212 <<
		"   time213: " << time213<<endl;
	}
	#endif
	#if _DEBUG==1
	pipeinfo_print(config);
	#endif
}
