#ifndef _TIMESIM_H
#define _TIMESIM_H

#include <unistd.h>
#include <stdlib.h>


#include "benchmarks.h"

void waste_time(unsigned);

class Load
{

public:
	
	// int _one_us_param_;
	// int _ten_us_param_;
	// int _hundred_us_param_;
	// int _one_ms_param_;

	static void consume_us(unsigned long);

	static void consume_ms(unsigned long);

	 unsigned long consume_us_benchmarks(unsigned long);

	 unsigned long consume_us_rand(unsigned long);

	 Load();
	 CPUStressor cpu_stressor;

private:

	MWC RND;

	static void waste_time_unit(unsigned long);

};




#endif