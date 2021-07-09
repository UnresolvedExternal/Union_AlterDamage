#include "CPublisher.h"

CPublisher& CPublisher::GetInstance()
{
	static CPublisher instance;
	return instance;
}

void CPublisher::Subscribe(const TGameEvent& events, const CDelegate<void()>& delegate)
{
	for (uint32_t i = 0u, flags = events; flags && i < maxGameEvents; i++, flags >>= 1)
		if (flags & 1u)
		{
			subs[i].push_back(&delegate.GetRaw());
			subsHashed[i].insert(&delegate.GetRaw());
		}
}

void CPublisher::Unsubscribe(const TGameEvent& events, const CDelegate<void()>& delegate)
{
	for (uint32_t i = 0u, flags = events; flags && i < maxGameEvents; i++, flags >>= 1)
		if (flags & 1u)
		{
			subs[i].erase(std::find(subs[i].begin(), subs[i].end(), &delegate.GetRaw()));
			subsHashed[i].erase(subsHashed[i].find(&delegate.GetRaw()));
		}
}

void CPublisher::Raise(const TGameEvent& event)
{
	size_t index = 0;

	for (size_t flags = event; flags; flags >>= 1)
		index += 1;

	index -= 1;

	std::vector<const std::function<void()>*> copy = subs[index];

	for (const std::function<void()>* func : copy)
		if (subsHashed[index].find(func) != subsHashed[index].end())
			(*func)();
}
