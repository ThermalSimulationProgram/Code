#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <string>
#include <semaphore.h>
#include <jni.h>
#include <vector>
#include <exception>
#include <algorithm>

// #include "Thread.h"
#include "TimeUtil.h"
// #include "Operators.h"
#include "Enumerations.h"
// #include "Task.h"
// #include "TimeSim.h"
// #include "Dispatcher.h"
// #include "Scheduler.h"
#include "Pipeline.h"
// #include "Worker.h"
#include "Parser.h"
#include "Scratch.h"
#include "warming.h"
#include "rtc.h"
// #include "temperature.h"
#include "vectormath.h"
#include "APTMKernel.h"


#include "utils.h"
#include "FileOperator.h"

using namespace std;




#define _TESTRTC  0

void testCalcAPTM(vector<double> &tons, vector<double>& toffs, unsigned i);
void runSimulation(int argc, char** argv);
void offlineSimulation();


void testAssignToff(double upBound, 
	const vector<int> &index, const vector<double>& tau0, 
	const vector<vector<double>>& breakToffs,
	const vector<vector<double>> &slopes, 
	const vector<int>& numValidData);

void testrtc();

void testStringUtils();
void testFileOperator();

int main(int argc, char** argv){
	

	
// vector<unsigned long> wcets = {14200, 9000, 3600, 5700};

//   getCoolingCurve(4, wcets);

 // testrtc();
// runSimulation( argc, argv);

//	offlineSimulation();
	testStringUtils();
testFileOperator();
	
}

void testFileOperator(){


	vector<double> t = loadVectorFromFile<double>("testfile.xml");
	dumpVector(t);

	 t = loadVectorFromFile<double>("notexistfile.csv");
}



void testStringUtils(){
	string test = " 1 ,5, 7";
	vector<double> t = stringToVector<double>(test);
	appStringToVector(test, t);
	displayvector(t, "t");
}


void offlineSimulation(){
	unsigned ncore = 4;
	vector<unsigned long> wcets = {14200, 9000, 3600, 5700};
	getWarmingCurve(ncore);
	getCoolingCurve(ncore, wcets);

}



void runSimulation(int argc, char** argv){


	string *file;
	if (argc > 1){

		for(int i=1; i<=argc; i++){
			if(argv[i] == NULL)
			    continue;
			file = new string(argv[i]);
			if(file->find(".xml") == string::npos) {
			*file = *file + ".xml";
      			}
		}
	}else{
		file = new string("example.xml");
	}

	Pipeline *p = new Pipeline(*file);
	p->initialize();

	//double temp = p->simulate();
	// cout << temp << endl;
	delete p;
	delete file;

}

void testAssignToff(double upBound, 
	const vector<int> &index, const vector<double>& tau0, 
	const vector<vector<double>>& breakToffs,
	const vector<vector<double>> &slopes, 
	const vector<int>& numValidData){

	// vector<double> t1 = vector<double>{15,11,17};
	// vector<double> t2 = vector<double>{22,17,19};
	// vector<double> t3 = vector<double>{50,45,78};
	// vector<vector<double>> tmp = vector<vector<double>>{t1,t2,t3};


	// vector<double> t11 = vector<double>{0.5, 0.4,0.37};
	// vector<double> t21 = vector<double>{0.38,0.35,0.25};
	// vector<double> t31 = vector<double>{0.2,0.25,0.21};
	// vector<vector<double>> tmp2 = vector<vector<double>>{t11,t21,t31};

	// vector<int> numValidData = {3,3,3};


	// vector<int> index={0,1,2};

	// vector<double> tau0 = {2,2,2};

	vector<double> ret;
	APTMKernel::assignToffs(ret, upBound,
		index, tau0, breakToffs, slopes, numValidData,
		vecsum(tau0));
}


void testCalcAPTM(vector<double> &tons, vector<double>& toffs, unsigned i){
	Parser p = Parser("example_aptm.xml");
	p.parseFile();

	unsigned nstages = 4;

	APTMKernel aptmkernel = APTMKernel(nstages, Scratch::getDwcets(), 
		vector<double>(nstages, 2), 0.93, APTM);

		
	aptmkernel.setOfflineData(Scratch::getOfflineData());

	 pipeinfo config;
	 config.Q = vector<int>{0,1,0,0};
	 config.activeSet = vector<int>{0,1,2,3};
	 config.sleepSet = vector<int>{};
	 config.ccs = vector<double>{};
	 ///double ttt = 36 + (double)(i%17)/17;
	 config.dcs = vector<double>{std::numeric_limits<double>::infinity(),
	 	std::numeric_limits<double>::infinity(), 4.20732,69.7434};
	 config.rho = vector<double>{0.0127029, 0.0127029,0.0127029,0.0127029};	
	 config.K = vector<double>{0.180382,0.114326,0.0457306,0.0724068};
	 config.allT = vector<double>{30,30,30,30};
	 vector<double> t1 = vector<double>{0,0,0};
	 vector<double> t2 = vector<double>{0,0,0,89.372,0.849333,0};
	 vector<double> t3 = vector<double>{0,0,0};
	 vector<double> t4 = vector<double>{0,0,0};
	 vector<vector<double>> tmp = vector<vector<double>>{t1,t2,t3,t4};
	 config.FIFOcurveData = tmp;
	 config.adaptionIndex = 159;



	 aptmkernel.calcAPTM(tons, toffs, config);

}


void testrtc(){
	rtc::initialize();

	long period = 100;
	double jitter = 150;
	long delay = 1;

	bucket bb = rtc::initbucket(period, jitter, delay);

	jobject alpha = rtc::historyAwareAlphaByLeakyBucket_BSW(bb,  50, 3);
	cout << rtc::toString(alpha);


//b = rtcpjdu(134,67,11);
	 jobject pjd = rtc::createPJDCurve(100, 0, 0, true );
	 jobject pjd2 = rtc::affine(pjd, 1, 90);
	 vector<double> pjdtestdata = rtc::segementsData(pjd2, 1200);
	 displayvector(pjdtestdata, "pjdtestdata");

	 cout << rtc::minspeedbdfEDG(pjd2, 59) << endl;
	 cout << rtc::minspeedbdfEDG(pjdtestdata, 59) << endl;

	 cout << rtc::minbdf_BSF(pjd2, 120, 0.4) << endl;
	 cout << rtc::minbdf_BSF(pjdtestdata, 120, 0.4) << endl;

	 jobject c = rtc::maxconv(alpha, pjd );
cout << rtc::toString(c );
	//d = rtcapproxs(c, 500, 0, 1);
	jobject d = rtc::approxs(c, 700, 1, true );
	cout << rtc::toString(d );
	vector<double> data = {0,0,0,5,0,1,10,5,1};
	vector<double> data2 = {0,2,0,5,2,1,10.1,7,1};
	jobject apercurve = rtc::Curve(data );
	jobject apercurve2 = rtc::Curve(data);
	jobject apercurve3 = rtc::Curve(data2);
	bool isequal1 = rtc::equals(pjd, apercurve );
	bool isequal2 = rtc::equals(apercurve2, apercurve );
	bool isequal3 = rtc::equals(apercurve3, apercurve );
	cout << "isequal1: " << isequal1 << endl;
	cout << "isequal2: " << isequal2 << endl;
	cout << "isequal3: " << isequal3 << endl;
	double y0 = rtc::y0epsilon(apercurve3 );
	cout << "y0: " << y0 << endl;
	double pdxx = rtc::pdx(apercurve3 );
	cout << "pdxx: " << pdxx << endl;

	jobject aclone = rtc::clone(apercurve3 );
	bool isequal4 = rtc::equals(apercurve3, aclone );
	cout << "isequal4: " << isequal4 << endl;

	double maxvd = rtc::maxVDist(apercurve3, apercurve );
	cout << "maxvd: " << maxvd << endl;

	string curve3 = rtc::toString(apercurve3 );
	cout << curve3 << endl;

	jobject minconvret = rtc::sub(apercurve, pjd );
	cout << rtc::toString(minconvret);



}


// dynamicdata getDynamicData(){
// 	dynamicdata d;

// 	unsigned ncols = (unsigned) getDouble("ncols.csv");
// 	unsigned ncols2 = (unsigned) getDouble("ncols2.csv");
// 	d.wcets = getVector<double>("wcets.csv");
// 	d.rho = getVector<double>("rho.csv");
// 	d.numValidData = getVector<unsigned>("numValidData.csv");
// 	d.breakToffs = getMatrix("breakToffs.csv",  ncols);
// 	d.slopes = getMatrix("slopes.csv",  ncols);
// 	d.breakToffs2 = getMatrix("breakToffs2.csv",  ncols2);
// 	d.slopes2 = getMatrix("slopes2.csv",  ncols2);
// 	d.temps = getMatrix("temps.csv",  ncols2);
// 	d.K = d.wcets * d.rho;
// 	return d;
// }



	/*Pipeline *p = new Pipeline("example.xml");
	p->initialize();

	p->simulate();
	 // thermalProp offdata = Parser::getOfflineData("testoffline", 3);
//////////////////////////////////////////////////////////////

	  unsigned nstages = 4;*/

	 // rtc::initialize();
	/*double tmp = std::stod("14.2 9 3.6");

	cout << tmp;
	exit(1);*/

	 // vector<unsigned> rl_adaption_times = integerVector(300, 18000000, 20000);

	 // Parser p = Parser("example.xml");
	 // p.parseFile();
	 // displayvector(Scratch::getWcets(), "getWcets");
	 // cout << "example.xml filed loaded"<<endl;

	 // vector<double> threezero = {0, 0, 0};
	 // jobject curvezero = rtc::Curve(threezero, "Scheduler::init");
	 // Scratch::setCurveZero(curvezero);


// 	 vector<double> rl_job_arrivals = Scratch::getArrivalTimes_ms();

// 	 long period = (long)Scratch::getPeriod()/1000;
// 	 double jitter = (double)Scratch::getJitter()/1000;
// 	 long delay  = (long)Scratch::getDistance()/1000;

// vector<jobject> haalpha =  Scheduler::setHaAlpha( rl_job_arrivals,
// 	 rl_adaption_times,	 period,  jitter,  delay);

	// APTMKernel aptmkernel = APTMKernel(nstages, Scratch::getDwcets(), 
	// 	vector<double>(nstages, 2), 0.93, APTM);

	// // aptmkernel.setHaAlpha(haalpha);
		
	// aptmkernel.setOfflineData(Scratch::getOfflineData());

	//  pipeinfo config;
	//  config.Q = vector<int>{0,1,0,0};
	//  config.activeSet = vector<int>{0,1,2,3};
	//  config.sleepSet = vector<int>{};
	//  config.ccs = vector<double>{};
	//  config.dcs = vector<double>{std::numeric_limits<double>::infinity(),
	//  	std::numeric_limits<double>::infinity(), 4.20732,69.7434};
	//  config.rho = vector<double>{0.0127029, 0.0127029,0.0127029,0.0127029};	
	//  config.K = vector<double>{0.180382,0.114326,0.0457306,0.0724068};
	//  config.allT = vector<double>{30,30,30,30};
	//  vector<double> t1 = vector<double>{0,0,0};
	//  vector<double> t2 = vector<double>{0,0,0,89.372,0.849333,0};
	//  vector<double> t3 = vector<double>{0,0,0};
	//  vector<double> t4 = vector<double>{0,0,0};
	//  vector<vector<double>> tmp = vector<vector<double>>{t1,t2,t3,t4};
	//  config.FIFOcurveData = tmp;
	//  // config.FIFOcurveData[0] = ;
	//  // config.FIFOcurveData[1] = vector<double>{0,0,0,89.372,0.849333,0};
	//  // config.FIFOcurveData[2] = vector<double>{0,0,0};
	//  // config.FIFOcurveData[3] = vector<double>{0,0,0};
	//  config.adaptionIndex = 159;

	//  vector<double> tons;
	//  vector<double> toffs;
	//  aptmkernel.calcAPTM(tons, toffs, config);

	// displayvector(config.Q, "config.Q");
	// displayvector(config.activeSet, "config.activeSet");
	// displayvector(config.sleepSet, "config.sleepSet");
	// displayvector(config.ccs, "config.ccs");
	// displayvector(config.dcs, "config.dcs");
	// displayvector(config.rho, "config.rho");
	// displayvector(config.K, "config.K");
	// displayvector(config.allT, "config.allT");

	// pipeinfo config;
	// Pipeline::getInfo2(config, config);
	// struct timespec rem;
	// struct timespec duration = TimeUtil::Millis(100);
	// struct timespec duration2 = TimeUtil::Millis(200);
	// int index;
	// int pre_index = -1;
	// do{
	// 	int counter = 0;
	// 	int block = 10;
	// 	do{
	// 		index = (int)getDouble("AdaptionIndex.csv");
	// 		if (counter > block){
	// 			cout<<"waiting for matlab"<<endl;
	// 			block += 10;
	// 		}
			
	// 		nanosleep(&duration, &rem);
	// 		counter++;
	// 	}while(index == pre_index);

	// 	nanosleep(&duration2, &rem);
	// 	pre_index = index;
	// 	ptmspec ret = aptmkernel.calcScheme();
	// displayvector(ret.tons, "ret.tons");
	// displayvector(ret.toffs, "ret.toffs");
	// saveDoubleVectorToFile2(ret.tons, "c++tons");
	// saveDoubleVectorToFile2(ret.toffs, "c++toffs");

	// }while(1);

	// cout << "start calculate aptm"<<endl;
	
	// vector<double> t144 = Scheduler::assignToffs( upBound, 
	// index,  tau0, breakToffs,slopes, numValidData);

// displayvector(t144, "t144");

// dynamicdata d = getDynamicData();




// double extBound = getDouble("extBound.csv");
//  index= getVector<unsigned>("index.csv");
//  index = index - 1;
//  tau0 = getVector<double>("tau0.csv");
//  numValidData = getVector<unsigned>("numValidData.csv");

// vector<double> tinvs;
// vector<double> tvlds;

// Scheduler::aPTM( extBound,  index, tau0, d, tinvs,  tvlds);
// displayvector(tinvs, "tinvs");
// displayvector(tvlds, "tvlds");


// struct timespec tend= TimeUtil::getTime();

// cout<<"all time is: "<<TimeUtil::convert_us(tend-tstart)<<endl;
// 	for (auto i: sort_indexes(a)) {
//   cout << a[i] << endl;
// }

	// vector<double> ret = Scheduler::reduceM(a, s, l, t, m);
	// for (unsigned i = 0; i < ret.size(); ++i)
	// {
	// 	cout<<ret[i]<<"\t";
	// }
	// cout << maxORminCore(a, integerVector(0,3), false) << endl;
	// cout << maxORminCore(l, integerVector(0,7),true) << endl;






// struct timespec tstart = TimeUtil::getTime();
// 	for (int i = 0; i < 1000; ++i)
// 	{
// 		vector<int> a = get_cpu_temperature();
// 	}
// 	struct timespec tend = TimeUtil::getTime();

// 	cout<<"all time is: "<<TimeUtil::convert_us(tend-tstart)<<endl;

// int main()
// {
// #if _TESTRTC == 1
// 	rtc::initialize();

	
// 	string callfun = "main";

// 	long period = 100;
// 	double jitter = 150;
// 	long delay = 1;

// 	bucket bb = rtc::initbucket(period, jitter, delay);

// 	jobject alpha = rtc::historyAwareAlphaByLeakyBucket_BSW(bb,  50, 3);
// 	cout << rtc::toString(alpha, callfun);


// //b = rtcpjdu(134,67,11);
// 	 jobject pjd = rtc::createPJDCurve(134, 67, 11, true, callfun);

// 	 jobject c = rtc::maxconv(alpha, pjd, callfun);
// cout << rtc::toString(c, callfun);
// 	//d = rtcapproxs(c, 500, 0, 1);
// 	jobject d = rtc::approxs(c, 700, 1, true, callfun);
// 	cout << rtc::toString(d, callfun);
	// vector<double> data = {0,0,0,5,0,1,10,5,1};
	// vector<double> data2 = {0,2,0,5,2,1,10.1,7,1};
	// jobject apercurve = rtc::Curve(data, callfun);
	// jobject apercurve2 = rtc::Curve(data,callfun);
	// jobject apercurve3 = rtc::Curve(data2,callfun);
	// bool isequal1 = rtc::equals(pjd, apercurve, callfun);
	// bool isequal2 = rtc::equals(apercurve2, apercurve, callfun);
	// bool isequal3 = rtc::equals(apercurve3, apercurve, callfun);
	// cout << "isequal1: " << isequal1 << endl;
	// cout << "isequal2: " << isequal2 << endl;
	// cout << "isequal3: " << isequal3 << endl;
	// double y0 = rtc::y0epsilon(apercurve3, callfun);
	// cout << "y0: " << y0 << endl;
	// double pdxx = rtc::pdx(apercurve3, callfun);
	// cout << "pdxx: " << pdxx << endl;

	// jobject aclone = rtc::clone(apercurve3, callfun);
	// bool isequal4 = rtc::equals(apercurve3, aclone, callfun);
	// cout << "isequal4: " << isequal4 << endl;

	// double maxvd = rtc::maxVDist(apercurve3, apercurve, callfun);
	// cout << "maxvd: " << maxvd << endl;

	// string curve3 = rtc::toString(apercurve3, callfun);
	// cout << curve3 << endl;

	// jobject minconvret = rtc::sub(apercurve, pjd, callfun);
	// cout << rtc::toString(minconvret, callfun);

	// long p1 = rtc::period(pjd, callfun);
	// long p2 = rtc::period(apercurve, callfun);
	// rtc::setName(pjd, "initName", callfun);
	// string newname = rtc::name(pjd, callfun);


	// rtc::scaleX(pjd, 2, callfun);
	// p1 = rtc::period(pjd, callfun);
	// cout << "the period of pjd is " << p1 << endl;
	// cout << "the name of pjd is " << newname << endl;
	// rtc::setName(pjd, "testName", callfun);
	// newname = rtc::name(pjd, callfun);
	// cout << "the name of pjd is " << newname << endl;
	// cout << "the second curve period is " << p2 << endl;

	// rtc::scaleY(apercurve, 1.5, callfun);
	// rtc::move(apercurve, 2, 3, callfun);
	// jobject aperdpart = rtc::aperiodicSegments(apercurve, callfun);
	// if(aperdpart == NULL)
	// {
	// 	cout << "error" << endl;
	// }


	

// 	#endif

// getWarmingCurve(8);
  // string path = "./warming_curve.xml";

  // Pipeline* p = new Pipeline(path);

  // // struct timespec ton = TimeUtil::Millis((long int)30000);
  // p->simulate();

  // sleep(1);
  //delete p;

  
// }





























	
// 		jobject lastseg = rtc::lastSegment(aperdpart, callfun);
// 		if(lastseg==NULL)
// 	{
// 		cout << "error" << endl;
// 	}
// 		double _s = rtc::s(lastseg, callfun);
// 		double _x = rtc::x(lastseg, callfun);
		
		
// 			haserror = false;
// 			cout << "the x of the start point of last segment is " << _x << endl;
// 			cout << "the slope of the start point of last segment is " << _s << endl;
		

		
	
// 	if(haserror)
// 	{
// 		cout << "error" << endl;
// 	}





















	//cerr << "test exception" << endl;
	


	// JavaVM *jvm;
	// JNIEnv *env;
	// int res;
	// JavaVMInitArgs vm_args;
	// JavaVMOption *options = new JavaVMOption[1];
	// options[0].optionString = const_cast<char*> (".");
	// //options[1].optionString = "-verbose:jni"; 
	// vm_args.version = JNI_VERSION_1_8;
	// vm_args.nOptions = 1;
	// vm_args.options = options;
	// vm_args.ignoreUnrecognized = true;

	// jthrowable exc;

	// res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	// if (res != JNI_OK)
	// {
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}

	// 	cout << "creat JVM failed" << endl;
	// }
	// delete options;



	// jclass CurveFactory = env->FindClass("ch/ethz/rtc/kernel/CurveFactory");
	// if (CurveFactory == NULL )
	// {		
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
	// 	cout << "find class CurveFactory failed \n";

	// }

	// jclass Curve = env->FindClass("ch/ethz/rtc/kernel/Curve");
	// if ( Curve == NULL)
	// {
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();

	// 	}
	// 	cout << "find class Curve failed \n";

	// }

	// jmethodID hasPeriodPart = env->GetMethodID(Curve, "hasAperiodicPart", "()Z");
	// if (hasPeriodPart == NULL)
	// {
	// 	cout << "failed to get method id hasPeriodPart";
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
		
	// }

	// jmethodID name = env->GetMethodID(Curve, "name2", "()Ljava/lang/String;");
	// test(env, "failed to get method id name");
	
	// cout << "out of test!";
	// jmethodID setName = env->GetMethodID(Curve, "setName", "(Ljava/lang/String;)V");
	// if (setName == NULL)
	// {
	// 	cout << "failed to get method id setName";
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
		
	// }

	// jmethodID getPeriod = env->GetMethodID(Curve, "period", "()J");
	// if (getPeriod == NULL)
	// {
	// 	cout << "failed to get method id getPeriod";
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
		
	// }


	// jmethodID createLowerPJDCurve = env->GetStaticMethodID(CurveFactory, "createLowerPJDCurve", "(JDDLjava/lang/String;)Lch/ethz/rtc/kernel/Curve;");
	
	// if (createLowerPJDCurve == NULL)
	// {
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
	// 	cout << "failed to get method id createLowerPJDCurve";
	// }



	// jstring _curvename = env->NewStringUTF("testname");
	// if(_curvename == NULL)
	// {
	// 	exc = env->ExceptionOccurred();
	// 	if (exc) {
	// 		env->ExceptionDescribe();
	// 		env->ExceptionClear();
	// 	}
	// 	cout << "new jstring _curvename is null\n!";

	// }
	// jvalue _p, _j, _d, _name;
	// _p.j=100; _j.d=0; _d.d=0; _name.l=_curvename;
	// const jvalue argsss[4] = {_p, _j, _d, _name};
	// jobject ret = env->CallStaticObjectMethodA(CurveFactory, createLowerPJDCurve, argsss);

	// exc = env->ExceptionOccurred();
	// if (exc) {
	// 	env->ExceptionDescribe();
	// 	env->ExceptionClear();
	// }

	//  jboolean b = env->CallBooleanMethod(ret, hasPeriodPart, 0);
	//  exc = env->ExceptionOccurred();
	//  if (exc) {
	//  	env->ExceptionDescribe();
	//  	env->ExceptionClear();
	//  }

	// env->CallVoidMethod(ret, setName, _curvename);
	// jstring rv = (jstring) env->CallObjectMethod(ret, name, 0);
	// if (rv == NULL)
	// {exc = env->ExceptionOccurred();
	//  if (exc) {
	//  	env->ExceptionDescribe();
	//  	env->ExceptionClear();
	//  }
	//  cout << "NULL string retruned" << endl;

	// }
	// else
	// {
	// 	jsize hh = env->GetStringUTFLength(rv);
	// int ll = hh;
	// cout << "the length is " << ll << endl;
	// }
	
	
	//  jlong period = env->CallLongMethod(ret, getPeriod, 0);;
	//  long p = period;



	//  const char *strReturn = env->GetStringUTFChars(rv, NULL);
	// exc = env->ExceptionOccurred();
	//  if (exc) {
	//  	env->ExceptionDescribe();
	//  	env->ExceptionClear();
	//  }
	//  string xx = string(strReturn);
	//  env->ReleaseStringUTFChars(rv, strReturn);

	// bool bb = b;
	// if (bb)
	// {
	// 	cout << "object " <<  xx << " has periodic part with period: "<< p <<endl;
	// }
	// else
	// {
	// 	cout << "object " <<  xx << "  don't have  periodic part"<<endl;
	// }




// struct timespec tstart = TimeUtil::getTime();
// 	std::vector<unsigned> a = {1,2,5,7};
// 	std::vector<unsigned> s = {1,2,4,7};
// 	std::vector<double>   l = {2.3,6.7,8.0,7.4,4.7,55.8,6.9,8.0};
// 	std::vector<double>   t = {0.0,1.2,1.4,1.6};
// 	unsigned m = 8;

// 	vector<vector<unsigned>> xx = {a,s,s,a};
// 	vector<unsigned> t100 = vectorExtract(xx, integerVector(0, 15, 4));

// 	displayvector(t100, "t100");
	

// 	std::vector<unsigned> t111 = s - 1;
// 	displayvector(t111, "t111");
// 	std::vector<unsigned> t112 = 6 - a;
// 	displayvector(t112, "t112");

// 	std::vector<unsigned> t113 = s - a;
// 	displayvector(t113, "t113");

	

// 	std::vector<unsigned> t311 = s + 6;
// 	displayvector(t311, "t311");

// 	std::vector<unsigned> t312 = 8 + a;
// 	displayvector(t312, "t312");
	
// 	std::vector<unsigned> t313 = s + a;
// 	displayvector(t313, "t313");


// 	std::cout << vecsum(l, 1,2) << endl;
// 	std::cout << vecsum(l)<<endl;


// 	std::vector<double> t51=a/2;
// 	displayvector(t51, "t51");
// 	std::vector<double> t52=a/s;
// 	displayvector(t52, "t52");
// 	std::vector<double> t53=0.5/s;
// 	displayvector(t53, "t53");

	
// 	std::vector<double> t61=0.5*s;
// 	displayvector(t61, "t61");
// 	std::vector<double> t62=s*3.1;
// 	displayvector(t62, "t62");
// 	std::vector<double> t63= a*s;
// 	displayvector(t63, "t63");


// 	std::vector<bool> t41=1.5>s;
// 	displayvector(t41, "t41");
// 	std::vector<bool> t42=s>2.0;
// 	displayvector(t42, "t42");
// 	std::vector<bool> t43= vectorLarger(a,s);
// 	displayvector(t43, "t43");

// 	std::vector<bool> t211=1.5<s;
// 	displayvector(t211, "t211");
// 	std::vector<bool> t212=s<2.0;
// 	displayvector(t212, "t212");
// 	std::vector<bool> t213= vectorLess(a,s);
// 	displayvector(t213, "t213");
	


// 	std::vector<unsigned>t7 = equalIndex(vector<unsigned>(5,3), (unsigned)3);
// 	displayvector(t7, "t7");

// 	// std::vector<double> t8 = vectorAssign(l, s, t);
// 	// displayvector(t8, "t8");

// 	std::vector<double> t9 = vectorExtract(l, s);
// 	displayvector(t9, "t9");

// 	unsigned id;
// 	std::cout<<minElement(t, &id)<<endl;
// 	std::cout<<id<<endl;

// 	std::cout<< minElement(l, 2,6,&id)<<endl;
// 	std::cout<<id<<endl;

// 	std::vector<double> t10 = minElement(t, s);
// 	displayvector(t10, "t10");

// 	std::vector<long int> t11= vecFloor(l);
// 	displayvector(t11, "t11");

// 	std::vector<bool> t121 = l == 6.7;
// 	displayvector(t121, "t121");
// 	std::vector<bool> t122 = vectorEqual(s,a);
// 	displayvector(t122, "t122");
// 	std::vector<bool> t123 = 3 < a;
// 	displayvector(t123, "t123");


// 	displayvector(t123 & t122, "t123&t122");


// 	std::cout << vectorAny(t121)<<endl;
// 	std::cout << vectorAll(t121)<<endl;




// 	std::vector<bool> t134 = vecNot(t);
// 	displayvector(t134, "t134");

// 	vectorAssign(t, t134, vectorExtract(t, t134)+1);
// 	displayvector(t, "t");

// 	double upBound = 24.0836;
// 	vector<unsigned> index={1, 2,3};
// 	vector<double> tau0 = {4, 3.3, 3.3};
// 	vector<vector<double>> breakToffs = { vector<double>{6.6315,   11.8315 ,  17.2315}, 
// 	 vector<double>{0, 0, 0}, vector<double>{0, 0, 0}, vector<double>{0, 0, 0}};
//     vector<vector<double>> slopes = { vector<double>{-1.0987,   -1.1313,   -1.2187}, 
// 	 vector<double>{0, 0, 0}, vector<double>{0, 0, 0}, vector<double>{0, 0, 0}};	
// 	 vector<unsigned> numValidData = {1,1,1};

// 	 vector<double> t151=vectorExtract(breakToffs, index, 1);
// 	 displayvector(t151, "t151");
	 

// 	 vector<vector<double>> testmat = getMatrix("testmatrix.csv");
// 	 for (unsigned i = 0; i < testmat.size(); ++i)
// 	 {
// 	 	displayvector(testmat[i], "testmat");
// 	 }
