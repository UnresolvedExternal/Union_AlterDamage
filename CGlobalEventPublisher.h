#pragma once
#include <unordered_map>
#include <vector>
#include <functional>

class CGlobalEventPublisher
{
public:
	enum GlobalEvent 
	{ 
		NONE,
		ENTRY,
		INIT,
		EXIT,
		LOOP,
		SAVE_BEGIN,
		SAVE_END,
		LOAD_BEGIN,
		LOAD_END,
		LOAD_BEGIN_TRIGGER,
		LOAD_END_TRIGGER,
		PAUSE,
		UNPAUSE,
		DEFINE_EXTERNALS
	};

private:
	static std::unordered_map<GlobalEvent, std::vector<std::function<void()>* > > events;
	CGlobalEventPublisher();

public:
	static void Subscribe(GlobalEvent event, std::function<void()>* subscriber);
	static void Unsubscribe(GlobalEvent event, std::function<void()>* subscriber);
	static void Raise(GlobalEvent event);
};