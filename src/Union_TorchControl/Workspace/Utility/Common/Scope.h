#pragma once

template <class T>
struct VarScope
{
private:
	T* var;
	T value;
	bool enabled;

public:
	VarScope();
	VarScope(T& var);
	VarScope(VarScope&& scope);
	VarScope& operator=(VarScope&& right);
	VarScope& Restore();
	VarScope& Disable();
	~VarScope();
};

template <class T>
inline VarScope<T>::VarScope() :
	var{},
	value{},
	enabled{}
{

}

template <class T>
inline VarScope<T>::VarScope(T& var) :
	var{ &var },
	value{ var },
	enabled{ true }
{

}

template <class T>
inline VarScope<T>::VarScope(VarScope&& scope) :
	var{ scope.var },
	value{ std::move(scope.value) },
	enabled{ scope.enabled }
{
	scope.var = nullptr;
	scope.enabled = false;
}

template <class T>
inline VarScope<T>& VarScope<T>::operator=(VarScope<T>&& right)
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
inline VarScope<T>& VarScope<T>::Restore()
{
	*var = value;
	return *this;
}

template <class T>
inline VarScope<T>& VarScope<T>::Disable()
{
	enabled = false;
	return *this;
}

template <class T>
inline VarScope<T>::~VarScope()
{
	if (enabled)
		Restore();
}

template <class T>
VarScope<T> AssignTemp(T& var, const T& value)
{
	VarScope<T> scope{ var };
	var = value;
	return scope;
}

template <class T>
VarScope<T> AssignTemp(T& var, std::nullptr_t)
{
	VarScope<T> scope{ var };
	var = nullptr;
	return scope;
}
