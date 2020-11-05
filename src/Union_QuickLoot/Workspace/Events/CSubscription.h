#pragma once

#include "CPublisher.h"
#include "Workspace\Utility\Common\CMemPool.h"

class CSubscription
{
	MEMPOOL_DECL(CSubscription)

private:
	TGameEvent event;
	CDelegate<void()> delegate;

public:
	CSubscription();
	CSubscription(const TGameEvent& event, const CDelegate<void()>& delegate);
	CSubscription(CSubscription&& subscription);
	void Reset(const TGameEvent& event, const CDelegate<void()>& delegate);
	void Reset();
	~CSubscription();
};

#define ZSUB(event) (CHECK_THIS_ENGINE ? (TGameEvent::event) : TGameEvent::NoEvent)
#define ADDSUB(event) subs.push_back(CSubscription(TGameEvent::event, std::bind(&std::remove_pointer_t<decltype(this)>::On##event, this)))
