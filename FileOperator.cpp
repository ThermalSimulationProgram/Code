#include "FileOperator.h"


ifstream openFile(const string& name){
	ifstream file (name);
	if (!file.is_open()){
		cerr << "getVector:: failed to open given file named: '" << 
		name << "'" << endl;
		exit(1);
	}
	return file;
}
void closeFile(ifstream& file){
	file.close();
	if (file.is_open()){
		cerr << "getVector:: failed to close given file named: '" << 
		endl;
		exit(1);
	}
}
