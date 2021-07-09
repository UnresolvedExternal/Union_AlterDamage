namespace NAMESPACE
{
	namespace Settings
	{
		ZOPTION(MouseScale, 3.0f);
	}

	namespace Settings
	{
		CSubscription loadSettings(ZSUB(Init), []()
			{
				CSingleOptionBase::LoadAll();
			});
	}
}