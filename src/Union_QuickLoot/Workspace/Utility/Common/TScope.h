#pragma once

template<class T>
class TScope
{
private:
	T& var;
	const T value;
	bool moved;

public:
	TScope();
	TScope(T& var);
	TScope(TScope&& scope);
	TScope& operator=(TScope&& right);
	~TScope();
};

template<class T>
inline TScope<T>::TScope() :
	moved(true)
{
}

template<class T>
inline TScope<T>::TScope(T& var) :
	var(var),
	value(var),
	moved(false)
{
}

template<class T>
inline TScope<T>::TScope(TScope&& scope) :
	var(scope.var),
	value(scope.value)
{
	moved = scope.moved;
	scope.moved = true;
}

template<class T>
inline TScope<T>& TScope<T>::operator=(TScope<T>&& right)
{
	return TScope<T>(right);
}

template<class T>
inline TScope<T>::~TScope()
{
	if (!moved)
		var = value;
}

template<class T>
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