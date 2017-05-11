#include "Parser.h"

#include <iostream>
#include <string>
#include <algorithm>


#include "Scratch.h"
#include "TimeUtil.h"
#include "Enumerations.h"
#include "warmingCurve.h"
#include "vectormath.h"
#include "FileOperator.h"



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
	unsigned long duration = parseTimeMircroSecond(sim_node.child("duration"));
	
	// get pipeline stage number
	xml_node pipe_node     = sim_node.child("pipeline");
	int nstage             = (int) stoul(pipe_node.attribute("stagenumber").value(), NULL, 0);

	// get job release times
	vector<unsigned long> rl_release_times;
	xml_node event_node     = sim_node.child("events");

	// get task parameteres: period, jitter, distance, and wcets
	unsigned long period ;
	unsigned long jitter ;
	unsigned long distance ;
	unsigned long rltDeadline ;
	vector<unsigned long> wcets;
	// get the execution time factor
	double exe_factor ;

	parseTask( event_node, rl_release_times, period, jitter, distance,
	 rltDeadline, wcets, exe_factor, nstage );	

	// get scheduler attributes
	xml_node schedule_node = sim_node.child("scheduler");
	xml_node kernel_node   = schedule_node.child("kernel");
	string kernel_type     = kernel_node.attribute("type").value();
	enum _schedule_kernel type = GE;
	if (kernel_type == "APTM")
		type = APTM;
	else if (kernel_type == "BWS")
		type = BWS;
	else if (kernel_type == "PBOO" || kernel_type == "JOUR")
		type = PBOO;
	else if (kernel_type == "GE")
		type = GE;
	else if (kernel_type == "SAPTM")
		type = SAPTM;
	else if (kernel_type == "CS")
		type = CS;
	else{
		cout << "Parser error: could not recognize kernel type!\n";
		return -1;
	}

	// put all the necessary input parameters in scratch
	Scratch::initialize(nstage, period, jitter, distance, 
		rltDeadline, wcets, rl_release_times, type, duration, name);

	Scratch::setExeFactor(exe_factor);



	// get parameters of additional tasks
	xml_node moreevent_node     = sim_node.child("moreevents");
	//iterate through all of the children nodes
	if (moreevent_node){
		for (xml_node newevent = moreevent_node.first_child(); newevent; newevent = newevent.next_sibling()){

			parseTask( newevent, rl_release_times, period, jitter, distance,
				rltDeadline, wcets, exe_factor, nstage );

			Scratch::addTask(period, jitter, distance, rltDeadline, wcets, rl_release_times);
		}
	}
	
	xml_node isSaveFile	   = sim_node.child("save_result");
	if (isSaveFile){

		string isSave 		=  isSaveFile.attribute("value").value();
		if ((isSave == "false") || (isSave == "False")){
			Scratch::setSavingFile(false);
		}else if ((isSave == "true") || (isSave == "True")){
			Scratch::setSavingFile(true);
		}else{
			cout << "Parser warning: parameter of saving result error! set to default TRUE value\n";
		}
	}

	xml_node isSaveTempTrace	   = sim_node.child("save_temperature_trace");
	if (isSaveTempTrace){

		string isSave 		=  isSaveTempTrace.attribute("value").value();
		if ((isSave == "false") || (isSave == "False")){
			Scratch::isSaveTemperatureTrace = false;
		}else if ((isSave == "true") || (isSave == "True")){
			Scratch::isSaveTemperatureTrace = true;
		}else{
			cout << "Parser warning: parameter of saving temperature trace error! set to default FALSE value\n";
		}
	}

	xml_node benchmark = sim_node.child("benchmark");
	if(benchmark){
		string benchmark_name = benchmark.attribute("name").value();
		Scratch::setBenchmark(benchmark_name);
	}
	

	// handle additional parameters for the scheduler
	if(type == APTM){
		xml_node APeriodNode  = kernel_node.child("period");
		xml_node b_factorNode = kernel_node.child("b_factor");
		xml_node offline_node = kernel_node.child("offlinedata");

		if(!APeriodNode){
			cout << "Parser::parseFile(): adaption period is required for APTM kernel\n";
			return -1;
		}
		if(!offline_node){
			cout << "Parser::parseFile(): offline data is required for APTM kernel\n";
			return -1;
		}
		unsigned long adaptPeriod = parseTimeMircroSecond(APeriodNode);
		string prefix             = offline_node.child("prefix").attribute("path").value();
		thermalProp offlinedata   = getOfflineData(prefix, nstage);

		
		Scratch::setAdaptionPeriod(adaptPeriod);
		Scratch::setOfflineData(offlinedata);

		if(b_factorNode){
			double b_factor   	= b_factorNode.attribute("value").as_double();
			Scratch::setBFactor(b_factor);
		}
		
	}else if (type == BWS || type == SAPTM){
		unsigned long adaptPeriod = parseTimeMircroSecond(kernel_node.child("period"));
		Scratch::setAdaptionPeriod(adaptPeriod);
	}else if (type == PBOO){
		ptmspec ptm;
		vector<unsigned long> tons  = parseTimeVectorMicro<unsigned long>(kernel_node.child("ton"));
		vector<unsigned long> toffs = parseTimeVectorMicro<unsigned long>(kernel_node.child("toff"));
		ptm.tons                    = tons/1000;  
		ptm.toffs                   = toffs/1000;
		Scratch::setPTMValues(ptm);
		Scratch::setAdaptionPeriod(0);
	}else if (type == GE){
		ptmspec ptm;
		ptm.tons  = vector<double>(nstage,1000);  
		ptm.toffs = vector<double>(nstage,0);
		Scratch::setPTMValues(ptm);
		Scratch::setAdaptionPeriod(0);
	}else if (type == CS){
		vector<double> leakyBucket_b = stringToVector<double>(
			kernel_node.child("leaky_bucket_b").attribute("value").value());
		vector<double> leakyBucket_r = stringToVector<double>(
			kernel_node.child("leaky_bucket_r").attribute("value").value());

		if (minElement(leakyBucket_b) < -0.00000001 || minElement(leakyBucket_r) <= 0.00000001){
			cerr << "Parser::parseFile: leaky bucket parameters for cool shaper kernel must be nonnegative!" << endl;
			exit(1);
		}
		if (leakyBucket_b.size() != leakyBucket_r.size()){
			cerr << "Parser::parseFile: leaky bucket b must be the same size with leaky bucket r!" << endl;
			exit(1);
		}
		struct timespec wunit = parseTime(kernel_node.child("Wunit"));

		double Wunit = (double) TimeUtil::convert_ms(wunit);

		Scratch::leakyBucket_r = leakyBucket_r;
		Scratch::leakyBucket_b = leakyBucket_b;
		Scratch::Wunit = Wunit;
	}
	// Scratch::print();
	return ret;
}

// This function loads thermal property data of the processor from csv files
thermalProp Parser::getOfflineData(string prefix, unsigned nstage){
	thermalProp ret;
	ret.coolbreakToffs = loadMatrixFromFile<double>(prefix+"_coolBreakToffs.csv");
	ret.coolslopes     = loadMatrixFromFile<double>(prefix+"_coolslopes.csv");
	ret.numValidData   = loadVectorFromFile<int>(prefix+"_numValidData.csv");
	ret.allStageCurves = parseWarmingingCurve(prefix, nstage);
	return ret;
}


// This function loads warming curves of the processor from csv files
vector<warmingCurves> Parser::parseWarmingingCurve(string prefix, unsigned nstage){

	// first get the activation slope step
	unsigned slopestep = (unsigned) loadDoubleFromFile( prefix+"_slopestep.csv");

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
		vector<vector<double>> tmp  = loadMatrixFromFile<double>(name1.str());
		vector<unsigned> dataNumber = loadVectorFromFile<unsigned>(name2.str());

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



unsigned long parseTimeMircroSecond(xml_node n){
	struct timespec tmp = parseTime(n);
	return TimeUtil::convert_us(tmp);
}

struct timespec parseTime(xml_node n) {
	int time     = n.attribute("value").as_int();
	if (time < 0){
		cerr << "parseTime: time value must be nonnegative!" << endl;
		exit(0);
	}
	string units = n.attribute("unit").value();
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
		exit(1);
	}

	return ret;
}


// This function is used for debugging. Not used in real program
pipeinfo loadPipeInfo(unsigned nstage){
	pipeinfo ret;
	ret.Q         = loadVectorFromFile<int>("Q.csv");
	ret.activeSet = loadVectorFromFile<int>("activeSet.csv");
	ret.sleepSet  = loadVectorFromFile<int>("sleepSet.csv");
	ret.ccs       = loadVectorFromFile<double>("ccs.csv");
	ret.dcs       = loadVectorFromFile<double>("dcs.csv");
	ret.rho       = loadVectorFromFile<double>("rho.csv");
	ret.K         = loadVectorFromFile<double>("K.csv");
	ret.allT      = loadVectorFromFile<double>("allT.csv");

	vector<vector<double>> tmp;
	for (unsigned i = 0; i < nstage; ++i){
		stringstream name1; 
		name1 << "FIFOcurveData" << i+1 << ".csv";
		vector<double> tmpstage = loadVectorFromFile<double>(name1.str());
		tmp.push_back(tmpstage);

	}

	ret.FIFOcurveData = tmp;
	return ret;
}

// This function is used for debugging. Not used in real program
vector<WorkerInfo> loadWorkerInfo(unsigned nstage){
	
	vector<WorkerInfo> ret;

	vector<int> allnfifijobs     = loadVectorFromFile<int>(
		"allnFIFOJobs.csv");
	vector<unsigned> allstates   = loadVectorFromFile<unsigned>(
		"allstates.csv");
	vector<int> allonGoEventIds  = loadVectorFromFile<int>(
		"allonGoEventIds.csv");
	vector<double> allexecuteds  = loadVectorFromFile<double>(
		"allexecuteds.csv");
	vector<double> allsleepTimes = loadVectorFromFile<double>(
		"allsleepTimes.csv");

	for (int i = 0; i < (int)nstage; ++i)
	{
		WorkerInfo tmp;
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
		tmp.allEventAbsDeadlines = loadVectorFromFile<double>(name1.str());
		
		name1.str("");

		ret.push_back(tmp);

	}
	return ret;
}


void parseTask(xml_node event_node, vector<unsigned long>&rl_release_times,
unsigned long& period, unsigned long& jitter, unsigned long& distance,
unsigned long& rltDeadline, vector<unsigned long>& wcets,
double& exe_factor, int nstage ){

	rl_release_times.clear();

	string arrival_csv_path = event_node.child("csv_path").attribute("value").value();
	string unit             = event_node.child("csv_path").attribute("unit").value();
	
	if(arrival_csv_path.length() > 0 && unit.length() > 0){
		vector<double> initv = loadVectorFromFile<double> (arrival_csv_path);
		vector<double> v     = formatTimeMicros<double>(initv, unit);

		for (unsigned i = 0; i < v.size(); ++i){
			if( (i>0) && (v[i]< v[i-1])){			
				cout << "At lease on element in input time vector is less than its" <<
				" previous and is modified to its previous value \n";
				v[i] = v[i-1];
			} 
			rl_release_times.push_back(v[i]);
		}
	}
	
	// get task parameteres: period, jitter, distance, and wcets
	period        = parseTimeMircroSecond(
		event_node.child("period"));
	jitter        = parseTimeMircroSecond(
		event_node.child("jitter"));
	distance      = parseTimeMircroSecond(
		event_node.child("distance"));
	rltDeadline   = parseTimeMircroSecond(
		event_node.child("relative_deadline"));
	wcets = parseTimeVectorMicro<unsigned long>(
		event_node.child("wcets"));
	if ((int)wcets.size() != nstage){
		cout << "Parser error: wcetvalues' size doesn't match pipeline stage number\n";
		exit(1);
	}
	
	// get the execution time factor
	exe_factor = event_node.child("exe_factor").attribute("value").as_double();
}