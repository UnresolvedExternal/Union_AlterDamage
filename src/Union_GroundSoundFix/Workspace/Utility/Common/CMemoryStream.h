#pragma once

#include "UnionAfx.h"

class CMemoryStream
{
private:
	byte* buffer;

public:
	CMemoryStream(void* buffer) :
		buffer{ reinterpret_cast<byte*>(buffer) }
	{

	}

	void Read(void* data, size_t size)
	{
		CopyMemory(data, buffer, size);
		buffer += size;
	}

	void Write(const void* data, size_t size)
	{
		CopyMemory(buffer, data, size);
		buffer += size;
	}

	template <class T>
	CMemoryStream& operator<<(const T& data)
	{
		*reinterpret_cast<T*>(buffer) = data;
		buffer += sizeof(T);
		return *this;
	}

	template <>
	CMemoryStream& operator<<(const char* const& data)
	{
		byte* savedPos = buffer;
		buffer += sizeof(uint16);

		for (const char* str = data; *str; str++, buffer++)
			*buffer = *str;

		*reinterpret_cast<uint16*>(savedPos) = buffer - sizeof(uint16) - savedPos;
		return *this;
	}

	template <>
	CMemoryStream& operator<<(char* const& data)
	{
		return (*this) << const_cast<const char* const&>(data);
	}

	template <>
	CMemoryStream& operator<<(const string& data)
	{
		return (*this) << data.GetVector();
	}

	template <class T>
	CMemoryStream& operator>>(T& data)
	{
		data = *reinterpret_cast<T*>(buffer);
		buffer += sizeof(T);
		return *this;
	}

	template <>
	CMemoryStream& operator>>(string& data)
	{
		CMemoryStream& in = *this;
		
		uint16 len;
		in >> len;
		
		char* vector = new char[len + 1];
		in.Read(vector, len);
		vector[len] = '\0';
		data = vector;
		delete[] vector;
		
		return in;
	}
};