#pragma once

#include <memory>

namespace
{
	template <class T>
	inline void Release(T pointer)
	{
		pointer->Release();
	}

	template <class T, T Function>
	struct TFunctionDeleter
	{
		template<typename E>
		auto operator()(E&& e) const noexcept(noexcept(Function(std::forward<E>(e))))
		{
			return Function(std::forward<E>(e));
		}
	};
}

template <class T>
using ZOwner = std::unique_ptr<T, TFunctionDeleter<void(*)(T*), &Release>>;
