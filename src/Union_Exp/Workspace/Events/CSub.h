#pragma once

#include "CPublisher.h"

template <class T>
class CSub
{
private:
	TGameEvent events;
	T* option;
	CDelegate<void()> delegate;
	CDelegate<void(const T&)> onOption;
	bool attached;

	void InitOnOption()
	{
		onOption = [this](const T& option)
		{
			if (attached == static_cast<bool>(option))
				return;

			if (attached)
				CPublisher::GetInstance().Unsubscribe(events, delegate);
			else
				CPublisher::GetInstance().Subscribe(events, delegate);

			attached = !attached;
		};

		option->onChange += onOption;
	}

public:
	CSub() :
		events{ TGameEvent::NoEvent },
		attached{ false }
	{

	}

	CSub(const TGameEvent& events, T& option, const CDelegate<void()>& delegate) :
		events{ events },
		option{ &option },
		delegate{ delegate },
		attached{ false }
	{
		CPublisher::GetInstance();

		if (this->events & TGameEvent::Execute)
		{
			this->events = this->events & ~TGameEvent::Execute;

			if (static_cast<bool>(option))
				this->delegate();
		}

		if (this->events == TGameEvent::NoEvent)
			return;

		if (static_cast<bool>(option))
		{
			attached = true;
			CPublisher::GetInstance().Subscribe(events, this->delegate);
		}

		InitOnOption();
	}

	CSub(CSub&& y) :
		events{ y.events },
		option{ y.option },
		delegate{ std::move(y.delegate) },
		attached{ y.attached }
	{
		if (events == TGameEvent::NoEvent)
			return;

		option -= y.onOption;
		InitOnOption();

		y.onOption = nullptr;
		y.events = TGameEvent::NoEvent;
		y.attached = false;
	}

	CSub& operator=(CSub&& y)
	{
		if (events != TGameEvent::NoEvent)
		{
			if (attached)
				CPublisher::GetInstance().Unsubscribe(events, delegate);

			option->onChange -= onOption;
		}

		events = y.events;
		option = y.option;
		delegate = std::move(y.delegate);
		attached = y.attached;

		option->onChange -= y.onOption;
		InitOnOption();

		y.onOption = nullptr;
		y.events = TGameEvent::NoEvent;
		y.attached = false;

		return *this;
	}

	~CSub()
	{
		if (events != TGameEvent::NoEvent)
			option->onChange -= onOption;

		if (attached)
			CPublisher::GetInstance().Unsubscribe(events, delegate);
	}
};

template <>
class CSub<void>
{
private:
	TGameEvent events;
	CDelegate<void()> delegate;
	bool attached;

public:
	CSub() :
		events{ TGameEvent::NoEvent },
		attached{ false }
	{

	}

	CSub(const TGameEvent& events, const CDelegate<void()>& delegate) :
		events{ events },
		delegate{ delegate },
		attached{ (events & ~TGameEvent::Execute) != TGameEvent::NoEvent }
	{
		CPublisher::GetInstance();

		if (this->events & TGameEvent::Execute)
		{
			this->events = this->events & ~TGameEvent::Execute;
			this->delegate();
		}

		if (this->events == TGameEvent::NoEvent)
			return;

		CPublisher::GetInstance().Subscribe(this->events, this->delegate);
	}

	CSub(CSub&& y) :
		events{ y.events },
		delegate{ std::move(y.delegate) },
		attached{ y.attached }
	{
		y.events = TGameEvent::NoEvent;
		y.attached = false;
	}

	CSub& operator=(CSub&& y)
	{
		this->~CSub();

		events = y.events;
		delegate = std::move(y.delegate);
		attached = y.attached;

		y.events = TGameEvent::NoEvent;
		y.attached = false;

		return *this;
	}

	~CSub()
	{
		if (attached)
			CPublisher::GetInstance().Unsubscribe(events, delegate);

		events = TGameEvent::NoEvent;
		attached = false;
	}
};

CSub(const TGameEvent& a, const CDelegate<void()>& b ) -> CSub<void>;
CSub() -> CSub<void>;

TGameEvent MergeEvents(std::initializer_list<TGameEvent> list)
{
	TGameEvent merged{};

	for (TGameEvent e : list)
		merged = merged | e;

	return merged;
}

#define ADDSUB(event) subs.emplace_back({ TGameEvent::event, std::bind(&std::remove_pointer_t<decltype(this)>::On##event, this) })
#define ZSUB(...) ZTEST(MergeEvents({ __VA_ARGS__ }))