#include "Misc.h"

bool Intersects(const tagRECT& a, const tagRECT& b)
{
	return (a.left <= b.right &&
		b.left <= a.right &&
		a.top <= b.bottom &&
		b.top <= a.bottom);
}

float CoerceInRange(float start, float length, float minValue, float maxValue)
{
	start -= max(0, start + length - maxValue);
	start += max(0, minValue - start);
	return start;
}