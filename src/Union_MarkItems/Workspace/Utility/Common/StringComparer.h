#pragma once

#include "UnionAfx.h"
#include <functional>

namespace std
{
	template <>
	struct hash<CStringA>
	{
		size_t operator()(const CStringA& x) const
		{
			size_t hash = 5381;
			const char* str = x.GetVector();

			while (size_t c = *str++)
				hash = ((hash << 5) + hash) + reinterpret_cast<const unsigned char&>(c);

			return hash;
		}
	};

	template <>
	struct less<CStringA>
	{
		bool operator()(const CStringA& x, const CStringA& y) const
		{
			return strcmp(x.GetVector(), y.GetVector()) < 0;
		}
	};

	template <>
	struct equal_to<CStringA>
	{
		bool operator()(const CStringA& x, const CStringA& y) const
		{
			return strcmp(x.GetVector(), y.GetVector()) == 0;
		}
	};
}
