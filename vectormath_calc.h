#ifndef _VECTORMATHCALC_H
#define _VECTORMATHCALC_H

#include "vectormath_util.h"


#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

enum _vector_operator{
	__voplus,
	__vominus,
	__votimes,
	__vodivide,
	__volarger,
	__voless,
	__voequal,
	__volesseq,
	__volargereq,
	__vonoteq,
	__voand,
	__voor,
};



template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __plus(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = *__a + (*__b);
} 


template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __minus(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = *__a - (*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __times(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = *__a * (*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __divide(_Iter __a, _Iter2 __b, _Iter3 __c){
if (abs((*__b)) <= 0.000001){
	cout << "__a:" << *__a << endl;
	cout << "__b:" << *__b << endl;
	cerr << "vectormath::__divided:divided by zero\n";
	exit(1);
}
	*__c = (double)*__a / (*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __largerthan(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = (double)(*__a) > (double)(*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __lessthan(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = (double)(*__a) < (double)(*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __equalwith(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = abs((double)(*__a) - (double)(*__b)) < 0.000001;
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __lessequal(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = ((double)(*__a) < (double)(*__b)) || 
	(abs((double)(*__a) - (double)(*__b)) < 0.000001);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __largerequal(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = ((double)(*__a) > (double)(*__b)) || 
	(abs((double)(*__a) - (double)(*__b)) < 0.000001);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __noteq(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = (abs((double)(*__a) - (double)(*__b)) >= 0.000001);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __andop(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = *__a && (*__b);
}

template<typename _Iter, typename _Iter2, typename _Iter3> 
inline void __orop(_Iter __a, _Iter2 __b, _Iter3 __c){
	*__c = *__a || (*__b);
}


template<typename _Iter, typename _Iter2, typename _Iter3> 
 void __vectorsOperationCore(
_Iter __start1, _Iter __end1, _Iter2 __start2, _Iter2 __end2, 
enum _vector_operator __operation, _Iter3 __result){
	if (std::distance(__start1, __end1) != 
		std::distance(__start2, __end2)){
		cout << "error while trying doing operation: " << __operation << endl;
		cerr << "vectormath::vectorOperationCore:vector dimensions does not match\n";
		exit(1);
	}
	_Iter __temp1 =__start1; 
	_Iter2 __temp2 =__start2;
	switch (__operation){
		case  __vominus:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__minus(__temp1, __temp2, __result);
		}
		case __voplus:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__plus(__temp1, __temp2, __result);
		}
		case __votimes:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__times(__temp1, __temp2, __result);
		}
		case __vodivide:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__divide(__temp1, __temp2, __result);
		}
		case __volarger:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__largerthan(__temp1, __temp2, __result);
		}
		case __voless:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__lessthan(__temp1, __temp2, __result);
		}
		case __voequal:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__equalwith(__temp1, __temp2, __result);
		}
		case __volesseq:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__lessequal(__temp1, __temp2, __result);
		}
		case __volargereq:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__largerequal(__temp1, __temp2, __result);
		}
		case __vonoteq:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__noteq(__temp1, __temp2, __result);
		}
		case __voand:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__andop(__temp1, __temp2, __result);
		}
		case __voor:{
			for (; __temp1!=__end1; ++__temp1, ++__temp2,++__result)
				__orop(__temp1, __temp2, __result);
		}
	}
		
}

template<typename _Iter, typename _T1, typename _Iter2> 
 void __vectorScalarOperationCore(
_Iter __start, _Iter __end, _T1 __b, enum _vector_operator __operation, 
_Iter2 __result){
	vector<_T1> temp(std::distance(__start, __end), __b);
	__vectorsOperationCore(__start, __end, temp.begin(),
		temp.end(), __operation, __result);
}

template<typename _Iter, typename _T1, typename _Iter2> 
 void __vectorScalarOperationCore(
_T1 __b, _Iter __start, _Iter __end, enum _vector_operator __operation, 
_Iter2 __result){
	vector<_T1> temp(std::distance(__start, __end), __b);
	__vectorsOperationCore(temp.begin(), temp.end(), __start, 
		__end,  __operation, __result);
}


template<typename _T, typename _T1> 
vector<_T> operator -(const vector<_T> &__a, _T1 __b ){
	vector<_T> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__vominus, back_inserter(ret));
	return ret;
}
template<typename _T, typename _T1> 
vector<_T> operator -(_T1 __b, const vector<_T> &__a){
	vector<_T> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__vominus, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<_T> operator -(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<_T> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __vominus, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<_T> operator +(const vector<_T> & __a, _T1 __b ){
	vector<_T> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__voplus, back_inserter(ret));
	return ret;
	// return __a;
}

template<typename _T, typename _T1> 
vector<_T> operator +(_T1 __b, const vector<_T> &__a){
	vector<_T> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__voplus, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<_T> operator +(const vector<_T> & __a, const vector<_T1> &__b ){
	vector<_T> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __voplus, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<double> operator *(const vector<_T> &__a, _T1 __b ){
	vector<double> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__votimes, back_inserter(ret));
	return ret;
	// return __a;
}
template<typename _T, typename _T1> 
vector<double> operator *(_T1 __b, const vector<_T> &__a){
	vector<double> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__votimes, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<double> operator *(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<double> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __votimes, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<double> operator /(const vector<_T> &__a, _T1 __b){
	vector<double> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__vodivide, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<double> operator /(_T1 __b, const vector<_T> &__a){
	vector<double> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__vodivide, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<double> operator /(const vector<_T> &__a, const vector<_T1> &__b){
	vector<double> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __vodivide, back_inserter(ret));
	return ret;
}


template<typename _T, typename _T1> 
vector<bool> operator >(const vector<_T> &__a, _T1 __b ){
	vector<bool> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__volarger, back_inserter(ret));
	return ret;
	// return __a;
}

template<typename _T, typename _T1> 
vector<bool> operator >(_T1 __b, const vector<_T> & __a){
	vector<bool> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__volarger, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> vectorLarger(const vector<_T> & __a, const vector<_T1> &__b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __volarger, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator <(const vector<_T> &__a, _T1 __b ){
	vector<bool> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__voless, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator <(_T1 __b, const vector<_T> &__a){
	vector<bool> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__voless, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> vectorLess(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __voless, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator ==(const vector<_T> &__a,
_T1 __b ){
	vector<bool> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__voequal, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator ==(_T1 __b, const vector<_T> &__a){
	vector<bool> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__voequal, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> vectorEqual(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __voequal, back_inserter(ret));
	return ret;
}



template<typename _T, typename _T1> 
vector<bool> operator <=(const vector<_T> &__a, _T1 __b ){
	vector<bool> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__volesseq, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator <=(_T1 __b, const vector<_T> &__a){
	vector<bool> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__volesseq, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> vectorLessEq(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __volesseq, back_inserter(ret));
	return ret;
}


template<typename _T, typename _T1> 
vector<bool> operator >=(const vector<_T> &__a, _T1 __b ){
	vector<bool> ret;
	__vectorScalarOperationCore(__a.begin(), __a.end(), __b, 
		__volargereq, back_inserter(ret));
	return ret;
	// return __a;
}

template<typename _T, typename _T1> 
vector<bool> operator >=(_T1 __b, const vector<_T> &__a){
	vector<bool> ret;
	__vectorScalarOperationCore(__b, __a.begin(), __a.end(),
		__volargereq, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> vectorLargerEq(const vector<_T> &__a, const vector<_T1> &__b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __volargereq, back_inserter(ret));
	return ret;
}

template<typename _T, typename _T1> 
vector<bool> operator &(const vector<_T> &__a, vector<_T1> __b ){
	vector<bool> ret;
	__vectorsOperationCore(__a.begin(), __a.end(), __b.begin(),
	__b.end(), __voand, back_inserter(ret));
	return ret;
}




template<typename T> vector<double> vecFloor(const vector<T> &a){
	vector<double> ret;
	for (unsigned i = 0; i < a.size(); ++i)
	{
		ret.push_back((double)floor(a[i]));
	}
	return ret;
}




#endif