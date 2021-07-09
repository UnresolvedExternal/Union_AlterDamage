namespace NAMESPACE
{
	template <class T>
	class COptionSubscription
	{
	private:
		const TGameEvent event;
		const CDelegate<void()> onEvent;
		CSingleOption<T>& option;
		CDelegate<void(const CSingleOption<T>&)> onOption;

	public:
		COptionSubscription(const TGameEvent& event, CSingleOption<T>& option, const CDelegate<void()>& delegate) :
			event(event),
			onEvent(delegate),
			option(option)
		{
			CPublisher::GetInstance();

			if (event == TGameEvent::NoEvent)
				return;

			if (*option)
				CPublisher::GetInstance().Subs(event) += delegate;

			onOption = [event, delegate](const CSingleOption<T>& option)
			{
				if (*option)
				{
					CPublisher::GetInstance().Subs(event) -= delegate;
					CPublisher::GetInstance().Subs(event) += delegate;
				}
				else
					CPublisher::GetInstance().Subs(event) -= delegate;
			};

			option.onChange += onOption;
		}

		~COptionSubscription()
		{
			if (event != TGameEvent::NoEvent)
			{
				CPublisher::GetInstance().Subs(event) -= onEvent;
				option.onChange -= onOption;
			}
		}
	};
}