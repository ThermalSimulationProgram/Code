#ifndef _BWSKERNEL_H
#define _BWSKERNEL_H 

#include <vector>

#include "AdaptiveKernel.h"


using namespace std;


class BWSKernel:public AdaptiveKernel{
private:
	

public:
	BWSKernel(unsigned n, std::vector<double> wcets, 
		std::vector<double> _tbet, enum _schedule_kernel kernel,
		std::vector<unsigned long>& rl_scheduling_times);

	~BWSKernel();
	
	void getScheme(std::vector<double> &, std::vector<double>&);
	
	void calcBWS(std::vector<double> &, std::vector<double>&, const AdaptInfo&);

	void getAdaptInfo(AdaptInfo& config);
};



#endif