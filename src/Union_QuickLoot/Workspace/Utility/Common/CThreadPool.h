#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <queue>

class CThreadPool
{
private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> actions;
	std::mutex mutex;
	std::mutex awaiter;
	std::condition_variable condVar;
	std::condition_variable awaiterVar;
	int uncompletedActions;
	bool stopPending;

	void Run();

public:
	CThreadPool(int concurrency);
	void Enqueue(const std::function<void()>& action);
	void Wait();
	~CThreadPool();
};