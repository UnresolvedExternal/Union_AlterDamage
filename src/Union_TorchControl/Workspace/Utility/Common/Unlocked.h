#pragma once

#include "UnionAfx.h"

template <class T>
struct Unlocked
{
private:
	TMemUnlocker unlocker;

public:
	Unlocked(uint32_t address = 0x00000000u) :
		unlocker{ reinterpret_cast<void32>(address), sizeof(T), address }
	{
		
	}

	Unlocked(Unlocked&& y) :
		unlocker{ y.unlocker }
	{
		y.unlocker.ulAddress = nullptr;
		y.unlocker.ulSize = 0u;
		y.unlocker.ulOldRoot = 0u;
	}

	Unlocked& operator=(Unlocked&& y)
	{
		unlocker.~TMemUnlocker();

		unlocker.ulAddress = y.unlocker.ulAddress;
		unlocker.ulSize = y.unlocker.ulSize;
		unlocker.ulOldRoot = y.unlocker.ulOldRoot;

		y.unlocker.ulAddress = nullptr;
		y.unlocker.ulSize = 0u;
		y.unlocker.ulOldRoot = 0u;
	}

	operator T() const
	{
		return *reinterpret_cast<T*>(unlocker.ulAddress);
	}

	Unlocked& operator=(const T& value)
	{
		Value() = value;
		return *this;
	}

	T& Value()
	{
		return *reinterpret_cast<T*>(unlocker.ulAddress);
	}
};
