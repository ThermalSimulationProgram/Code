#ifndef _PARSER_H
#define _PARSER_H

#include "pugixml.hpp"
#include "structdef.h"

#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace pugi;
using namespace std;

template<typename T>
vector<T> getVector(string name){
	ifstream file (name);
	if (!file.is_open()){
		cerr << "getVector:: failed to open given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}

	std::string line;
	std::vector<T> ret;
	while(std::getline(file, line)){
		std::stringstream lineStream(line);
		std::string cell;
		while(std::getline(lineStream,cell,','))
			ret.push_back((T)stod(cell));
	}
	file.close();
	if (file.is_open()){
		cerr << "getVector:: failed to close given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}
	return ret;

}


template<typename T> void displayvector(std::vector<T> a, std::string name){

	std::cout << name<<": ";
	for (int i = 0; i < int(a.size()); ++i)
	{
		std::cout<< "  " << a[i] ;
	}
	std::cout<<std::endl<<"*************************"<<std::endl;
}

// This function converts a string to a double vector. The  string should be 
// in format: {number1, number2, ..., numbern} or {number1 number2 ... numbern}
vector<double> string2vector(string);


template<typename T> vector<T> parseTimeVectorMicro(xml_node n){
	vector<double> initvalues = string2vector(n.attribute("value").value());

	string units = n.attribute("units").value();
	vector<T> ret;
	for (unsigned i = 0; i < initvalues.size(); ++i){
		T v;
		if (units == "sec")
			v = (T) (initvalues[i]*1000000);
		else if (units == "ms")
			v = (T) (initvalues[i]*1000);
		else if (units == "us")
			v = (T) (initvalues[i]);
		else {
			cout << "parseTimeVectorMicro: Parser error: could not recognize time unit!\n";
			return ret;
		}
		ret.push_back(v);
	}
	return ret;	
}

vector<vector<double>> getMatrix(string name);


double getDouble(string name);

void saveDoubleVectorToFile(vector<double> data, string filename);

void saveDoubleVectorToFile2(vector<double> data, string filename);



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
	

	// This function is used for debugging. Not used in real program
	static pipeinfo loadPipeInfo(unsigned nstage);
	// This function is used for debugging. Not used in real program
	static vector<workerinfo> loadWorkerInfo(unsigned nstage);

};






#endif