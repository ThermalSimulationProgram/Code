#ifndef SAPTMKERNEL_H
#define SAPTMKERNEL_H

#include <vector>
#include "structdef.h"
#include "AdaptiveKernel.h"


class SAPTMKernel: public AdaptiveKernel{

	double tswon;
	double tswoff;
	double g;
	double epsilon;
	double minDeadline;

	std::vector<double> partTimes;


public:
	SAPTMKernel(unsigned n, const std::vector<double>& wcets, 
		const std::vector<double>& _tbet,
		std::vector<unsigned long>& rl_scheduling_times);


	int getScheme(std::vector<double> & tons, std::vector<double>& toffs);

	jobject getBeta();

	jobject getEDFHaAlpha();

	int GoldenSearch(std::vector<double> beta_data, std::vector<double> & tons, std::vector<double>& toffs);

	double calculate_lambda_based_on_toff(const std::vector<double>& beta, double toff, double& ton);

};


#endif