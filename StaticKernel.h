#ifndef _SATICKERNEL_H
#define _SATICKERNEL_H 


#include "ScheduleKernel.h"

#include <vector>

using namespace std;


class StaticKernel: public ScheduleKernel{
protected:
	vector<double> tons;
	vector<double> toffs;

public:
	StaticKernel(unsigned n, enum _schedule_kernel kernel_type);

	void setStaticScheme(vector<double>, vector<double>);

	void getScheduleScheme(vector<double> &, vector<double>&, pipeinfo&);

};








#endif