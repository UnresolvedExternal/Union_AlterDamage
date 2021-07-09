#include "CGlobalEventPublisher.h"
#include <algorithm>
#include "UnionAfx.h"

std::unordered_map<CGlobalEventPublisher::GlobalEvent, std::vector<std::function<void()>* > > CGlobalEventPublisher::events;

void CGlobalEventPublisher::Subscribe(CGlobalEventPublisher::GlobalEvent event, std::function<void()>* subscriber)
{
	auto& callbacks = events[event];
	callbacks.push_back(subscriber);
}

void CGlobalEventPublisher::Unsubscribe(CGlobalEventPublisher::GlobalEvent event, std::function<void()>* subscriber)
{
	auto& callbacks = events[event];
	auto it = std::find(callbacks.begin(), callbacks.end(), subscriber);
	
	if (it != callbacks.end())
	{
		callbacks.erase(it);
	}
}

void CGlobalEventPublisher::Raise(CGlobalEventPublisher::GlobalEvent event)
{
	auto& callbacks = events[event];
	std::for_each(callbacks.begin(), callbacks.end(), [](std::function<void()>* callback) { (*callback)(); });
}