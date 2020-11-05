#include "StringComparer.h"

bool CStringEqualityComparer::operator()(const string& x, const string& y) const
{
	return x == y;
}

size_t CStringHasher::operator()(const string& x) const
{
	size_t hash = 5381;
	const unsigned char* str = reinterpret_cast<const unsigned char*>(x.GetVector());
	int c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

int CStringComparer::operator()(const string& x, const string& y) const
{
	return strcmp(x.GetVector(), y.GetVector());
}
