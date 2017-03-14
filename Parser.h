#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "pugixml.hpp"

#include "structdef.h"
#include "utils.h"


using namespace pugi;
using namespace std;



class Parser{
private:
	string xml_path;
public:
	Parser(string);

	// This function parse the file pointed by xml_path, and then 
	// save all necessary data required by the simulation in Scratch class.
	int parseFile();

	struct timespec parseTime(xml_node n);
	unsigned long parseTimeMircroSecond(xml_node n);

		
	// This function loads warming curves of the processor from csv files
	static vector<warmingCurves> parseWarmingingCurve(string,
		unsigned);

	// This function loads thermal property data of the processor from csv files
	static thermalProp getOfflineData(string, unsigned);
	
};


template<typename T> T formatTimeMicros(double v, string unit){
	T r;
	if (unit == "sec")
		r = (T) (v*1000000);
	else if (unit == "ms")
		r = (T) (v*1000);
	else if (unit == "us")
		r = (T) (v);
	else {
		cout << "parseTimeVectorMicro: Parser error: could not recognize time unit!\n";
		r = -1;
	}
	return r;
}

template<typename T> vector<T> formatTimeMicros(const vector<double>& v, string unit){
	vector<T> ret;
	for (int i = 0; i < (int) v.size(); ++i)
		ret.push_back(formatTimeMicros<T>(v[i], unit));

	return ret;
}


template<typename T> vector<T> parseTimeVectorMicro(xml_node n){
	vector<double> initvalues = stringToVector<double>(n.attribute("value").value());

	string unit = n.attribute("units").value();
	vector<T> ret = formatTimeMicros<T>(initvalues, unit);

	return ret;	
}


// This function is used for debugging. Not used in real program
pipeinfo loadPipeInfo(unsigned nstage);
// This function is used for debugging. Not used in real program
vector<workerinfo> loadWorkerInfo(unsigned nstage);




#endif