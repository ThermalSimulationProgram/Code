#ifndef _WARMING_H
#define _WARMING_H

#include <string>
#include <vector>

using namespace std;

void getWarmingCurve(unsigned);

void getCoolingCurve(unsigned, vector<unsigned long> wcets);

void getOneStageCurve(const vector<unsigned long> &, const vector<unsigned long> &, 
	const unsigned, const string, const string);





#endif