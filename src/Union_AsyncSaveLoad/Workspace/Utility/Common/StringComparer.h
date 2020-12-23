#pragma once

#include "UnionAfx.h"

class CStringEqualityComparer
{
public:
	bool operator()(const string& x, const string& y) const;
};

class CStringHasher
{
public:
	size_t operator()(const string& x) const;
};

class CStringComparer
{
public:
	int operator()(const string& x, const string& y) const;
};