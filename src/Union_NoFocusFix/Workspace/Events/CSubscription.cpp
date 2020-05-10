#include "CSubscription.h"

MEMPOOL_DEF(CSubscription)

CMemPool<std::function<void()>>& CSubscription::GetDelegatesPool()
{
	static CMemPool<std::function<void()>> pool;
	return pool;
}

CSubscription::CSubscription() :
	event(TGameEvent::NoEvent)
{
	CPublisher::GetInstance();
	GetDelegatesPool();
}

CSubscription::CSubscription(const TGameEvent& event, const std::function<void()>& delegate, bool enabled) :
	CSubscription()
{
	Reset(event, delegate, enabled);
}

CSubscription::CSubscription(CSubscription&& subscription) :
	event(subscription.event),
	delegate(subscription.delegate),
	helperDelegate(subscription.helperDelegate)
{
	subscription.event = TGameEvent::NoEvent;
}

void CSubscription::Reset(const TGameEvent& event, const std::function<void()>& delegate, bool enabled)
{
	Reset();
	if (!enabled)
		return;

	this->event = event;

	if (event != TGameEvent::NoEvent)
	{
		this->delegate = delegate;

		helperDelegate = GetDelegatesPool().New([&]()
		{
			this->delegate();
		});

		CPublisher::GetInstance().Subscribe(event, helperDelegate);
	}
}

void CSubscription::Reset()
{
	if (event != TGameEvent::NoEvent)
	{
		CPublisher::GetInstance().Unsubscribe(event, this->helperDelegate);
		event = TGameEvent::NoEvent;
		GetDelegatesPool().Delete(this->helperDelegate);
	}
}

CSubscription::~CSubscription()
{
	Reset();
}
