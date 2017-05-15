#include "RTCCurveDataSubIterator.h"

#include <cmath>
#include <iostream>

#include "utils.h"

using namespace std;

#define EPSILON (0.0000001)

#define _DEBUG 0

double YatSegement(double x0, double y0, double s0, double x){

	if (abs(x - x0) < EPSILON){
		return y0;
	}
	if (abs(s0) < EPSILON){
		return y0;
	}

	return y0 + s0 * (x - x0);
}

bool CurveChecker(const vector<double>& data){
	return CurveChecker(data, -EPSILON);
}
bool CurveChecker(const vector<double>& data, double initY){
	int numeldata = data.size();
	if (numeldata < 3 || numeldata%3 != 0){
		cout << "CurveChecker: data element number error" << endl;
		return false;
	}

	double previousY = 0.0;
	double previousX = -EPSILON;
	for (int i = 0; i < numeldata; i+=3)
	{
		if (data[i] < previousX + EPSILON){
			cout << "CurveChecker: x is not strictly increasing" << endl;
			return false;
		}
		if (data[i+1] <= previousY - EPSILON){
			cout << "CurveChecker: y is not wide increasing" << endl;
			return false;
		}
		previousX = data[i];
		previousY = data[i+1];
	}
	return true;
}

RTCCurveDataIterator::RTCCurveDataIterator(const std::vector<double>& _c, double _xmax):
c(_c){

	int numeldata = _c.size();
	if (numeldata < 3 || numeldata%3 != 0){
		cout << "RTCCurveDataIterator: data element number error" << endl;
		displayvector(_c, "CurveChecker:c");
		exit(1);
	}

	// if (!CurveChecker(_c)){
	// 	cerr<<"RTCCurveDataIterator: input curve vector error\n" ;
	// 	displayvector(_c, "CurveChecker:c");
	// 	exit(1);
	// }

	size = _c.size();
	// for (int i = 2; i < size; i+=3){
	// 	if (abs(_c[i]) > EPSILON){
	// 		cerr<<"RTCCurveDataIterator: only works for zero slope curves. Input has non-zero slope segments\n" ;
	// 		exit(1);
	// 	}
	// }

	if (xMax < _c[0]){
		cerr << "RTCCurveDataIterator: too small xMax!" << endl;
		exit(1);
	}

	xMax = _xmax;
	init = false;
	pointer = -1;
	lastSegment = false;
}


bool RTCCurveDataIterator::next(){
	if (lastSegment){
		return false;
	}

	if (!init){
		init = true;
		pointer = 0;
	}
	if (pointer >= size - 3 || c[pointer+3] > xMax){
		lastSegment = true;
	}
	#if _DEBUG == 1
	cout << "lastSegment: " << lastSegment << " pointer: " << pointer << " size: " << size << endl;
	#endif
	_xStart = c[pointer];
	if (!lastSegment){
		_xEnd = c[pointer + 3];
	}else{
		_xEnd = xMax;
	}

	_yStart = c[pointer+1];
	_s = c[pointer + 2];
	_yEnd = YatSegement(_xStart, _yStart, _s, _xEnd);

	if (!lastSegment){
		pointer += 3;
	}

	return true;
}

double RTCCurveDataIterator::xStart(){
	if (!init){
		cerr << "RTCCurveDataIterator: not initialized!" << endl;
		exit(1);
	}
	return _xStart;
}
double RTCCurveDataIterator::xEnd(){
	if (!init){
		cerr << "RTCCurveDataIterator: not initialized!" << endl;
		exit(1);
	}
	return _xEnd;
}
double RTCCurveDataIterator::yStart(){
	if (!init){
		cerr << "RTCCurveDataIterator: not initialized!" << endl;
		exit(1);
	}
	return _yStart;
}
double RTCCurveDataIterator::yEnd(){
	if (!init){
		cerr << "RTCCurveDataIterator: not initialized!" << endl;
		exit(1);
	}
	return _yEnd;
}
double RTCCurveDataIterator::s(){
	if (!init){
		cerr << "RTCCurveDataIterator: not initialized!" << endl;
		exit(1);
	}
	return _s;
}

void RTCCurveDataIterator::print(){
	bool flag;
	while(next()){
		display();
	}

}

void  RTCCurveDataIterator::display(){
	cout << "xStart: " << xStart() << ", yStart: " << yStart() << 
		", xEnd: " << xEnd() << ", yEnd: " << yEnd() << ", s: " << s() << endl;
}


RTCCurveDataSubIterator::RTCCurveDataSubIterator(
const std::vector<double>& _a, const std::vector<double>& _b, double xmax):
a(_a, xmax),  b(_b, xmax){
	init = false;

}


bool RTCCurveDataSubIterator::next(){

	if (!init){
		a.next();
		b.next();
		init = true;
		aIsNew = true;
		bIsNew = true;
		_xEnd = 0.0;

		#if _DEBUG == 1
		a.display();
		b.display();
		#endif

	}else{

		aIsNew = abs(_xEnd - a.xEnd()) < EPSILON ? a.next() : false;
		bIsNew = abs(_xEnd - b.xEnd()) < EPSILON ? b.next() : false;

		#if _DEBUG == 1
		a.display();
		b.display();
		cout << "aIsNew: " << aIsNew << " bIsNew: " << bIsNew << endl;
		#endif

		if (!aIsNew && !bIsNew){
			return false;
		}
	}
	


	_xStart = _xEnd;
	_xEnd = a.xEnd() < b.xEnd() ? a.xEnd() : b.xEnd();
	_yStartA = YatSegement(a.xStart(), a.yStart(), a.s(), _xStart);
	_yEndA 	= YatSegement(a.xStart(), a.yStart(), a.s(), _xEnd);

	_yStartB = YatSegement(b.xStart(), b.yStart(), b.s(), _xStart);
	_yEndB 	= YatSegement(b.xStart(), b.yStart(), b.s(), _xEnd);

	return true;

}



double RTCCurveDataSubIterator::xStart(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _xStart;
}
double RTCCurveDataSubIterator::xEnd(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _xEnd;
}
double RTCCurveDataSubIterator::yStartA(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _yStartA;
}
double RTCCurveDataSubIterator::yStartB(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _yStartB;
}
double RTCCurveDataSubIterator::yEndA(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _yEndA;
}
double RTCCurveDataSubIterator::yEndB(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return _yEndB;
}
double RTCCurveDataSubIterator::sA(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return a.s();
}
double RTCCurveDataSubIterator::sB(){
	if (!init){
		cerr << "RTCCurveDataSubIterator: not initialized!" << endl;
		exit(1);
	}
	return b.s();
}


void RTCCurveDataSubIterator::print(){
	while(next()) {
		cout << "xStart: " << xStart() << ", yStartA: " << yStartA() << ", yStartb: " << yStartB() <<
		", xEnd: " << xEnd() << ", yEndA: " << yEndA() << ", yEndB: " << yEndB()  << endl;
	}
}