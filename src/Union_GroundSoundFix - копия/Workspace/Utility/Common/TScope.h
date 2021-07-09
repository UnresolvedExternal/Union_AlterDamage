#pragma once

template<class T>
class TScope
{
private:
	T* var;
	T value;

public:
	TScope();
	TScope(T& var);
	TScope(TScope&& scope);
	TScope& operator=(TScope&& right);
	void Restore();
	void Disable();
	~TScope();
};

template <class T>
inline TScope<T>::TScope() :
	var(nullptr),
	value(T())
{

}

template <class T>
inline TScope<T>::TScope(T& var) :
	var(&var),
	value(var)
{

}

template <class T>
inline TScope<T>::TScope(TScope&& scope) :
	var(scope.var),
	value(std::move(scope.value))
{
	scope.var = nullptr;
}

template<class T>
inline TScope<T>& TScope<T>::operator=(TScope<T>&& right)
{
	Restore();
	var = right.var;
	value = std::move(right.value);
	right.var = nullptr;
	return *this;
}

template <class T>
inline void TScope<T>::Restore()
{
	if (var)
	{
		*var = value;
		var = nullptr;
	}
}

template <class T>
inline void TScope<T>::Disable()
{
	var = nullptr;
}

template <class T>
inline TScope<T>::~TScope()
{
	Restore();
}

template <class T>
TScope<T> AssignTemp(T& var, const T& value)
{
	TScope<T> scope(var);
	var = value;
	return scope;
}

template<class T>
TScope<T> AssignTemp(T& var, std::nullptr_t)
{
	TScope<T> scope(var);
	var = nullptr;
	return scope;
}