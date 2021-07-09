#pragma once

#include "CPublisher.h"
#include "Workspace\Utility\Common\CMemPool.h"

class CSubscription
{
	MEMPOOL_DECL(CSubscription)

private:
	TGameEvent event;
	std::function<void()>* delegate;
	
	CMemPool<std::function<void()>>& GetDelegatesPool();

public:
	CSubscription();
	CSubscription(const TGameEvent& event, const std::function<void()>& delegate, bool enabled = true);
	CSubscription(CSubscription&& subscription);
	void Reset(const TGameEvent& event, const std::function<void()>& delegate, bool enabled = true);
	void Reset();
	~CSubscription();
};

// ADDSUB(Loop, ClassName::MethodName);
#define ADDSUB(event, method) subs.push_back(CSubscription(TGameEvent::event, std::bind(&method, this)))