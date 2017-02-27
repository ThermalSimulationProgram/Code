#ifndef _VECTORMATHUTIL_H
#define _VECTORMATHUTIL_H 

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;



 vector<int> integerVector(int start, int end);
 vector<int> integerVector(int start, int end, int interval);

 bool vectorAny(const vector<bool> &);
 bool vectorAll(const vector<bool> &);


#endif