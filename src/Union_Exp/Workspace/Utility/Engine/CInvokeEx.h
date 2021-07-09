#include <variant>

namespace NAMESPACE
{
	enum class THookMode
	{
		Disabled = 0,
		Hook,
		Patch
	};

	template <class T, class TActiveValue = void>
	class CInvokeEx
	{
	private:
		TActiveValue* option;
		CDelegate<void(const TActiveValue&)> onChange;
		std::variant<std::monostate, CInvoke<T>, ModulePatchCallInvoker<T>> ivk;
		const TInstance origin;
		const TInstance hook;
		const THookMode mode;
		bool attached;

		void TryAttach()
		{
			if (attached)
				return;

			attached = true;

			if (ivk.index() == 0)
				if (mode == THookMode::Patch)
					ivk.emplace<2>(origin, hook);
				else
					ivk.emplace<1>(origin, hook);

			if (mode == THookMode::Patch)
				std::get<2>(ivk).Attach(origin, hook);
			else
				std::get<1>(ivk).Attach(origin, hook);
		}

		void TryDetach()
		{
			if (!attached)
				return;

			attached = false;

			if (mode == THookMode::Patch)
				std::get<2>(ivk).Detach();
			else
				std::get<1>(ivk).Detach();
		}

	public:
		CInvokeEx(const TInstance& origin, const TInstance& hook, const THookMode& mode, TActiveValue& option) :
			option{ (origin && mode != THookMode::Disabled) ? &option : nullptr },
			ivk{ },
			origin{ origin },
			hook{ hook },
			mode{ mode },
			attached{ false }
		{
			if (!this->option)
				return;

			if (static_cast<bool>(option))
				TryAttach();

			onChange = [this](const TActiveValue& value)
			{
				if (static_cast<bool>(value))
					TryAttach();
				else
					TryDetach();
			};

			option.onChange += onChange;
		}

		operator T()
		{
			if (mode == THookMode::Patch)
				return std::get<2>(ivk);
			else
				return std::get<1>(ivk);
		}

		~CInvokeEx()
		{
			if (!option)
				return;
				
			option->onChange -= onChange;
			TryDetach();
		}
	};

	template <class T>
	class CInvokeEx<T, void>
	{
	private:
		std::variant<std::monostate, CInvoke<T>, ModulePatchCallInvoker<T>> ivk;
		const TInstance origin;
		const TInstance hook;
		const THookMode mode;
		bool attached;

		void TryAttach()
		{
			if (attached)
				return;

			attached = true;

			if (ivk.index() == 0)
				if (mode == THookMode::Patch)
					ivk.emplace<2>(origin, hook);
				else
					ivk.emplace<1>(origin, hook);

			if (mode == THookMode::Patch)
				std::get<2>(ivk).Attach(origin, hook);
			else
				std::get<1>(ivk).Attach(origin, hook);
		}

		void TryDetach()
		{
			if (!attached)
				return;

			attached = false;

			if (mode == THookMode::Patch)
				std::get<2>(ivk).Detach();
			else
				std::get<1>(ivk).Detach();
		}

	public:
		CInvokeEx(const TInstance& origin, const TInstance& hook, const THookMode& mode) :
			ivk{ },
			origin{ origin },
			hook{ hook },
			mode{ mode },
			attached{ false }
		{
			if (!origin || mode == THookMode::Disabled)
				return;

			TryAttach();
		}

		operator T()
		{
			if (mode == THookMode::Patch)
				return std::get<2>(ivk);
			else
				return std::get<1>(ivk);
		}

		~CInvokeEx()
		{
			TryDetach();
		}
	};

	template <class T, class E>
	auto InvokeExAuto_BySignature(string from, T to, const THookMode& mode, E& opt)
	{
		return CInvokeEx<T, E>{ static_cast<TInstance>(FindEngineAddress(from, typeid(to).name())), static_cast<TInstance>(to), mode, opt };
	}

#define AS_OPT(from, to, mode, option) InvokeExAuto_BySignature(#from, to, mode, option)

#define FASTHOOK_OPT(className, methodName, option)	\
	auto Hook_##className##_##methodName { AS_OPT(&##className##::##methodName, &##className##::##methodName##_Union, THookMode::Hook, option) }

#define FASTHOOK_PATCH_OPT(className, methodName, option)	\
	auto Hook_##className##_##methodName { AS_OPT(&##className##::##methodName, &##className##::##methodName##_Union, THookMode::Patch, option) }
}