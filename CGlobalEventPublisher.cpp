#include "CGlobalEventPublisher.h"
#include <algorithm>
#include <set>

std::unordered_map<CGlobalEventPublisher::GlobalEvent, std::vector<std::function<void()>*>> CGlobalEventPublisher::events;

void CGlobalEventPublisher::Subscribe(CGlobalEventPublisher::GlobalEvent event, std::function<void()>* subscriber)
{
	auto& callbacks = events[event];
	callbacks.push_back(subscriber);
}

void CGlobalEventPublisher::Unsubscribe(CGlobalEventPublisher::GlobalEvent event, std::function<void()>* subscriber)
{
	auto& callbacks = events[event];
	auto it = std::find(callbacks.begin(), callbacks.end(), subscriber);
	callbacks.erase(it);
}

void CGlobalEventPublisher::Raise(CGlobalEventPublisher::GlobalEvent event)
{
	auto& callbacks = events[event];
	std::set<std::function<void()>*> toExecute(callbacks.begin(), callbacks.end());
	bool executed;

	do
	{
		executed = false;

		for (size_t i = 0; i < callbacks.size(); i++)
		{
			auto it = toExecute.find(callbacks[i]);

			if (it != toExecute.end())
			{
				(*callbacks[i])();
				executed = true;
				toExecute.erase(it);
			}
		}
	} while (executed);
}