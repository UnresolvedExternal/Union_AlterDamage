#pragma once

#include <vector>
#include <functional>
#include <memory>

template <class T>
class Delegate
{
private:
	typedef std::function<T> TFunction;

	std::shared_ptr<TFunction> delegate;

public:
	Delegate() :
		delegate(std::make_shared<TFunction>(nullptr))
	{

	}

	template <class T>
	Delegate(const T& function) :
		delegate(std::make_shared<TFunction>(function))
	{

	}

	Delegate(const Delegate&) = default;
	Delegate(Delegate&&) = default;
	Delegate& operator=(const Delegate&) = default;
	Delegate& operator=(Delegate&&) = default;

	bool operator==(const Delegate& right) const
	{
		return delegate == right.delegate;
	}

	bool operator!=(const Delegate& right) const
	{
		return delegate != right.delegate;
	}

	template <class ...TArgs>
	void operator()(TArgs&& ...args) const
	{
		(*delegate)(args...);
	}

	const TFunction& GetRaw() const
	{
		return *delegate;
	}
};

template <class T>
class DelegateList
{
private:
	std::vector<Delegate<T>> delegates;

public:
	template <class ...TArgs>
	void operator()(TArgs&& ... args) const
	{
		auto copy = delegates;

		for (const auto& delegate : copy)
			if (std::find(begin(delegates), end(delegates), delegate) != end(delegates))
				delegate(args...);
	}

	DelegateList& operator+=(const Delegate<T>& delegate)
	{
		delegates.push_back(delegate);
		return *this;
	}

	DelegateList& operator-=(const Delegate<T>& delegate)
	{
		auto it = std::find(begin(delegates), end(delegates), delegate);

		if (it != end(delegates))
			delegates.erase(it);
		
		return *this;
	}
};
