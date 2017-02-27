#ifndef _TIMESIM_H
#define _TIMESIM_H

#include <semaphore.h>

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

private:

	static void waste_time_unit(unsigned long);

};




#endif