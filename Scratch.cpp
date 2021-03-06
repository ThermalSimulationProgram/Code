#include "Scratch.h"


#include "utils.h"
#include "vectormath.h"

#include <iostream>

using namespace std;


string 					Scratch::name;
int 					Scratch::nstage;
vector<unsigned long> 			Scratch::period;
vector<unsigned long> 			Scratch::jitter;
vector<unsigned long> 			Scratch::distance;
vector<unsigned long> 			Scratch::rltDeadline;
vector<double>					Scratch::rltDeadline_ms;
vector<vector<unsigned long> > 	Scratch::wcets;
vector<vector<double> > 			Scratch::dwcets;
vector<vector<unsigned long> >	Scratch::arrival_times;
enum _schedule_kernel 	Scratch::kernel;
unsigned long 			Scratch::duration;
double 					Scratch::exefactor;
double 					Scratch::bfactor;
unsigned long 			Scratch::adaption_period;
ptmspec 				Scratch::ptm;
thermalProp 			Scratch::offlinedata;
sem_t 					Scratch::access_sem;
bool 					Scratch::isSave;
string 					Scratch::benchmark;

double Scratch::Wunit = 50; //unit millisecond
std::vector<double> Scratch::leakyBucket_b;
std::vector<double> Scratch::leakyBucket_r;

bool Scratch::isSaveTemperatureTrace = false;


void Scratch::initialize(int _nstage, unsigned long _period,
	unsigned long _jitter, unsigned long _distance,
	unsigned long _rltDeadline, vector<unsigned long> _wcets,
	vector<unsigned long> _arrival_times, 
	enum _schedule_kernel _kernel, unsigned long _duration,
	string _name){

	nstage 			= _nstage;
	period.push_back(_period);       
	jitter.push_back(_jitter);
	distance.push_back(_distance);
	rltDeadline.push_back(_rltDeadline);
	rltDeadline_ms.push_back((double)_rltDeadline/1000);
	wcets.push_back(_wcets);
	arrival_times.push_back(_arrival_times); 
	kernel 			= _kernel;
	duration 		= _duration;
	name            = _name;
	adaption_period = 20000;
	exefactor 		= 1;
	bfactor 		= 0.93;
	isSave 			= true;
	dwcets.push_back(wcets[0]/1000);

	benchmark 		= "default";

	sem_init(&access_sem, 0, 1);
}

void Scratch::print(){
	cout << "nstage \t\t\t= " << nstage << endl;
	// cout << "period \t\t\t= " << period << endl;
	// cout << "jitter \t\t\t= " << jitter << endl;
	// cout << "distance \t\t= " << distance << endl;
	// cout << "rltDeadline \t\t= " << rltDeadline << endl;
	// cout << "rltDeadline_ms \t\t= " << rltDeadline_ms << endl;
	cout << "kernel \t\t\t= " << kernel << endl;
	cout << "duration \t\t= " << duration << endl;
	cout << "name \t\t\t= " << name << endl;
	cout << "adaption_period \t= " << adaption_period << endl;
	cout << "exefactor \t\t= " << exefactor << endl;
	cout << "bfactor \t\t= " << bfactor << endl;
	displayvector(period, "period");
	displayvector(jitter, "jitter");
	displayvector(distance, "distance");
	displayvector(rltDeadline, "rltDeadline");
	displayvector(rltDeadline_ms, "rltDeadline_ms");
	for (int i = 0; i < (int)wcets.size(); ++i)
	{
		displayvector(wcets[i], "wcets");
		displayvector(dwcets[i], "dwcets");
	}
	
	displayvector(ptm.tons, "tons");
	displayvector(ptm.toffs, "toffs");

	displayvector(leakyBucket_b, "leakyBucket_b");
	displayvector(leakyBucket_r, "leakyBucket_r");
	cout << "Wunit \t\t= " << Wunit << endl;
}

void Scratch::addTask(unsigned long _period, unsigned long _jitter,
				unsigned long _distance, unsigned long _deadline, 
				std::vector<unsigned long> _wcet, std::vector<unsigned long> _arrivaltimes){
	sem_wait(&access_sem);
	period.push_back(_period);
	jitter.push_back(_jitter);
	distance.push_back(_distance);
	rltDeadline.push_back(_deadline);
	rltDeadline_ms.push_back((double)_deadline/1000);
	wcets.push_back(_wcet);
	dwcets.push_back(_wcet/1000);
	arrival_times.push_back(_arrivaltimes);
	sem_post(&access_sem);
}


void Scratch::setBenchmark(const string& name){
	sem_wait(&access_sem);
	benchmark = name;
	sem_post(&access_sem);
}


void Scratch::setSavingFile(bool f){
	sem_wait(&access_sem);
	isSave = f;
	sem_post(&access_sem);
}

bool Scratch::isSaveFile(){
	sem_wait(&access_sem);
	bool ret = isSave;
	sem_post(&access_sem);
	return ret;
}

void Scratch::setName(string newname){
	sem_wait(&access_sem);
	name = newname;
	sem_post(&access_sem);
}

void Scratch::setAdaptionPeriod(unsigned long p){
	sem_wait(&access_sem);
	adaption_period = p;
	sem_post(&access_sem);
	
}

void Scratch::setPTMValues(ptmspec _ptm){
	sem_wait(&access_sem);
	if (kernel != PBOO && kernel != GE){
		cout << "Scratch::setPTMValues:Set PTM parameters only for PBOO and GE kernel"<< endl;
		sem_post(&access_sem);
		return;
	}
	
	ptm = _ptm;
	sem_post(&access_sem);
	
}

void Scratch::setOfflineData(thermalProp data){
	sem_wait(&access_sem);
 	if (kernel != APTM){
		cout << "Scratch::setOfflineData:Set offline data only for APTM kernel"<< endl;
		sem_post(&access_sem);
		return;
	}
	if ((int)data.allStageCurves.size()!= nstage){
		cout << "Scratch::setOfflineData:Invalid input offline data"<<endl;
		sem_post(&access_sem);
		return;
	}

	offlinedata = data;
	sem_post(&access_sem);
}
thermalProp Scratch::getOfflineData(){
	thermalProp ret;
	sem_wait(&access_sem);
	ret = offlinedata;
	sem_post(&access_sem);
	return ret;
}


void Scratch::setExeFactor(double f){
	if(f<0 || f>1){
		cout << "Scratch::setExeFactor:Input exefactor out of range" << endl;
		return;
	}
	sem_wait(&access_sem);
	exefactor = f;
	sem_post(&access_sem);

}

void Scratch::setBFactor(double f){
	if(f<0 || f>1){
		cout << "Scratch::setBFactor:Input bfactor out of range" << endl;
		return;
	}

	sem_wait(&access_sem);
	if (kernel != APTM){
		sem_post(&access_sem);
		cout << "Scratch::setBFactor:Set bfactor only for APTM kernel"<< endl;
		return;
	}
	bfactor = f;
	sem_post(&access_sem);

}

string Scratch::getBenchmarkName(){
	sem_wait(&access_sem);
	string ret = benchmark;
	sem_post(&access_sem);
	return ret;

}

unsigned long Scratch::getAdaptionPeriod(){
	sem_wait(&access_sem);
	unsigned long ret = adaption_period;
	sem_post(&access_sem);
	return ret;

}


ptmspec Scratch::getPTMValues(){
	sem_wait(&access_sem);
	if (kernel != PBOO && kernel != GE){
		cout << "Scratch::getPTMValues:Warning get PTM parameters only for PBOO and GE kernel";
	}
	ptmspec ret = ptm;
	sem_post(&access_sem);
	return ret;
}


string Scratch::getName(){
	sem_wait(&access_sem);
	string ret = name;
	sem_post(&access_sem);
	return ret;
}
int Scratch::getNstage(){
	sem_wait(&access_sem);
	int ret = nstage;
	sem_post(&access_sem);
	return ret;
}
unsigned long Scratch::getPeriod(){
	sem_wait(&access_sem);
	unsigned ret = period[0];
	sem_post(&access_sem);
	return ret;
}
unsigned long Scratch::getJitter(){
	sem_wait(&access_sem);
	unsigned long ret = jitter[0];
	sem_post(&access_sem);
	return ret;
}
unsigned long Scratch::getDistance(){
	sem_wait(&access_sem);
	unsigned long ret = distance[0];
	sem_post(&access_sem);
	return ret;
}
unsigned long Scratch::getRltDeadline(){
	sem_wait(&access_sem);
	unsigned long ret = rltDeadline[0];
	sem_post(&access_sem);
	return ret;
}
double 	Scratch::getRltDeadline_ms(){
	sem_wait(&access_sem);
	double ret = rltDeadline_ms[0];
	sem_post(&access_sem);
	return ret;
}
vector<unsigned long> Scratch::getWcets(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = wcets[0];
	sem_post(&access_sem);
	return ret;
}
vector<double> 	Scratch::getDwcets(){
	sem_wait(&access_sem);
	vector<double> ret = dwcets[0];
	sem_post(&access_sem);
	return ret;
}


std::vector<std::vector<unsigned long> > Scratch::getAllArrivalTimes(){
	sem_wait(&access_sem);
	std::vector<std::vector<unsigned long> > ret = arrival_times;
	sem_post(&access_sem);
	return ret;
}

std::vector<std::vector<double> > Scratch::getAllArrivalTimes_ms(){
	sem_wait(&access_sem);
	vector<vector<double> > ret;
	for (int i = 0; i < (int)arrival_times.size(); ++i)
	 {
	 	ret.push_back(arrival_times[i]/1000);
	 } 
	sem_post(&access_sem);
	return ret;
}


vector<unsigned long> Scratch::getArrivalTimes(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = arrival_times[0];
	sem_post(&access_sem);
	return ret;
}

vector<double> Scratch::getArrivalTimes_ms(){
	sem_wait(&access_sem);
	vector<double> ret;
	for (unsigned i = 0; i < arrival_times[0].size(); ++i)
		ret.push_back((double)arrival_times[0][i]/1000);
	
	sem_post(&access_sem);
	
	return ret;
}


enum _schedule_kernel Scratch::getKernel(){
	sem_wait(&access_sem);
	enum _schedule_kernel ret = kernel;
	sem_post(&access_sem);
	return ret;
}
unsigned long Scratch::getDuration(){
	sem_wait(&access_sem);
	unsigned long ret = duration;
	sem_post(&access_sem);
	return ret;
}

double Scratch::getExeFactor(){
	sem_wait(&access_sem);
	double ret = exefactor;
	sem_post(&access_sem);
	return ret;
}

double Scratch::getBFactor(){
	sem_wait(&access_sem);
	double ret = bfactor;
	sem_post(&access_sem);
	return ret;
}



std::vector<unsigned long> 	Scratch::getAllPeriod(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = period;
	sem_post(&access_sem);
	return ret;
}
vector<unsigned long> 	Scratch::getAllJitter(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = jitter;
	sem_post(&access_sem);
	return ret;
}
vector<unsigned long> 	Scratch::getAllDistance(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = distance;
	sem_post(&access_sem);
	return ret;
}
vector<unsigned long> Scratch::getAllRltDeadline(){
	sem_wait(&access_sem);
	vector<unsigned long> ret = rltDeadline;
	sem_post(&access_sem);
	return ret;
}
vector<double> 	Scratch::getAllRltDeadline_ms(){
	sem_wait(&access_sem);
	vector<double> ret = rltDeadline_ms;
	sem_post(&access_sem);
	return ret;
}
vector<std::vector<unsigned long> > Scratch::getAllWcets(){
	sem_wait(&access_sem);
	vector<std::vector<unsigned long> > ret = wcets;
	sem_post(&access_sem);
	return ret;
}
vector<std::vector<double> >	Scratch::getAllDwcets(){
	sem_wait(&access_sem);
	vector<std::vector<double> > ret = dwcets;
	sem_post(&access_sem);
	return ret;
}




