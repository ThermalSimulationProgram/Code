#ifndef _VECTORMATH_H
#define _VECTORMATH_H


#include "vectormath_util.h"
#include "vectormath_calc.h"
#include "vectormath_prop.h"
#include "vectormath_access.h"


template <typename T>
vector<int> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<int> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](int i1, int i2) {return v[i1] > v[i2];});
     return idx;
}



template<typename T> vector<T> singlesVector(unsigned _length, T sample){
	vector<T> ret(_length, sample);
	return ret;
}





#endif