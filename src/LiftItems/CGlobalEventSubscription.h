#pragma once
#include "CGlobalEventPublisher.h"
#include <memory>

class CGlobalEventSubscription
{
private:
	CGlobalEventPublisher::GlobalEvent event;
	std::unique_ptr<std::function<void()> > subscriber;

	CGlobalEventSubscription(const CGlobalEventSubscription& subscription);
	CGlobalEventSubscription& operator=(const CGlobalEventSubscription& subscription);

public:
	CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent event, std::function<void()> subscriber);
	CGlobalEventSubscription(CGlobalEventSubscription&& subscription);
	~CGlobalEventSubscription();
};