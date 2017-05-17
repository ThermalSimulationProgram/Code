#include "CoolShaper.h"

#include "Scratch.h"
#include "vectormath.h"
#include "Pipeline.h"
#include "Worker.h"
#include "TimeUtil.h"
#include "utils.h"

using namespace std;

#define _DEBUG 0


CoolShaper::CoolShaper(Worker* _w){

	if (Scratch::getNstage() > 1){
		cerr << "CoolShaper is only for single core experiment" << endl;
		exit(1);
		
	}

	previousTime = 0;

	Wunit = Scratch::Wunit;
	leakyBucket_b = Scratch::leakyBucket_b;
	leakyBucket_r = Scratch::leakyBucket_r;

	leakyBucket_f = leakyBucket_b + Wunit;

	worker = _w;

	timeExpenses.reserve(10000);
}

void CoolShaper::init(){
	previousTime = TimeUtil::getTimeUSec(); 
}

void CoolShaper::runShaper(){
	unsigned long time = TimeUtil::getTimeUSec();
	for (int i = 0; i < (int) leakyBucket_f.size() ; ++i)
	{
		double v1 = leakyBucket_b[i] + Wunit;
		double v2 = leakyBucket_f[i] + (double)(time - previousTime)/1000*leakyBucket_r[i];
		leakyBucket_f[i] = v1 < v2 ? v1 : v2;
	}
	#if _DEBUG == 1
	cout << "CoolShaper::runShaper ";
	displayvector(leakyBucket_f, "leakyBucket_f");
	#endif

	previousTime = time;


	double min_f = minElement(leakyBucket_f);
	if (min_f >= Wunit){
		if (worker->hasTask()){
			worker->setNextAction(_run);
		}else{
			worker->enableInterrupt();
			worker->setNextAction(_waiting);
		}

	}else{
		double tsleep = 0;
		for (int i = 0; i < (int)leakyBucket_f.size(); ++i)
		{
			double temp = (Wunit - leakyBucket_f[i]) / leakyBucket_r[i];
			if (temp > tsleep){
				tsleep = temp;
			}
		}
		//cout << "CoolShaper:: sleep time : " << tsleep << endl;
		worker->setNextAction(_idle);
		worker->setIdleLength((unsigned long)(tsleep*1000));
	}
	unsigned long thisExpense = TimeUtil::getTimeUSec() - time;
	timeExpenses.push_back(thisExpense);

}

void CoolShaper::decreaseLeakyBucketf(double w){	
	leakyBucket_f = leakyBucket_f - w;
	for (int i = 0; i < (int) leakyBucket_f.size(); ++i)
	{
		if (leakyBucket_f[i]<-0.0000001){
			leakyBucket_f[i] = 0;
		}
	}
	#if _DEBUG == 1
	cout << "CoolShaper::decreaseLeakyBucketf ";
	displayvector(leakyBucket_f, "leakyBucket_f");
	#endif


}

double CoolShaper::getWunit(){
	return Wunit;
}

std::vector<unsigned long> CoolShaper::getExpense(){
	return timeExpenses;
}