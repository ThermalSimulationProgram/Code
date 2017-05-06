#ifndef RTCCURVEDATASUBITERATOR_H
#define RTCCURVEDATASUBITERATOR_H

#include <vector>

double YatSegement(double x0, double y0, double s0, double x);

bool CurveChecker(const std::vector<double>&);

class RTCCurveDataIterator{
private:
	std::vector<double> c;
	
	int size;
    int pointer;
    double _xStart;
    double _xEnd;
    double _yStart;
    double _yEnd;
    double _s;
    double xMax;


    bool init;
    bool lastSegment;

public:
	RTCCurveDataIterator(const std::vector<double>& c, double _xmax);

	bool next();

	double xStart();
	double xEnd();
	double yStart();
	double yEnd();
	double s();

	void print();
	void display();
};


class RTCCurveDataSubIterator{
private:
	RTCCurveDataIterator a;
	RTCCurveDataIterator b;

    double _xStart;
    double _xEnd;
    double _yStartA;
    double _yEndA;
    double _yStartB;
    double _yEndB;

    bool init;
    bool aIsNew;
    bool bIsNew;

public:

	RTCCurveDataSubIterator(const std::vector<double>& a, const std::vector<double>& b, double xmax);

	bool next();

	double xStart();
	double xEnd();
	double yStartA();
	double yStartB();
	double yEndA();
	double yEndB();
	double sA();
	double sB();

	void print();
};



#endif