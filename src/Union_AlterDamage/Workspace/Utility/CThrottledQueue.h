#pragma once

#include "UnionAfx.h"
#include <queue>
#include <unordered_map>

template <class T>
class CThrottledQueue
{
private:
	float throttling;
	std::queue<T> queue;
	std::unordered_map<T, float> timestamps;

public:
	CThrottledQueue(float throttling);
	void Enqueu(const T& elem, float timestamp);
	T Dequeu(float timestamp);
	int Size();
};

template<class T>
inline CThrottledQueue<T>::CThrottledQueue(float throttling) :
	throttling(throttling)
{
}

template<class T>
inline void CThrottledQueue<T>::Enqueu(const T& elem, float timestamp)
{
	auto it = timestamps.find(elem);
	if (it != timestamps.end() && it->second + throttling >= timestamp)
		return;
	timestamps[elem] = timestamp;
	queue.push(elem);
}

template<class T>
inline T CThrottledQueue<T>::Dequeu(float timestamp)
{
	T elem = queue.front();
	queue.pop();
	timestamps[elem] = timestamp;
	return elem;
}

template<class T>
inline int CThrottledQueue<T>::Size()
{
	return queue.size();
}
