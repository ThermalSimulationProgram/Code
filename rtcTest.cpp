#include "UnitTesting.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "rtc.h"
#include "utils.h"
#include "RTCCurveDataSubIterator.h"


using namespace std;
void randomly_test_rtc_plus(int loopnum);
bool test_rtc_plus(jobject a, jobject b, double x);

void testrtc(){
	rtc::initialize();

	vector<double> testzero{0, 0, 0.1};

	RTCCurveDataIterator testit(testzero, 10);

	testit.print();

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
	 testit = RTCCurveDataIterator(pjdtestdata, 1211);
	 testit.print();


	 

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

	RTCCurveDataSubIterator testit2 (pjdtestdata, data, 1300);
	testit2.print();

	vector<double> sumdata = rtc::plus(pjdtestdata, rtc::segementsData(c, 1300));

	displayvector(sumdata, "sumdata");

	jobject sum_pjd_c = rtc::plus(pjd2, c);
	displayvector(rtc::segementsData(sum_pjd_c, 1300), "sumdata2");

	bool flag = true;
	for (int i = 0; i < 1600; i+=10)
	{
		if (!test_rtc_plus(pjd2, c, (double)i+1)){
			cout << "failed! i = " << i << endl;
			flag = false;
			break;
		}
	}
	if (flag){
		cout << "passed rtc plus test!" << endl;
	}

	randomly_test_rtc_plus(100);

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


bool test_rtc_plus(jobject a, jobject b, double x){
	vector<double> dataA = rtc::segementsData(a, x);
	vector<double> dataB = rtc::segementsData(b, x);
	jobject sum = rtc::plus(a,b);
	vector<double> test_sum = rtc::plus(dataA, dataB);
	vector<double> correct_sum = rtc::segementsData(sum, x);
	
	if (test_sum.size() != correct_sum.size()){
		return false;
	}

	for (int i = 0; i < (int) test_sum.size(); ++i)
	{
		if (abs(test_sum[i] - correct_sum[i]) > 0.0001){
			return false;
		}
	}
	return true;


}

void randomly_test_rtc_plus(int loopnum){
	bool allflag = true;
	for (int i = 0; i < loopnum; ++i)
	{	
		long p1 = rand()%1000 + 60;
		double j1 = (double) (rand()%(2*p1) + 1);
		double d1 = (double) (rand()%20 + 0);
		d1 = 0;
		jobject pjd1 = rtc::createPJDCurve(p1, j1, d1, true );

		long p2 = rand()%1000 + 60;
		double j2 = (double) (rand()%(2*p2) + 1);
		double d2 = (double) (rand()%20 + 0);
		d2 = 0;
		jobject pjd2 = rtc::createPJDCurve(p2, j2, d2, true );


		bool flag = true;
		for (int j = 0; j < 10000; j+=50)
		{
			if (!test_rtc_plus(pjd2, pjd1, (double)j+1)){
				cout << "failed! p1 = " << p1 << ", j1 = " << j1 <<
				", d1 = " << d1 << ", p2 = " << p2 << ", j2 = "<< j2<< 
				", d2 = " << ", j = " << j << endl;
				flag = false;
				break;
			}
		}
		if (!flag){
			allflag = false;
			break;
		}
	}
	if(allflag){
		cout << "passed rtc plus test!" << endl;
	}
}