#pragma once

template<class T>
class TScope
{
private:
	T& var;
	const T value;
	bool moved;

public:
	TScope(T& var);
	TScope(TScope&& scope);
	~TScope();
};

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
inline TScope<T>::~TScope()
{
	if (!moved)
		var = value;
}

template<class T>
TScope<T> AssignTemp(T& var, const T& value)
{
	auto scope = TScope<T>(var);
	var = value;
	return scope;
}