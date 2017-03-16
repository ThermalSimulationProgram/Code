#include "UnitTesting.h"

#include <iostream>
#include <vector>

#include "APTMKernel.h"
#include "Parser.h"
#include "Enumerations.h"
#include "structdef.h"
#include "vectormath.h"
#include "Scratch.h"


using namespace std;



void testCalcAPTM(vector<double> &tons, vector<double>& toffs, unsigned i){
	Parser p = Parser("example_aptm.xml");
	p.parseFile();

	unsigned nstages = 4;
	vector<unsigned long> nousevector(2,10);
	APTMKernel aptmkernel = APTMKernel(nstages, Scratch::getDwcets(), 
		vector<double>(nstages, 2), 0.93, APTM, nousevector);

		
	aptmkernel.setOfflineData(Scratch::getOfflineData());

	 AdaptInfo config;
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
