#pragma once

template <class T>
class CCache
{
protected:
	std::vector<CSubscription> subs;
	bool initialized;

	CCache() :
		initialized(false)
	{

	}

	virtual void Initialize() = 0;
	virtual void Clear() = 0;
	virtual std::vector<const T*> Filter(const std::function<bool(const T&)>& predicate) const = 0;

public:
	void Invalidate()
	{
		Clear();
		initialized = false;
	}

	std::vector<const T*> Select(const std::function<bool(const T&)>& predicate = nullptr)
	{
		if (!initialized)
		{
			Initialize();
			initialized = true;
		}

		return std::move(Filter(predicate ? predicate : std::function<bool(const T&)>([](const T&) { return true; })));
	}

	virtual ~CCache()
	{
	};
};

template <class T>
class CVectorCache : public CCache<T>
{
protected:
	virtual void Initialize() override
	{

	}

	virtual void Clear() override
	{
		entities.clear();
	}

	virtual std::vector<const T*> Filter(const std::function<bool(const T&)>& predicate) const override
	{
		std::vector<const T*> filtered;

		for (const T& entity : entities)
			if (predicate(entity))
				filtered.push_back(&entity);

		return std::move(filtered);
	}

public:
	std::vector<T> entities;
};

template <class T>
class CSubCache : public CCache<T>
{
protected:
	CCache<T>& cache;
	std::vector<const T*> entities;
	std::function<bool(const T&)> predicate;

	virtual void Initialize() override
	{
		if (predicate)
			entities = cache.Select(predicate);
		else
			entities = cache.Select();
	}

	virtual void Clear() override
	{
		entities.clear();
	}

	virtual std::vector<const T*> Filter(const std::function<bool(const T&)>& predicate) const override
	{
		std::vector<const T*> filtered;

		for (const T* entity : entities)
			if (predicate(*entity))
				filtered.push_back(entity);

		return std::move(filtered);
	}

public:
	CSubCache(CCache& cache) :
		cache(cache)
	{

	}

	void SetPredicate(const std::function<bool(const T&)>& predicate)
	{
		this->predicate = predicate;
		Invalidate();
	}
};