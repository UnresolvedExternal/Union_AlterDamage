#include "CGlobalEventSubscription.h"

CGlobalEventSubscription::CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent event, std::function<void()> subscriber):
	event(event)
{
	this->subscriber.reset(new std::function<void()>(subscriber));
	CGlobalEventPublisher::Subscribe(event, this->subscriber.get());
}

CGlobalEventSubscription::CGlobalEventSubscription(CGlobalEventSubscription&& subscription):
	event(subscription.event)
{
	subscriber.swap(subscription.subscriber);
	subscription.event = CGlobalEventPublisher::NONE;
}

CGlobalEventSubscription::~CGlobalEventSubscription()
{
	if (event != CGlobalEventPublisher::NONE)
	{
		CGlobalEventPublisher::Unsubscribe(event, subscriber.get());
	}
}
