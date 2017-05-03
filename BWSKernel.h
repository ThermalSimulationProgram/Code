#ifndef _BWSKERNEL_H
#define _BWSKERNEL_H 

#include <vector>

#include "AdaptiveKernel.h"





class BWSKernel:public AdaptiveKernel{
private:
	

public:
	BWSKernel(unsigned n, const std::vector<double>& wcets, 
		const std::vector<double>& _tbet, enum _schedule_kernel kernel,
		std::vector<unsigned long>& rl_scheduling_times);

	~BWSKernel();
	
	int getScheme(std::vector<double> &, std::vector<double>&);
	
	void calcBWS(std::vector<double> &, std::vector<double>&, const AdaptInfo&);

	void getAdaptInfo(AdaptInfo& config);
};



#endif