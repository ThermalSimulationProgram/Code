#include "SAPTMKernel.h"

#include <iostream>
#include <cmath>

#include "TimeUtil.h"
#include "vectormath.h"
#include "Semaphores.h"
#include "Statistics.h"
#include "Scratch.h"
#include "ScheduleKernelAPI.h"
#include "utils.h"

#define _DEBUG 0
#define _PROFILE1 1

using namespace std;


SAPTMKernel::SAPTMKernel(unsigned _nstages, const vector<double>& _wcets, const vector<double>& _tbet,
		std::vector<unsigned long>& rl_scheduling_times)
: AdaptiveKernel(_nstages, _wcets, _tbet, SAPTM, rl_scheduling_times),
partTimes(10, 0){

	if ( _nstages > 1){
		cerr << "SAPTMKernel works only for single core scenarios" << endl;
		exit(1);

	}

	tswon = 0.5;
	tswoff = 0.5;
	g = 6.3;
	epsilon = 0.1;
	vector<double> alldeadlines = Scratch::getAllRltDeadline_ms();

	minDeadline = minElement(alldeadlines);

}


int SAPTMKernel::getScheme(std::vector<double> & tons, std::vector<double>& toffs){
	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "saptm kernel starts collecting info\n";
	Semaphores::print_sem.post_sem();
	#endif	

	#if _PROFILE1 == 1
	double timein = (double)Statistics::getRelativeTime();
	#endif
	unsigned long part1_timein = Statistics::getRelativeTime();
	
	// jobject beta = getBeta();
	// vector<double> beta_data = rtc::segementsData(beta, 2000);
	vector<double> beta_data = getBetaCurveData();
	unsigned long part1_time = Statistics::getRelativeTime() - part1_timein;


	#if _PROFILE1 == 1
	double timeout = (double)Statistics::getRelativeTime();
	double time1 = timeout - timein;
	#endif
	// partTimes[0] += timeout - timein;

	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "saptm kernel starts calculating scheme\n";
	Semaphores::print_sem.post_sem();
	#endif

	#if _PROFILE1 == 1
	timein = (double)Statistics::getRelativeTime();
	#endif

	int c = GoldenSearch(beta_data, tons, toffs);

	#if _PROFILE1 == 1
	timeout = (double)Statistics::getRelativeTime();
	double time2 = timeout - timein;
	if (time1 + time2 > 200){
		partTimes[0] += time1;
		partTimes[1] += time2;
		cout << "time1:  "<< time1 << 
		"   time2:  " << time2 << " time2 loop counter: " << c << endl;
		cout << "alltime1:  "<<partTimes[0] << 
		"   alltime2:  " << partTimes[1] << endl;
	}
	#endif
	
	

	#if _DEBUG == 1
	Semaphores::print_sem.wait_sem();
	cout << "saptm kernel finishes calculating scheme\n";
	Semaphores::print_sem.post_sem();
	#endif

	// displayvector(tons, "tons");
	// displayvector(toffs, "toffs");

	return (int) part1_time;
}



int SAPTMKernel::GoldenSearch(vector<double> beta_data, std::vector<double> & tons, std::vector<double>& toffs){
	
	toffs.clear();
	tons.clear();
	int counter = 0;
	double toff_max = rtc::minbdf_BSF(beta_data, minDeadline, 1);
	double toff_min = tswoff + tswon;
	if (toff_max < toff_min){
		tons.push_back(100000);
		toffs.push_back(0);
		return counter;
	}

	double toff_test1 = toff_min + 0.382*(toff_max - toff_min);
	double toff_test2 = toff_min + 0.618*(toff_max - toff_min);
	double ton_test1, ton_test2;
	double lambda1 = calculate_lambda_based_on_toff(beta_data, toff_test1, ton_test1);
	double lambda2 = calculate_lambda_based_on_toff(beta_data, toff_test2, ton_test2);

	while( toff_max - toff_test1 > epsilon){
		if (lambda1 > lambda2){
			toff_min = toff_test1;
			toff_test1 = toff_test2;
			toff_test2 = toff_min + 0.618*(toff_max - toff_min);
			lambda1 = lambda2;
			lambda2 = calculate_lambda_based_on_toff(beta_data, toff_test2, ton_test2);
		}else{
			toff_max = toff_test2;
			toff_test2 = toff_test1;
			toff_test1 = toff_min + 0.382*(toff_max - toff_min);
			lambda2 = lambda1;
			lambda1 = calculate_lambda_based_on_toff(beta_data, toff_test1, ton_test1);
		}
		counter++;
	}

	if (lambda1 < lambda2){
		toffs.push_back(toff_test1);
		tons.push_back(ton_test1);

	}else{
		toffs.push_back(toff_test2);
		tons.push_back(ton_test2);
	}
	return counter;
}

jobject SAPTMKernel::getEDFHaAlpha(){
	#if _DEBUG == 1
	cout << "SAPTMKernel:: start getEDFHaAlpha" << endl;
	#endif
	jobject edfHaAlpha = multi_haAlpha[adaptCounter];

	// for (int i = 1; i < (int) multi_haAlpha.size(); ++i)
	// {
	// 	edfHaAlpha = rtc::plus(edfHaAlpha,   multi_haAlpha[i][adaptCounter] );
	// }
	++adaptCounter;
	return edfHaAlpha;

}

std::vector<double> SAPTMKernel::getEDFHaAlphaCurveData(){
	#if _DEBUG == 1
	cout << "SAPTMKernel:: start getEDFHaAlpha" << endl;
	#endif
	vector<double> edfHaAlpha_data = multi_haAlpha_curve_data[adaptCounter];
	++adaptCounter;
	return edfHaAlpha_data;
}

std::vector<double> SAPTMKernel::getBetaCurveData(){
	return rtc::plus(getFIFOCurveData(), getEDFHaAlphaCurveData());
}

jobject SAPTMKernel::getBeta(){
	#if _DEBUG == 1
	cout << "SAPTMKernel:: start getBeta" << endl;
	#endif
	
	vector<double> curvedata = getFIFOCurveData();
	#if _DEBUG == 1
	cout << "SAPTMKernel:: finish creating fifo_curve" << endl;
	#endif

	jobject fifo_curve = rtc::Curve(curvedata);

	jobject ret = rtc::plus(fifo_curve, getEDFHaAlpha());   

	return ret;
}




std::vector<double> SAPTMKernel::getFIFOCurveData(){
	vector<newWorkerInfo> config;
	// cout << "getBeta 1" << endl;
	scheduleAPI->getNewWorkerInfo(config);
	// cout << "getBeta 1.0" << endl;

	double now = (double) Statistics::getRelativeTime_ms() ;
	vector<double> curvedata = {0.0, 0.0, 0.0};
	double timeToDeadline0 = 0;

	// cout << "getBeta 2" << endl;
	for (int i = 0; i < (int)config[0].allEventAbsDeadlines.size(); ++i)
	{
		double timeToDeadline = (double)config[0].allEventAbsDeadlines[i] - now;
		if (timeToDeadline < 0 ){
			cerr << " deadline miss happen " << endl;
			exit(1);
		}
		if (timeToDeadline < timeToDeadline0){
			cout << " no increment " << endl;
			exit(1);
		}
		unsigned numel = curvedata.size();
		if (abs(timeToDeadline - timeToDeadline0)<0.000001){
			curvedata[numel-2] = curvedata[numel-2] + config[0].allEventLoads[i];
		}else{
			curvedata.push_back(timeToDeadline);
			curvedata.push_back(curvedata[numel-2] + config[0].allEventLoads[i]);
			curvedata.push_back(0);
			timeToDeadline0 = timeToDeadline;
		}
	}

	// displayvector(curvedata, "curvedata");

	if (curvedata.size()%3 != 0){
		cerr << "getBeta: wrong curvedata" << endl;
		exit(1);

	}

	return curvedata;
}



double SAPTMKernel::calculate_lambda_based_on_toff(const vector<double>& beta, double toff, double& ton){
	double rho = rtc::minspeedbdfEDG(beta, toff + tswon);
	if (rho<0 || rho >= 1){
		ton = 1000000;
		return 1.0;
	}
	ton = (toff * rho + tswon) / (1 - rho) ;
	double lambda =  ( 1 - exp(-g * (tswoff+ton)/1000 )  )   / 
				 	 ( 1 - exp(-g * (ton+toff)/1000 )    );
	return lambda;

}