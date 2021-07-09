#pragma once

#include "UnionAfx.h"
#include <regex>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

template <class T>
T CoerceInRange(T start, const T& length, const T& minValue, const T& maxValue)
{
	start -= MAX(0, start + length - maxValue);
	start += MAX(0, minValue - start);
	return start;
}

bool Intersects(const tagRECT& a, const tagRECT& b);
Array<string> SearchByRegex(const string& text, const string& re);

template <class TBase, class TDerived>
TDerived* TROLOLO_CAST(TBase* instance)
{
#ifndef _DEBUG
	return dynamic_cast<TDerived*>(instance);
#endif
	return reinterpret_cast<TDerived*>(instance);
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

// Array<T>

template <class T>
const T* begin(const Array<T>& array)
{
	return &array[0];
}

template <class T>
const T* end(const Array<T>& array)
{
	return &array[0] + array.GetNum();
}

template <class T>
const T* cbegin(const Array<T>& array)
{
	return &array[0];
}

template <class T>
const T* cend(const Array<T>& array)
{
	return &array[0] + array.GetNum();
}

template <class T>
T* begin(Array<T>& array)
{
	return &array[0];
}

template <class T>
T* end(Array<T>& array)
{
	return &array[0] + array.GetNum();
}

class CCmdn
{
public:
	template <class T>
	CCmdn& operator<<(const T& e)
	{
		cmd << e;
		return *this;
	}

	~CCmdn()
	{
		cmd << endl;
	}
};

const float epsF = 1e-6;

inline bool IsLessF(float x, float y) { return x < y - epsF; }
inline bool IsMoreF(float x, float y) { return x > y + epsF; }
inline bool IsEqualF(float x, float y) { return x <= y + epsF && x >= y - epsF; }

template <typename T, typename U>
constexpr size_t OffsetOf(U T::* member)
{
	return (char*)& ((T*)nullptr->*member) - (char*)nullptr;
}

inline bool HasWord(const string& text, const string& word)
{
	if (!word.Length())
		return true;

	if (text.Length() < word.Length())
		return false;

	for (uint i = 0; i < text.Length() - word.Length() + 1; i++)
		for (uint k = 0; k <= word.Length(); k++)
			if (k == word.Length())
				return true;
			else
				if (text[i + k] != word[k])
					break;

	return false;
}

#define CMDN if (true) CCmdn()
#define LOG(exp) cmd << #exp << ": " << (exp) << endl
