#include "CThreadPool.h"

void CThreadPool::Run()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock { mutex };
		condVar.wait(lock, [&]() { return stopPending || !actions.empty(); });

		if (actions.empty())
			return;
		
		std::function<void()> action = actions.front();
		actions.pop();
		lock.unlock();
		action();

		lock.lock();
		std::unique_lock<std::mutex> awaiterLock { awaiter };
		uncompletedActions -= 1;

		if (!uncompletedActions)
			awaiterVar.notify_all();
	}
}

CThreadPool::CThreadPool(int concurrency) :
	uncompletedActions(0),
	stopPending(false)
{
	threads.reserve(concurrency);

	for (int i = 0; i < concurrency; i++)
		threads.push_back(std::move(std::thread{ std::bind(&CThreadPool::Run, this) }));
}

void CThreadPool::Enqueue(const std::function<void()>& action)
{
	std::unique_lock<std::mutex> lock { mutex };
	actions.push(action);
	condVar.notify_one();
	std::unique_lock<std::mutex> awaiterLock { awaiter };
	uncompletedActions += 1;
}

void CThreadPool::Wait()
{
	std::unique_lock<std::mutex> awaiterLock { awaiter };
	awaiterVar.wait(awaiterLock, [&]() { return !uncompletedActions; });
}

CThreadPool::~CThreadPool()
{
	std::unique_lock<std::mutex> lock { mutex };
	stopPending = true;
	lock.unlock();
	condVar.notify_all();

	for (auto& t : threads)
		t.join();
}
