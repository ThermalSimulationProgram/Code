#include "vectormath_util.h"






vector<int> integerVector(int start, int end){
	vector<int> ret;
	if (end < start)
		return ret;
	ret.reserve(end-start+1);
	for (int i = start; i <= end; ++i)
		ret.push_back(i);
	
	return ret;
}

 vector<int> integerVector(int start, int end, 
 	int interval){
 	vector<int> ret;
 	if (end < start)
		return ret;
	ret.reserve(end-start+1);
	for (int i = start; i <= end; i = i+interval)
		ret.push_back(i);
	
	return ret;
 }

bool vectorAny(const vector<bool> & a){
	for (int i = 0; i < int(a.size()); ++i){
		if (a[i])
			return true;
	}
	return false;
}

bool vectorAll(const vector<bool> &a){
	for (int i = 0; i < int(a.size()); ++i){
		if (!a[i])
			return false;
	}
	return true;
}