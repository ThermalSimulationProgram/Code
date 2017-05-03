#ifndef _APTMKERNEL_H
#define _APTMKERNEL_H 

#include <vector>

#include "structdef.h"
#include "AdaptiveKernel.h"



typedef struct dynamicinfo
{
	std::vector<double> rho;
	std::vector<std::vector<double>> breakToffs;
	std::vector<std::vector<double>> slopes;
	std::vector<int> numValidData;
	std::vector<linearSegs> warmingcurves;
	std::vector<double> K;
	
}dynamicdata;


class APTMKernel : public AdaptiveKernel{
private:
	double bcoef;
	
	thermalProp offlineData;

	std::vector<double> partTimes;
	int counter;
	int block;
	
public:
	APTMKernel(unsigned n, const std::vector<double>& wcets, 
		const std::vector<double>& _tbet, double _bcoef, enum _schedule_kernel kernel,
		std::vector<unsigned long>& rl_scheduling_times);

	~APTMKernel();
	
	void setOfflineData(thermalProp);
	
	void setBcoef(double);
	
	void calcAPTM(std::vector<double> &, std::vector<double>&, const AdaptInfo&);

	void getApdatInfo(AdaptInfo& config);

	int getScheme(std::vector<double> &, std::vector<double>&);

	void aPTM(double extBound, const std::vector<int>& index,
		const std::vector<double> &tau0, const dynamicdata &d, std::vector<double>& tinvs, 
		std::vector<double>& tvlds);

	static void assignToffs(std::vector<double>& extlambda, double, const std::vector<int> &,
		const std::vector<double> &, const std::vector<std::vector<double>>&,
		const std::vector<std::vector<double>>&, const std::vector<int>&, double sumLambda);

	static std::vector<double> assignTons(double, const std::vector<double>&,
		const std::vector<linearSegs> &,
		std::vector<int> &);
};



#endif