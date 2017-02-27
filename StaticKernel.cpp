#include "StaticKernel.h"



StaticKernel::StaticKernel(unsigned n, enum _schedule_kernel kernel_type)
:ScheduleKernel(n, kernel_type){

}

void StaticKernel::setStaticScheme(vector<double> _tons, 
	vector<double> _toffs){
	tons = _tons;
	toffs = _toffs;
}


void StaticKernel::getScheduleScheme(vector<double>& tonsin, 
	vector<double>& toffsin, pipeinfo& config){
	tonsin = tons;
	toffsin = toffs;
}