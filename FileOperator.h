#ifndef _FILEOPERATOR_H
#define _FILEOPERATOR_H 

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include "utils.h"

using namespace std;

ifstream openInputFile(const string& filename);
ofstream openNewOutputFile(const string& filename);
ofstream openOutputFile(const string& filename);




void closeFile(ifstream& file);
void closeFile(ofstream& file);

void changePermission(const string& filename);

template<typename T>
vector<T> loadVectorFromFile(string filename){

	ifstream file = openInputFile(filename);
	
	std::string line;
	std::vector<T> ret;
	while(std::getline(file, line))
		appStringToVector(line, ret);
	
	closeFile(file);
	return ret;
}

template<typename T>
vector<vector<T> > loadMaxtrixFromFile(string filename){

	ifstream file = openInputFile(filename);
	
	std::string line;
	std::vector<vector<T> > maxtrix;
	while(std::getline(file, line)){
		std::vector<T> row;
		appStringToVector(line, row);

		maxtrix.push_back(row);
	}

	closeFile(file);
	return maxtrix;
}

template<typename T>
void dumpDataToFile(ofstream& file, const string& filename, vector<T> data){
	string out = vectorTostring(data);

	file << out << endl;

	closeFile(file);

	changePermission(filename);
}

template<typename T>
void saveToNewFile(const string& filename,  vector<T> data){
	ofstream file = openNewOutputFile(filename);
	dumpDataToFile(file, filename, data);
}

template<typename T>
void appendToFile(const string& filename,  vector<T> data){

	ofstream file = openOutputFile(filename);
	dumpDataToFile(file, filename, data);

}





























#endif