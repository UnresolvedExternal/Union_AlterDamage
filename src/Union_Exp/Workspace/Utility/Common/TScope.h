#pragma once

template<class T>
struct TScope
{
private:
	T* var;
	T value;
	bool enabled;

public:
	TScope();
	TScope(T& var);
	TScope(TScope&& scope);
	TScope& operator=(TScope&& right);
	TScope& Restore();
	TScope& Disable();
	~TScope();
};

template <class T>
inline TScope<T>::TScope() :
	var{},
	value{},
	enabled{}
{

}

template <class T>
inline TScope<T>::TScope(T& var) :
	var{ &var },
	value{ var },
	enabled{ true }
{

}

template <class T>
inline TScope<T>::TScope(TScope&& scope) :
	var{ scope.var },
	value{ std::move(scope.value) },
	enabled{ scope.enabled }
{
	scope.var = nullptr;
	scope.enabled = false;
}

template<class T>
inline TScope<T>& TScope<T>::operator=(TScope<T>&& right)
{
	if (enabled)
		Restore();

	var = right.var;
	value = std::move(right.value);
	enabled = right.enabled;
	right.var = nullptr;

	return *this;
}

template <class T>
inline TScope<T>& TScope<T>::Restore()
{
	*var = value;
	return *this;
}

template <class T>
inline TScope<T>& TScope<T>::Disable()
{
	enabled = false;
	return *this;
}

template <class T>
inline TScope<T>::~TScope()
{
	if (enabled)
		Restore();
}

template <class T>
TScope<T> AssignTemp(T& var, const T& value)
{
	TScope<T> scope{ var };
	var = value;
	return scope;
}

template<class T>
TScope<T> AssignTemp(T& var, std::nullptr_t)
{
	TScope<T> scope{ var };
	var = nullptr;
	return scope;
}
