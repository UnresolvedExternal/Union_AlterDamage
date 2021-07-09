#pragma once

#include "Publisher.h"

template <class T>
class Sub
{
private:
	GameEvent events;
	T* option;
	Delegate<void()> delegate;
	Delegate<void()> onOption;
	bool attached;

	void InitOnOption()
	{
		onOption = [this]()
		{
			if (attached == static_cast<bool>(**option))
				return;

			if (attached)
				Publisher::GetInstance().Unsubscribe(events, delegate);
			else
				Publisher::GetInstance().Subscribe(events, delegate);

			attached = !attached;
		};

		option->onChange += onOption;
	}

public:
	Sub() :
		events{ GameEvent::NoEvent },
		attached{ false }
	{
		Publisher::GetInstance();
	}

	Sub(const GameEvent& events, T& option, const Delegate<void()>& delegate) :
		events{ events },
		option{ &option },
		delegate{ delegate },
		attached{ false }
	{
		Publisher::GetInstance();

		if (this->events & GameEvent::Execute)
		{
			this->events = this->events & ~GameEvent::Execute;

			if (static_cast<bool>(*option))
				this->delegate();
		}

		if (this->events == GameEvent::NoEvent)
			return;

		if (static_cast<bool>(*option))
		{
			attached = true;
			Publisher::GetInstance().Subscribe(events, this->delegate);
		}

		InitOnOption();
	}

	Sub(Sub&& y) :
		events{ y.events },
		option{ y.option },
		delegate{ std::move(y.delegate) },
		attached{ y.attached }
	{
		if (events == GameEvent::NoEvent)
			return;

		option -= y.onOption;
		InitOnOption();

		y.onOption = nullptr;
		y.events = GameEvent::NoEvent;
		y.attached = false;
	}

	Sub& operator=(Sub&& y)
	{
		if (events != GameEvent::NoEvent)
		{
			if (attached)
				Publisher::GetInstance().Unsubscribe(events, delegate);

			option->onChange -= onOption;
		}

		events = y.events;
		option = y.option;
		delegate = std::move(y.delegate);
		attached = y.attached;

		option->onChange -= y.onOption;
		InitOnOption();

		y.onOption = nullptr;
		y.events = GameEvent::NoEvent;
		y.attached = false;

		return *this;
	}

	~Sub()
	{
		if (events != GameEvent::NoEvent)
			option->onChange -= onOption;

		if (attached)
			Publisher::GetInstance().Unsubscribe(events, delegate);
	}
};

template <>
class Sub<void>
{
private:
	GameEvent events;
	Delegate<void()> delegate;
	bool attached;

public:
	Sub() :
		events{ GameEvent::NoEvent },
		attached{ false }
	{
		Publisher::GetInstance();
	}

	Sub(const GameEvent& events, const Delegate<void()>& delegate) :
		events{ events },
		delegate{ delegate },
		attached{ (events & ~GameEvent::Execute) != GameEvent::NoEvent }
	{
		Publisher::GetInstance();

		if (this->events & GameEvent::Execute)
		{
			this->events = this->events & ~GameEvent::Execute;
			this->delegate();
		}

		if (this->events == GameEvent::NoEvent)
			return;

		Publisher::GetInstance().Subscribe(this->events, this->delegate);
	}

	Sub(Sub&& y) :
		events{ y.events },
		delegate{ std::move(y.delegate) },
		attached{ y.attached }
	{
		y.events = GameEvent::NoEvent;
		y.attached = false;
	}

	Sub& operator=(Sub&& y)
	{
		this->~Sub();

		events = y.events;
		delegate = std::move(y.delegate);
		attached = y.attached;

		y.events = GameEvent::NoEvent;
		y.attached = false;

		return *this;
	}

	~Sub()
	{
		if (attached)
			Publisher::GetInstance().Unsubscribe(events, delegate);

		events = GameEvent::NoEvent;
		attached = false;
	}
};

Sub(const GameEvent& a, const Delegate<void()>& b ) -> Sub<void>;
Sub() -> Sub<void>;

GameEvent MergeEvents(std::initializer_list<GameEvent> list)
{
	GameEvent merged{};

	for (GameEvent e : list)
		merged = merged | e;

	return merged;
}

#define ADDSUB(event) subs.emplace_back(GameEvent::event, std::bind(&std::remove_pointer_t<decltype(this)>::On##event, this))
#define ZSUB(...) ZTEST(MergeEvents({ __VA_ARGS__ }))