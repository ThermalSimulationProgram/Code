#ifndef _ADAPTIVEKERNEL_H
#define _ADAPTIVEKERNEL_H 

#include <vector>

#include "ScheduleKernel.h"
#include "rtc.h"


typedef struct
{
	std::vector<int>   Q;
	std::vector<int> activeSet;
	std::vector<int> sleepSet;
	std::vector<double> ccs;
	std::vector<double> dcs;
	std::vector<double> rho;
	std::vector<double> K;
	std::vector<std::vector<double>> FIFOcurveData;
	std::vector<double> allT;
	int adaptionIndex;
} AdaptInfo;



class AdaptiveKernel: public ScheduleKernel{

protected:
	std::vector<double> wcets;

	std::vector<double> allk;

	std::vector<double> minDSCs;

	std::vector<double> minALLKs;

	std::vector<double> TBET;
	
	double relativeDeadline;

	jobject curvezero;

	std::vector<jobject> haAlpha;

	int adaptCounter;

	

public:
	AdaptiveKernel(unsigned n, std::vector<double> wcets, 
		std::vector<double> _tbet, enum _schedule_kernel kernel, 
		std::vector<unsigned long>& rl_scheduling_times);

	virtual ~AdaptiveKernel();

	void setHaAlpha(std::vector<jobject>, double);

	virtual void getScheme(std::vector<double> &, std::vector<double>&) = 0;

	// virtual void getAdaptScheme(std::vector<double> &, std::vector<double>&);

	static std::vector<double> reduceM(const std::vector<int> &, const std::vector<int> &, 
		std::vector<double> , const std::vector<double> &, unsigned);

	void getRawAdaptInfo(AdaptInfo&);

	// get the curve data required to construct the arrival curve of the jobs
	// in ith FIFO
	static std::vector<double> getFIFODemands(const double&, 
		const std::vector<double>&, const int&, 
		const double&, const double&, enum _schedule_kernel kernel);
	
};


void pipeinfo_print(const AdaptInfo &);











#endif

