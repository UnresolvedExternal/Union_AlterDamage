#pragma once

#include "UnionAfx.h"

bool Intersects(const tagRECT& a, const tagRECT& b);
float CoerceInRange(float start, float length, float minValue, float maxValue);

template <class T>
T* end(CArray<T>& array)
{
	return array.GetNum() ? (&array[array.GetNum() - 1] + 1) : (T*)sizeof(T);
}

template <class T>
T* begin(CArray<T>& array)
{
	return array.GetNum() ? &array[0] : (T*)sizeof(T);
}

