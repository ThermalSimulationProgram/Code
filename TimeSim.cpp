#include "TimeSim.h"
#include "TimeUtil.h"


#include <cmath>
#include <stdlib.h>
#include <iostream>

using namespace std;


void waste_time(unsigned _duration){
  double result = 0.0;
  int offset = rand();
  long duration = _duration * 10;
  for (long i = 0; i < duration; ++i)
    result = result + sin(i+offset)*tan(i+offset);
 
}

void Load::consume_us(unsigned long us){
	if (us == 0){
		return;
	}
	unsigned long start = 0, end = 0;
	start = TimeUtil::convert_us(TimeUtil::getTime());
	do{
		Load::waste_time_unit(10);
		end = TimeUtil::convert_us(TimeUtil::getTime());
	}while((end - start)<=us);
}


void Load::consume_ms(unsigned long ms){
	Load::consume_us(1000*ms);
}

void Load::waste_time_unit(unsigned long _duration){
	int offset = rand();
	double result = 0.0;
	for (unsigned long i = 0; i < _duration; ++i){
		result = result + sin(i+offset)*tan(i+offset);
	}

}

