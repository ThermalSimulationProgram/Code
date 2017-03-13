#include "FileOperator.h"


ifstream openInputFile(const string& filename){
	ifstream file (filename);
	if (!file.is_open()){
		cerr << "openInputFile:: failed to open given file named: '" << 
		filename << "'" << endl;
		exit(1);
	}
	return file;
}


ofstream openNewOutputFile(const string& filename){
	ofstream file;
  	file.open(filename, std::ofstream::out | std::ofstream::trunc);

	if (!file.is_open()){
		cerr << "openNewOutputFile:: failed to open given file named: '" << 
		filename << "'" << endl;
		exit(1);
	}
	return file;
}
ofstream openOutputFile(const string& filename){
	ofstream file;
  	file.open(filename, std::ofstream::out | std::ofstream::app);
  	
	if (!file.is_open()){
		cerr << "openOutputFile:: failed to open given file named: '" << 
		filename << "'" << endl;
		exit(1);
	}
	return file;
}

void closeFile(ifstream& file){
	file.close();
	if (file.is_open()){
		cerr << "closeFile:: failed to close given file named: '" << 
		endl;
		exit(1);
	}
}

void closeFile(ofstream& file){
	file.close();
	if (file.is_open()){
		cerr << "closeFile:: failed to close given file named: '" << 
		endl;
		exit(1);
	}
}



void changePermission(const string& filename){
	if (system(("chmod 666 " + filename ).data() )){
		cout << "changePermission: Failed to change permission of file "
		<< filename << endl;
	}
}
