#pragma once

#include "Workspace\Events\Delegate.h"

template <class T>
class ActiveValue
{
private:
	T value;

public:
	ActiveValue(const T& value = {}) : value{ value } { }
	ActiveValue(ActiveValue&&) = default;

	operator T() const { return value; }
	const T& operator*() const { return value; }
	const T* operator->() const { return &value; }

	ActiveValue& operator=(const T& y)
	{
		const bool propagate = value != y;
		value = y;

		if (propagate)
			onChange();

		return *this;
	}

	DelegateList<void()> onChange;
};
