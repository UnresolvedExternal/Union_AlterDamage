#include "CSubscription.h"

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

void CPublisher::Raise(const TGameEvent& event)
{
	nesting += 1;
	Subs(event)();
	nesting -= 1;
	dllDetached = dllDetached || event == TGameEvent::DetachDll && nesting == 0;
}
