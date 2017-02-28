#include "AdaptiveKernel.h"

#include "Scratch.h"
#include "rtc.h"
#include "vectormath.h"

AdaptiveKernel::AdaptiveKernel(unsigned n, vector<double> _wcets, 
		vector<double> _tbet, enum _schedule_kernel kernel):
ScheduleKernel(n, kernel){

	wcets    = _wcets;
	allk 	= 1/wcets;
	TBET     = _tbet;
	vector<double> threezeros(3,0);
	curvezero = rtc::Curve(threezeros);

	for (unsigned i = 0; i < wcets.size(); ++i){
		minDSCs.push_back(vecsum(wcets, i, nstages-1));
		minALLKs.push_back(minElement(allk, i, nstages-1));
	}
	
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




