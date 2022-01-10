#pragma once

#include "basic_comparators.h"


template<class T>
inline bool less<T>::operator()(T first, T second)
{
	return first < second;
}

template<class T>
inline bool greater<T>::operator()(T first, T second)
{
	return first > second;
}

template<class T>
inline bool less_or_equal<T>::operator()(T first, T second)
{
	return first <= second;
}

template<class T>
inline bool greater_or_equal<T>::operator()(T first, T second)
{
	return first >= second;
}


