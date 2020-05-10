#include "CSubscription.h"
#include <algorithm>
#include <unordered_set>

CPublisher::CPublisher() :
	dllDetached(false),
	nesting(0)
{

}

CPublisher& CPublisher::GetInstance()
{
	static CPublisher instance;
	return instance;
}

void CPublisher::Subscribe(const TGameEvent& event, const std::function<void()>* delegate)
{
	Subs(event).push_back(delegate);
}

void CPublisher::Unsubscribe(const TGameEvent& event, const std::function<void()>* delegate)
{
	if (dllDetached)
	{
		return;
	}

	auto where = std::find(Subs(event).begin(), Subs(event).end(), delegate);
	if (where != Subs(event).end())
		Subs(event).erase(where);
}

void CPublisher::Raise(const TGameEvent & event)
{
	nesting += 1;
	std::unordered_set<const std::function<void()>*> toExecute(Subs(event).begin(), Subs(event).end());

	bool executed;
	do
	{
		executed = false;

		for (auto* delegate : Subs(event))
		{
			auto where = toExecute.find(delegate);
			if (where != toExecute.end())
			{
				(*delegate)();
				executed = true;
				toExecute.erase(where);
				break;
			}
		}
	} while (executed);

	nesting -= 1;
	dllDetached = dllDetached || ((event == TGameEvent::DetachDll) && (nesting == 0));
}
