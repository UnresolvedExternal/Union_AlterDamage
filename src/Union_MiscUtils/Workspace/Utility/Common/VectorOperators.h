#pragma once

#include <vector>
#include <algorithm>

namespace std
{
	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator+=(std::vector<T, TAlloc>& vector, const T& value)
	{
		vector.push_back(value);
		return vector;
	}

	template <>
	std::vector<int>& operator+=(std::vector<int>& vector, const int& value)
	{
		vector.push_back(value);
		return vector;
	}

	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator+=(std::vector<T, TAlloc>& vector, T&& value)
	{
		vector.push_back(std::move(value));
		return vector;
	}

	template <class T, class TAllocLeft, class TAllocRight>
	std::vector<T, TAllocLeft>& operator+=(std::vector<T, TAllocLeft>& left, const std::vector<T, TAllocRight>& right)
	{
		left.reserve(left.size() + right.size());

		for (const T& value : right)
			left += value;

		return left;
	}

	template <class T, class TAllocLeft, class TAllocRight>
	std::vector<T, TAllocLeft>& operator+=(std::vector<T, TAllocLeft>& left, std::vector<T, TAllocRight>&& right)
	{
		left.reserve(left.size() + right.size());

		for (T&& value : right)
			left += std::move(value);

		right.clear();
		return left;
	}

	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator-=(std::vector<T, TAlloc>& vector, const T& value)
	{
		auto it = std::find(vector.begin(), vector.end(), value);

		if (it == vector.end())
			return vector;

		if (it + 1 != vector.end())
			*it = std::move(vector.back());

		vector.pop_back();
		return vector;
	}

	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator^=(std::vector<T, TAlloc>& vector, const T& value)
	{
		size_t removed = 0;

		for (size_t i = 0; i < vector.size(); i++)
			if (vector[i] == value)
				removed += 1;
			else
				if (removed)
					vector[i - removed] = std::move(vector[i]);

		if (removed)
			vector.resize(vector.size() - removed);

		return vector;
	}

	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator|=(std::vector<T, TAlloc>& vector, const T& value)
	{
		if (std::find(vector.begin(), vector.end(), value) == vector.end())
			vector += value;

		return vector;
	}

	template <class T, class TAlloc>
	std::vector<T, TAlloc>& operator|=(std::vector<T, TAlloc>& vector, T&& value)
	{
		if (std::find(vector.begin(), vector.end(), value) == vector.end())
			vector += std::move(value);

		return vector;
	}

	template <class T, class TAlloc>
	bool operator&(const std::vector<T, TAlloc>& vector, const T& value)
	{
		return std::find(vector.begin(), vector.end(), value) != vector.end();
	}
}