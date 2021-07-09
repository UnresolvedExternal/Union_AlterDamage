#pragma once

#include "UnionAfx.h"
#include <algorithm>

template <class T>
T CoerceInRange(T start, const T& length, const T& minValue, const T& maxValue)
{
	start -= std::max(T(), start + length - maxValue);
	start += std::max(T(), minValue - start);
	return start;
}

void ParseExternalScript(const string& parser, const string& path);

template <class T>
bool CompareFlags(const std::pair<string, T>& left, const std::pair<string, T>& right)
{
	return left.second > right.second;
}

template <class T>
class CFlagsSerializer
{
private:
	Array<std::pair<string, T>> pairs;
	string delimeter;
	bool sorted;

	void Sort()
	{
		pairs.SortByFunc(&CompareFlags<T>);
		sorted = true;
	}

public:
	CFlagsSerializer(string delimeter = "|") :
		delimeter(delimeter),
		sorted(true)
	{

	}

	void Add(const string& name, const T& value)
	{
		pairs.InsertEnd(std::make_pair(name, value));
		sorted = false;
	}

	string ToString(T flags)
	{
		if (!sorted)
			Sort();

		string str;
		for (uint i = 0; i < pairs.GetNum(); i++)
		{
			if ((flags & pairs[i].second) == pairs[i].second)
			{
				if (str.Length())
					str += delimeter;
				str += pairs[i].first;
				flags = flags & ~pairs[i].second;
			}
		}

		if (flags)
		{
			if (str.Length())
				str += delimeter;
			str += string(flags);
		}

		return str;
	}

	T ToFlags(const string& str) const
	{
		T flags = 0;
		for (int i = 1; true; i++)
		{
			string word = str.GetWord(delimeter, i);
			if (!word.Length())
				break;

			for (uint k = 0; k < pairs.GetNum(); k++)
				if (pairs[k].first.Compare(word))
				{
					flags |= pairs[k].second;
					break;
				}
		}

		return flags;
	}
};

const float epsF = 1e-6;

inline bool IsLessF(float x, float y) { return x < y - epsF; }
inline bool IsMoreF(float x, float y) { return x > y + epsF; }
inline bool IsEqualF(float x, float y) { return x <= y + epsF && x >= y - epsF; }

bool TryParse(const string& line, int& number);
bool TryParse(const string& line, float& number);
bool TryParse(const string& line, string& text);

class CStringConvertible
{
public:
	virtual string ToString() const = 0;
	inline virtual ~CStringConvertible() { }
};

#define LOG(exp) cmd << #exp << ": " << (exp) << endl
