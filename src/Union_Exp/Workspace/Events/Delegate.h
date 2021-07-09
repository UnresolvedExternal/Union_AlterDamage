#pragma once

#include <vector>
#include <functional>
#include <memory>

template <class TDelegate>
class CDelegate
{
private:
	typedef std::function<TDelegate> TFunction;

	std::shared_ptr<TFunction> delegate;

public:
	CDelegate() :
		delegate(std::make_shared<TFunction>(nullptr))
	{

	}

	template <class T>
	CDelegate(const T& function) :
		delegate(std::make_shared<TFunction>(function))
	{

	}

	CDelegate(const CDelegate&) = default;
	CDelegate(CDelegate&&) = default;
	CDelegate& operator=(const CDelegate&) = default;
	CDelegate& operator=(CDelegate&&) = default;

	bool operator==(const CDelegate& right) const
	{
		return delegate == right.delegate;
	}

	bool operator!=(const CDelegate& right) const
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

template <class TDelegate>
class CDelegateContainer
{
private:
	std::vector<CDelegate<TDelegate>> delegates;

public:
	template <class ...TArgs>
	void operator()(TArgs&& ... args) const
	{
		auto copy = delegates;
		for (const auto& delegate : copy)
			if (std::find(begin(delegates), end(delegates), delegate) != end(delegates))
				delegate(args...);
	}

	CDelegateContainer& operator+=(const CDelegate<TDelegate>& delegate)
	{
		delegates.push_back(delegate);
		return *this;
	}

	CDelegateContainer& operator-=(const CDelegate<TDelegate>& delegate)
	{
		auto it = std::find(begin(delegates), end(delegates), delegate);
		if (it != end(delegates))
			delegates.erase(it);
		return *this;
	}
};
