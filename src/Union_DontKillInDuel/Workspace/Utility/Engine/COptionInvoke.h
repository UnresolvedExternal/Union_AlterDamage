namespace NAMESPACE
{
	template <class T, class TOpt>
	class COptionInvoke
	{
	private:
		CSingleOption<TOpt>& option;
		CDelegate<void(const CSingleOption<TOpt>&)> onOption;
		CInvoke<T> ivk;
		const TInstance origin;
		const TInstance hook;
		const EInterMode mode;
		bool attached;

		void TryAttach()
		{
			if (!attached)
			{
				ivk.Attach(origin, hook, mode);
				attached = true;
			}
		}

		void TryDetach()
		{
			if (attached)
			{
				ivk.Detach();
				attached = false;
			}
		}

	public:
		COptionInvoke(const TInstance& origin, const TInstance& hook, const EInterMode& mode, CSingleOption<TOpt>& option) :
			option(option),
			ivk(origin, hook, IVK_DISABLED),
			origin(origin),
			hook(hook),
			mode(mode),
			attached(false)
		{
			if (mode == IVK_DISABLED)
				return;

			if (*option)
				TryAttach();

			onOption = [this](const CSingleOption<TOpt>& value)
			{
				if (*value)
					TryAttach();
				else
					TryDetach();
			};


			option.onChange += onOption;
		}

		operator T()
		{
			return ivk;
		}

		~COptionInvoke()
		{
			option.onChange -= onOption;
		}
	};
}