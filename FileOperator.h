#ifndef _FILEOPERATOR_H
#define _FILEOPERATOR_H 

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include "utils.h"

using namespace std;

ifstream openFile(const string& name);
void closeFile(ifstream& file);



template<typename T>
vector<T> loadVectorFromFile(string name){

	ifstream file = openFile(name);
	
	std::string line;
	std::vector<T> ret;
	while(std::getline(file, line))
		appStringToVector(line, ret);
	
	closeFile(file);
	return ret;
}

template<typename T>
vector<vector<T> > loadMaxtrixFromFile(string name){

	ifstream file = openFile(name);
	
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


























#endif