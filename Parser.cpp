#include "Parser.h"
#include "Scratch.h"
#include "TimeUtil.h"
#include "Enumerations.h"
#include "warmingCurve.h"
#include "vectormath.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace pugi;
using namespace std;

Parser::Parser(string _xml_path){
	xml_path = _xml_path;
}

// This function parse the file pointed by xml_path, and then 
// save all necessary data required by the simulation in Scratch class.
int Parser::parseFile(){
	int ret = 0;

	// load the xml file
	xml_document doc;
	if( !doc.load_file(xml_path.data()) ){
		std::cout << "Could not find file...\n";
		return -1;
	}

	// get simulation name and duration
	xml_node sim_node      = doc.child("simulation");
	string name            = sim_node.attribute("name").value();
	// save the duration in microsecond unit
	unsigned long duration = parseTimeMircroSecond(
		sim_node.child("duration"));
	#if _DEBUG==1
	cout << "duration" << duration << endl;
	#endif

	// get pipeline stage number
	xml_node pipe_node = sim_node.child("pipeline");
	int nstage         = (int) stoul(
		pipe_node.attribute("stagenumber").value(), NULL, 0);

	// get job release times
	xml_node event_node     = sim_node.child("events");
	string arrival_csv_path = event_node.child("csv_path").attribute("value").value();
	string units            = event_node.child("csv_path").attribute("units").value();
	vector<unsigned long> rl_release_times;
	if(arrival_csv_path.length() > 0){
		vector<double> v = getVector<double> (arrival_csv_path);
		if (units == "sec")
			v = v*1000000;
		else if (units == "ms")
			v = v*1000;
		else if (units == "us")
			v = v;
		else {
			cout << "Parser error: could not recognize time unit!\n";
			return -1;
		}

		for (unsigned i = 0; i < v.size(); ++i){
			if( (i>0) && (v[i]< v[i-1])){			
				cout << "At lease on element in input time vector is less than its" <<\
				" previous and is modified to its previous value \n";
				v[i] = v[i-1];
			} 
			rl_release_times.push_back(v[i]);
		}
	}
	
	// get task parameteres: period, jitter, distance, and wcets
	unsigned long period        = parseTimeMircroSecond(
		event_node.child("period"));
	unsigned long jitter        = parseTimeMircroSecond(
		event_node.child("jitter"));
	unsigned long distance      = parseTimeMircroSecond(
		event_node.child("distance"));
	unsigned long rltDeadline   = parseTimeMircroSecond(
		event_node.child("relative_deadline"));
	vector<unsigned long> wcets = parseTimeVectorMicro<unsigned long>(
		event_node.child("wcets"));
	if ((int)wcets.size() != nstage){
		cout << "Parser error: wcetvalues' size doesn't match pipeline stage number\n";
		return -1;
	}
	
	// get the execution time factor
	double exe_factor = event_node.child("exe_factor").attribute("value").as_double();

	// get scheduler attributes
	xml_node schedule_node = sim_node.child("scheduler");
	xml_node kernel_node   = schedule_node.child("kernel");
	string kernel_type     = kernel_node.attribute("type").value();
	enum _schedule_kernel type = GE;
	if (kernel_type == "APTM")
		type = APTM;
	else if (kernel_type == "BWS")
		type = BWS;
	else if (kernel_type == "PBOO")
		type = PBOO;
	else if (kernel_type == "GE")
		type = GE;
	else{
		cout << "Parser error: could not recognize kernel type!\n";
		return -1;
	}

	// put all the necessary input parameters in scratch
	Scratch::initialize(nstage, period, jitter, distance, 
		rltDeadline, wcets, rl_release_times, type, duration, name);

	Scratch::setExeFactor(exe_factor);

	// handle additional parameters for the scheduler
	if(type == APTM){
		unsigned long adaptPeriod = parseTimeMircroSecond(kernel_node.child("period"));
		double b_factor 		  = kernel_node.child("b_factor").attribute("value").as_double();

		xml_node offline_node     = kernel_node.child("offlinedata");
		string prefix             = offline_node.child("prefix").attribute("path").value();
		thermalProp offlinedata   = getOfflineData(prefix, nstage);

		Scratch::setBFactor(b_factor);
		Scratch::setAdaptionPeriod(adaptPeriod);
		Scratch::setOfflineData(offlinedata);
	}else if (type == BWS){
		unsigned long adaptPeriod = parseTimeMircroSecond(kernel_node.child("period"));
		Scratch::setAdaptionPeriod(adaptPeriod);
	}else if (type == PBOO){
		ptmspec ptm;
		vector<unsigned long> tons  = parseTimeVectorMicro<unsigned long>(kernel_node.child("ton"));
		vector<unsigned long> toffs = parseTimeVectorMicro<unsigned long>(kernel_node.child("toff"));
		ptm.tons                    = tons/1000;  
		ptm.toffs                   = toffs/1000;
		Scratch::setPTMValues(ptm);
	}else if (type == GE){
		ptmspec ptm;
		ptm.tons  = vector<double>(nstage,1000);  
		ptm.toffs = vector<double>(nstage,0);
		Scratch::setPTMValues(ptm);
	}
	// Scratch::print();
	return ret;
}

// This function loads thermal property data of the processor from csv files
thermalProp Parser::getOfflineData(string prefix, unsigned nstage){
	thermalProp ret;
	ret.coolbreakToffs = getMatrix(prefix+"_coolBreakToffs.csv");
	ret.coolslopes     = getMatrix(prefix+"_coolslopes.csv");
	ret.numValidData   = getVector<int>(prefix+"_numValidData.csv");
	ret.allStageCurves = parseWarmingingCurve(prefix, nstage);
	return ret;
}


// This function loads warming curves of the processor from csv files
vector<warmingCurves> Parser::parseWarmingingCurve(string prefix, unsigned nstage){

	// first get the activation slope step
	unsigned slopestep = (unsigned) getDouble(prefix+"_slopestep.csv");

	vector<warmingCurves> ret;
	// load the warming curves for all stages
	for (unsigned i = 0; i < nstage; ++i)
	{
		// for each stage, the warmingCurves object constains x=floor(95/step) curves
		warmingCurves aStageCurves = warmingCurves(slopestep);

		stringstream name1, name2; 
		// load the data files for the warmingCurves object.
		// The format of prefix_warmingdata.csv:
		// toff11, toff12, toff13, ..., toff1n, toff21, toff22, ... toff2m, .... , toffx1,...,toffxt.
		// temp11, temp12, temp13, ..., temp1n, temp21, temp22, ... temp2m, .... , tempx1,...,tempxt.
		// slope11, slope12, ... slope1n, slope21, slope22, ... slope2m, slopex1, ..., slopext.
		// the element numbers in each curve, i.e., the n, m, and t above, are saved in prefix_warmingdataNumber.csv
		name1 << prefix << "_warmingdata" << i+1 << ".csv";
		name2 << prefix << "_warmingdataNumber" << i+1 << ".csv";


		// load the matrix representation of the two files
		vector<vector<double>> tmp  = getMatrix(name1.str());
		vector<unsigned> dataNumber = getVector<unsigned>(name2.str());

		// load the curve for each activation slope
		unsigned cursor = 0;
 		for (unsigned j = 0; j < dataNumber.size(); ++j){
			unsigned length       = dataNumber[j];
			vector<int> ids       = integerVector(cursor, cursor+length-1);
			vector<double> toffs  = vectorExtract(tmp[0], ids);
			vector<double> temps  = vectorExtract(tmp[1], ids);
			vector<double> slopes = vectorExtract(tmp[2], ids);
			// construct a linearSegs object
			linearSegs tmpSeg     = linearSegs();
			tmpSeg.setData(toffs, slopes, temps);

			aStageCurves.insert(tmpSeg);

			cursor += length;
		}

		ret.push_back(aStageCurves);
	}
	return ret;
}

// This function is used for debugging. Not used in real program
pipeinfo Parser::loadPipeInfo(unsigned nstage){
	pipeinfo ret;
	ret.Q         = getVector<int>("Q.csv");
	ret.activeSet = getVector<int>("activeSet.csv");
	ret.sleepSet  = getVector<int>("sleepSet.csv");
	ret.ccs       = getVector<double>("ccs.csv");
	ret.dcs       = getVector<double>("dcs.csv");
	ret.rho       = getVector<double>("rho.csv");
	ret.K         = getVector<double>("K.csv");
	ret.allT      = getVector<double>("allT.csv");

	vector<vector<double>> tmp;
	for (unsigned i = 0; i < nstage; ++i)
	{
		stringstream name1; 
		name1 << "FIFOcurveData" << i+1 << ".csv";
		vector<double> tmpstage = getVector<double>(name1.str());
		tmp.push_back(tmpstage);

	}

	ret.FIFOcurveData = tmp;
	return ret;

}

// This function is used for debugging. Not used in real program
vector<workerinfo> Parser::loadWorkerInfo(unsigned nstage){
	
	vector<workerinfo> ret;

	vector<int> allnfifijobs     = getVector<int>(
		"allnFIFOJobs.csv");
	vector<unsigned> allstates   = getVector<unsigned>(
		"allstates.csv");
	vector<int> allonGoEventIds  = getVector<int>(
		"allonGoEventIds.csv");
	vector<double> allexecuteds  = getVector<double>(
		"allexecuteds.csv");
	vector<double> allsleepTimes = getVector<double>(
		"allsleepTimes.csv");

	for (int i = 0; i < (int)nstage; ++i)
	{
		workerinfo tmp;
		tmp.stageId   = i;
		tmp.nFIFOJobs = allnfifijobs[i];
		tmp.executed  = allexecuteds[i];
		unsigned s    = allstates[i];

		if (s==0)
			tmp.state = _sleep;
		else
			tmp.state = _active;

		tmp.sleepTime   = allsleepTimes[i];
		
		tmp.onGoEventId = allonGoEventIds[i];

		stringstream name1;
		name1 << "allEventAbsDeadlines" << i+1 << ".csv";
		tmp.allEventAbsDeadlines = getVector<double>(name1.str());
		
		name1.str("");

		ret.push_back(tmp);

	}
	return ret;
}

// This function converts a string to a double vector. The  string should be 
// in format: {number1, number2, ..., numbern} or {number1 number2 ... numbern}
vector<double> string2vector(string sin){
	string base = sin;
	if (base.length() < 3)
	{
		cerr<<"string2vector:: Input string too short!"<<endl;
		exit(1);
	}
	std::replace(base.begin(), base.end(), '{', ' ');
	std::replace(base.begin(), base.end(), '}', ' ');
	std::replace(base.begin(), base.end(), ',', ' ');
	std::string::size_type sz;
	vector<double> result;
	try{
		do{
			result.push_back(stod(base, &sz));
			base = base.substr(sz);
		}while(base.length()>=2);
	}
	catch(...){
		cerr<<"Unknown error happens for string "<<sin<<endl;
		exit(1);
	}
	return result;
}


unsigned long Parser::parseTimeMircroSecond(xml_node n){
	struct timespec tmp = parseTime(n);
	return TimeUtil::convert_us(tmp);
}

struct timespec Parser::parseTime(xml_node n) {
	int time     = n.attribute("value").as_int();
	string units = n.attribute("units").value();
	struct timespec ret;

	if(units == "sec") {
		ret = TimeUtil::Seconds(time);
	}
	else if(units == "ms") {
		ret = TimeUtil::Millis(time);
	}
	else if(units == "us") {
		ret = TimeUtil::Micros(time);
	}
	else {
		cout << "Parser error: could not recognize time unit!\n";
	}

	return ret;
}

// read a csv file and save the numbers in a 2-D vector 
vector<vector<double>> getMatrix(string name){
	std::ifstream  data(name);
	if (!data.is_open()){
		cerr << "getMatrix:: failed to open given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}
	std::string line;
	std::vector<vector<double> > parsedCsv;
	while(std::getline(data,line))
	{
		std::stringstream lineStream(line);
		std::string cell;
		std::vector<double> parsedRow;
		while(std::getline(lineStream,cell,','))
			parsedRow.push_back(stod(cell));

		parsedCsv.push_back(parsedRow);
	}
	data.close();
	if (data.is_open()){
		cerr << "getMatrix:: failed to close given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}
	return parsedCsv;
}

// read a csv file and save the first numbers in double type
double getDouble(string name){
	ifstream file (name);
	if (!file.is_open()){
		cerr << "getDouble:: failed to open given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}
	double v; 
	file>>v;
	file.close();
	if (file.is_open()){
		cerr << "getDouble:: failed to close given file named: '" << 
		name << "'"  << endl;
		exit(1);
	}
	return v;
}

// This function saves a double vector to a file, appending new data to its existing data
void saveDoubleVectorToFile(vector<double> data, string filename){
  ofstream file;
  file.open((filename + ".csv").data() , std::ofstream::out | std::ofstream::app);
  if (!file.is_open()){
		cerr << "saveDoubleVectorToFile:: failed to open given file named: '" << 
		filename << "'"  << endl;
		exit(1);
	}
  stringstream out;
  for(unsigned int c=0; c< data.size();c++) {
    if ( c != data.size()-1 )
      out << data[c] << "," ;
    else
      out << data[c];

  }
  file << out.str() << endl;
  file.close();
  if (file.is_open()){
		cerr << "saveDoubleVectorToFile:: failed to close given file named: '" << 
		filename << "'"  << endl;
		exit(1);
	}

  //Change the owner and permissions of generated files
  //system(("chown hsf:hsf " + filePrefix + "_*.csv").data() );
  if (system(("chmod 666 " + filename + ".csv").data() )){
    cout << "saveDoubleVectorToFile:: error changing file permissions " << endl;
  }

}


// This function saves a double vector to a file, overwriting existing data
void saveDoubleVectorToFile2(vector<double> data, string filename){
  ofstream file;

  /************ SAVING _temprature_trace *********/
  file.open((filename + ".csv").data(), std::ofstream::out | std::ofstream::trunc);
  stringstream out;
  for(unsigned int c=0; c< data.size();c++) {

    if ( c != data.size()-1 )
      out << data[c] << "," ;
    else
      out << data[c];

  }
  file << out.str() << endl;
  file.close();
  //Change the owner and permissions of generated files
  //system(("chown hsf:hsf " + filePrefix + "_*.csv").data() );
  if (system(("chmod 666 " + filename + ".csv").data() ))
  {
    cout << "error saving file " << endl;
  }

}




