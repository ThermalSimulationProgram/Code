#ifndef _FILEOPERATOR_H
#define _FILEOPERATOR_H 

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include "utils.h"


#define FSTREAM_IN (0xF)
#define FSTREAM_OUT (0x0)
#define FSTREAM_TRUNC (0x1)
#define FSTREAM_APP (0x2)

using namespace std;


class FileOperator{
private:
	ifstream iFile;
	ofstream oFile;

	bool is_input_active;

	bool isOpen;

	bool isSaved;

	string filename;

	vector<string> content;

	int mode;

public:
	FileOperator(const string&, int);
	FileOperator(const FileOperator&);
	~FileOperator();

	double readDouble();
	
	template<typename T> 
	vector<T> readVector();

	template<typename T>
	vector<vector<T>> readMatrix();

	void write(const string &);

	void write(const vector<string>& );

	void save();

	void close();

	void changePermission();
	void changePermission(const string&);
};

template<typename T> 
vector<T> FileOperator::readVector(){
	vector<T> ret;
	if ((!is_input_active) && isOpen)
		return ret;
	for (int i = 0; i < (int) content.size(); ++i)
		appStringToVector(content[i], ret);

	return ret;
}

template<typename T>
vector<vector<T>> FileOperator::readMatrix(){
	vector<vector<T>> ret;
	if ((!is_input_active) && isOpen)
		return ret;
	ret.reserve( content.size());
	for (int i = 0; i < (int) content.size(); ++i)
		ret.push_back(stringToVector<T>(content[i]));

	return ret;
}

// read a csv file and save the first numbers as double
double loadDoubleFromFile(const string  filename);


template<typename T>
vector<T> loadVectorFromFile(const string  filename){

	FileOperator file = FileOperator(filename, (int)FSTREAM_IN);
	
	return file.readVector<T>();
}

template<typename T>
vector<vector<T> > loadMatrixFromFile(const string filename){
	FileOperator file = FileOperator(filename, (int)FSTREAM_IN);
	
	return file.readMatrix<T>();
}

void saveContentToNewFile(const string filename, const vector<string>&data);
void appendContentToFile(const string filename, const vector<string>&data);

template<typename T>
void saveToNewFile(const string filename,  const vector<T>& data){
	saveContentToNewFile(filename, vector<string>(1, vectorTostring(data)));
}

template<typename T>
void saveToNewFile(const string filename,  const vector<vector<T>>& data){
	saveContentToNewFile(filename, matrixTostring(data));
}


template<typename T>
void appendToFile(const string filename,  const vector<T>& data){
	appendContentToFile(filename, vector<string>(1, vectorTostring(data)));
}

template<typename T>
void appendToFile(const string filename,  const vector<vector<T>>&data){
	appendContentToFile(filename, matrixTostring(data));
}







#endif