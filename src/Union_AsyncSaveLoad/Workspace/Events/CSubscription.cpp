#include "CSubscription.h"

MEMPOOL_DEF(CSubscription)

CSubscription::CSubscription() :
	event(TGameEvent::NoEvent),
	delegate([]() { })
{
	CPublisher::GetInstance();
}

CSubscription::CSubscription(const TGameEvent& event, const CDelegate<void()>& delegate) :
	event(event),
	delegate(delegate)
{
	CPublisher::GetInstance();

	if (event != TGameEvent::NoEvent)
		CPublisher::GetInstance().Subs(event) += delegate;
}

CSubscription::CSubscription(CSubscription&& subscription) :
	event(subscription.event),
	delegate(subscription.delegate)
{
	subscription.event = TGameEvent::NoEvent;
}

void CSubscription::Reset(const TGameEvent& event, const CDelegate<void()>& delegate)
{
	Reset();

	this->event = event;
	this->delegate = delegate;

	if (event != TGameEvent::NoEvent)
		CPublisher::GetInstance().Subs(event) += delegate;
}

void CSubscription::Reset()
{
	if (event != TGameEvent::NoEvent)
	{
		CPublisher::GetInstance().Subs(event) -= delegate;
		event = TGameEvent::NoEvent;
	}
}

CSubscription::~CSubscription()
{
	Reset();
}
