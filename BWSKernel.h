#ifndef _BWSKERNEL_H
#define _BWSKERNEL_H 

#include <vector>

#include "structdef.h"
#include "rtc.h"
#include "AdaptiveKernel.h"

using namespace std;


class BWSKernel:public AdaptiveKernel{
private:
	

public:
	BWSKernel(unsigned n, vector<double> wcets, 
		vector<double> _tbet, enum _schedule_kernel kernel);

	~BWSKernel();
	
	void getScheduleScheme(vector<double> &, vector<double>&, pipeinfo&);
	
	void calcBWS(vector<double> &, vector<double>&, const pipeinfo&);

	void getPipelineInfo(pipeinfo& config);
};



#endif