#pragma once

#include <algorithm>

template <class T>
T CoerceInRange(T start, const T& length, const T& minValue, const T& maxValue)
{
	start -= std::max(T(), start + length - maxValue);
	start += std::max(T(), minValue - start);
	return start;
}
