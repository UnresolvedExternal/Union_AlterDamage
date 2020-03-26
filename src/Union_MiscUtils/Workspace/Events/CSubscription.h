#pragma once

#include "CPublisher.h"
#include "Workspace\Utility\CMemPool.h"

class CSubscription
{
	MEMPOOL_DECL(CSubscription)

private:
	TGameEvent event;
	std::function<void()> delegate;
	std::function<void()>* helperDelegate;

	CMemPool<std::function<void()>>& GetDelegatesPool();

public:
	CSubscription();
	CSubscription(const TGameEvent& event, const std::function<void()>& delegate, bool enabled = true);
	CSubscription(CSubscription&& subscription);
	void Reset(const TGameEvent& event, const std::function<void()>& delegate, bool enabled = true);
	void Reset();
	~CSubscription();
};