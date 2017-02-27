#ifndef _ADAPTIVEKERNEL_H
#define _ADAPTIVEKERNEL_H 


#include "ScheduleKernel.h"


using namespace std;



class AdaptiveKernel: public ScheduleKernel{

protected:
	vector<double> wcets;

	vector<double> allk;

	vector<double> minDSCs;

	vector<double> minALLKs;

	vector<double> TBET;
	
	double relativeDeadline;

	jobject curvezero;

	vector<jobject> haAlpha;

	

public:
	AdaptiveKernel(unsigned n, vector<double> wcets, 
		vector<double> _tbet, enum _schedule_kernel kernel);


	void setHaAlpha(vector<jobject>, double);

	virtual void getScheduleScheme(vector<double> &, vector<double>&, pipeinfo&) = 0;

	static vector<double> reduceM(const vector<int> &, const vector<int> &, 
		vector<double> , const vector<double> &, unsigned);

	
};














#endif

