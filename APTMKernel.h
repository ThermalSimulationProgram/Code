#ifndef _APTMKERNEL_H
#define _APTMKERNEL_H 

#include <vector>

#include "structdef.h"
#include "rtc.h"
#include "Scheduler.h"
#include "AdaptiveKernel.h"

using namespace std;



class APTMKernel : public AdaptiveKernel{
private:
	double bcoef;
	
	thermalProp offlineData;

	vector<double> partTimes;
	int counter;
	int block;
	
public:
	APTMKernel(unsigned n, vector<double> wcets, 
		vector<double> _tbet, double _bcoef, enum _schedule_kernel kernel);

	~APTMKernel();
	
	void setOfflineData(thermalProp);
	
	void setBcoef(double);
	
	void calcAPTM(vector<double> &, vector<double>&,const pipeinfo&);

	void getPipelineInfo(pipeinfo& config);

	void getScheduleScheme(vector<double> &, vector<double>&, pipeinfo&);

	void aPTM(double extBound, const vector<int>& index,
		const vector<double> &tau0, const dynamicdata &d, vector<double>& tinvs, 
		vector<double>& tvlds);
	static void assignToffs(vector<double>& extlambda, double, const vector<int> &,
		const vector<double> &, const vector<vector<double>>&,
		const vector<vector<double>>&, const vector<int>&, double sumLambda);

	static vector<double> assignTons(double, const vector<double>&,
		const vector<linearSegs> &,
		vector<int> &);
};



#endif