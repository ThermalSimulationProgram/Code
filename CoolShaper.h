#ifndef COOLSHAPER_H
#define COOLSHAPER_H 


#include <vector>

class Worker;

class CoolShaper{

private:
	double Wunit;
	std::vector<double> leakyBucket_b;
	std::vector<double> leakyBucket_r;
	std::vector<double> leakyBucket_f;

	unsigned long previousTime;

	Worker* worker;


	std::vector<unsigned long> timeExpenses;

public:
	CoolShaper(Worker* w);

	void runShaper();

	void init();

	void decreaseLeakyBucketf(double w);

	double getWunit();
};







#endif