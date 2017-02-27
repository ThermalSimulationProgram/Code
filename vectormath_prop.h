#ifndef _VECTORMATHPROP_H
#define _VECTORMATHPROP_H

#include "vectormath_util.h"

#include <vector>
//#include <math.h>
#include <iostream>
#include <algorithm>

using namespace std;


template<typename T> T maxORminCore(const vector<T> & a, 
const vector<int> &ids,
bool ismax, int* id){

	int asize = (int) a.size();
 	if(ids.size() == 0 ){
 		cerr << "vectormath::maxORminCore: input index is empty. ismax = " << ismax << endl;
 		exit(1);
 		
 	}
 	if(ids[0] >= asize || ids[0] < 0){
 		cerr<<"vectormath::maxORminCore: Incorrect first element index\n";
 		exit(1);
	}

	T ret = a[ids[0]];
	int _id = ids[0];
	if (ismax){
		for (unsigned i = 1; i < ids.size(); ++i){
			if(ids[i] >= asize){
				cerr<<"vectormath::maxORminCore: Incorrect element index\n";
				exit(1);
			}
			if(ids[i] < 0){
				cerr<<"vectormath::maxORminCore: Negative element index\n";
				exit(1);
			}
			if(a[ids[i]] > ret){
				ret = a[ids[i]];
				_id = ids[i];
			}
		}
	}else{
		for (unsigned i = 1; i < ids.size(); ++i){
			if(ids[i] >= asize){
				cerr<<"vectormath::maxORminCore: Incorrect element index\n";
				exit(1);
			}
			if(a[ids[i]] < ret){
				ret = a[ids[i]];
				_id = ids[i];
			}
		}
	} 
	if(id != NULL)
		*id = _id ;
	return ret;

}
template<typename T> T maxORminCore(const vector<T>& a, 
const vector<int> & ids,
bool ismax){
	return maxORminCore(a, ids, ismax, NULL);
}


template<typename T> T vecsumcore(const vector<T> & a, 
const vector<int>& ids){
	int asize = (int)a.size();
	T ret = 0;
	for (unsigned i = 0; i < ids.size(); ++i){
		if(ids[i] >= asize){
			cerr<<"vectormath::vecsumcore: Incorrect element index\n";
			exit(1);
		}
		if(ids[i] < 0){
			cerr<< "vectormath::vecsumcore: Negative element index" << endl;
			exit(1);
		}
		ret += a[ids[i]];
	}
	
	return ret;
}

template<typename T> T vecsum(const vector<T>& a){
	vector<int> ids = integerVector(0, a.size()-1);
	return vecsumcore(a, ids);
}

template<typename T> T vecsum(const vector<T>& a, int s,
int e){
	vector<int> ids = integerVector(s, e);
	return vecsumcore(a, ids);
}


template<typename T> vector<int> equalIndex(const vector<T> &a, T b){
	vector<int> ret;
	for (int i = 0; i < (int)a.size(); ++i){
		if(a[i] == b)
			ret.push_back(i);
	}
	return ret;
}

template<typename T> T minElement(const vector<T>&a, int s, 
int e, int *id){
	vector<int> ids = integerVector(s, e);
	return maxORminCore(a, ids, false, id);
}


template<typename T> T minElement(const vector<T> & a, int s, 
int e){
	return minElement(a, s, e, NULL);
}



template<typename T> T minElement(const vector<T> & a, int* id){
	return minElement(a, 0, a.size()-1, id);
}

template<typename T> T minElement(const  vector<T> & a){
	return minElement(a, 0, a.size()-1, NULL);
}


template<typename T, typename T1> vector<T> minElement(const vector<T>& a, 
const vector<T1> &b){
	if (a.size() != b.size()){
		cerr << "vectormath::minElement:input vectors dimensions must agree!";
		exit(1);
	}
	vector<T> ret;
	for (int i = 0; i <(int) a.size(); ++i)
		ret.push_back((double)a[i]>b[i]?b[i]:a[i]);
	
	return ret;
}





template<typename T> T maxElement(const vector<T> & a, int s, 
int e, int *id){
	vector<int> ids = integerVector(s, e);
	return maxORminCore(a, ids, true, id);
}

template<typename T> T maxElement(const vector<T>& a, int s, 
int e){
	return maxElement(a, s, e, NULL);
}

template<typename T> T maxElement(const vector<T>& a, int *id){
	return maxElement(a, 0, a.size()-1, id);
}

template<typename T> T maxElement(const vector<T>& a){
	return maxElement(a, 0, a.size()-1, NULL);
}

template<typename T> T maxElement(
const vector<vector<T>> & a){
	T ret = maxElement(a[0]);
	for (unsigned i = 1; i < a.size(); ++i){
		double tmp = maxElement(a[i]);
		if(ret < tmp)
			ret = tmp;
	}
	return ret;
}


template<typename T> vector<bool> vecIsInf(const vector<T> & a){
	vector<bool> ret;
	for (int i = 0; i < (int)a.size(); ++i)
		ret.push_back(isinf((long double)a[i]));
	
	return ret;
}

template<typename T> vector<vector<T>> vecAbs(const vector<vector<T>> & a){
	vector<vector<T>> ret = a;
	for (int i = 0; i < (int)a.size(); ++i){
		for (int j = 0; j < (int) a[i].size(); ++j){
			ret[i][j] = abs(ret[i][j]);
		}
	}
	return ret;
}

template<typename T> vector<T> vecAbs(const vector<T> & a){
	vector<T> ret = a;
	ret.reserve(a.size());
	for (int i = 0; i < (int)a.size(); ++i)
			ret[i] = abs(ret[i]);
	return ret;
}



template<typename T> vector<bool> vecNot(const vector<T> & a){
	vector<bool> ret;
	for (int i = 0; i < (int)a.size(); ++i)
		ret.push_back(!(bool)(a[i]));
	
	return ret;
}

template<typename T> vector<int> vecFind(const vector<T> & a){
	vector<int> ret;
	for (int i = 0; i < (int)a.size(); ++i){
		if (a[i])
			ret.push_back(i);
	}
	return ret;
}





#endif
