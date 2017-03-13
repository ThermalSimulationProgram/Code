#ifndef _UTILS_H 
#define _UTILS_H 


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

string formatString(const string& base);
istringstream stringToiss(const string& base);

template<typename T> 
std::vector<T> stringToVector(std::string const& pointLine)
{
	std::istringstream iss = stringToiss(pointLine);

	return std::vector<T>{ 
		std::istream_iterator<T>(iss),
		std::istream_iterator<T>()
	};
}

template<typename T> 
void appStringToVector(std::string const& pointLine, vector<T>& data)
{
	std::istringstream iss = stringToiss(pointLine);

	data.insert(data.end(), std::istream_iterator<T>(iss), 
		std::istream_iterator<T>());
}

template<typename T> 
void dumpVector(std::vector<T> a){
	for (int i = 0; i < int(a.size()); ++i){
		std::cout << a[i] << "  "  ;
	}
	std::cout<<std::endl<<"*************************"<<std::endl;
}





#endif