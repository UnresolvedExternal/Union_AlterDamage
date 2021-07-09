#pragma once
#include "UnionAfx.h"
#include <vector>

#define MEMPOOL_DECL(className)					\
public:											\
	void* operator new(size_t size)				\
	{											\
		return memPool.Alloc();					\
	}											\
												\
	void operator delete(void* ptr)				\
	{											\
		memPool.Free(ptr);						\
	}											\
												\
private:										\
	static CMemPool<className> memPool;			


#define MEMPOOL_DEF(className)					\
CMemPool<className> className::memPool;			\


template<class T>
class CMemPool
{
private:
	std::vector<void*> pages;
	std::vector<void*> segments;
	size_t pageSize;
	size_t segmentSize;
	size_t perPage;

	void EnsureSpace();
	inline void* PopSegment();

public:
	inline CMemPool();

	template <class ...Args>
	inline T* New(Args&&... args);

	inline void Delete(T* ptr);
	inline void* Alloc();
	inline void Free(void* ptr);
	inline ~CMemPool();
};

template<class T>
void CMemPool<T>::EnsureSpace()
{
	if (segments.empty())
	{
		void* page = new char[pageSize];
		pages.push_back(page);

		for (size_t i = 0; i < perPage; i++)
			segments.push_back((char*)page + i * segmentSize);
	}
}

template<class T>
inline void* CMemPool<T>::PopSegment()
{
	void* segment = segments[segments.size() - 1];
	segments.pop_back();
	return segment;
}

namespace
{
	size_t GetMax(size_t x, size_t y)
	{
		return (x > y) ? x : y;
	}
}

template<class T>
inline CMemPool<T>::CMemPool()
{
	segmentSize = sizeof(T);
	pageSize = GetMax((size_t)1 << 14, segmentSize * 16);
	perPage = pageSize / segmentSize;
}

template<class T>
template<class ...Args>
inline T* CMemPool<T>::New(Args&&... args)
{
	return new (Alloc()) T(args...);
}

template<class T>
inline void CMemPool<T>::Delete(T* ptr)
{
	ptr->~T();
	segments.push_back(ptr);
}

template<class T>
inline void* CMemPool<T>::Alloc()
{
	EnsureSpace();
	return PopSegment();
}

template<class T>
inline void CMemPool<T>::Free(void* ptr)
{
	segments.push_back(ptr);
}

template<class T>
inline CMemPool<T>::~CMemPool()
{
	for (auto page : pages)
		delete[] page;
}
