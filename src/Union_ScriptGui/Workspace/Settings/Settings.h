namespace NAMESPACE
{
	namespace Settings
	{

	}

	namespace Settings
	{
		CSubscription loadSettings(ZSUB(Init), []()
			{
				CSingleOptionBase::LoadAll();
			});
	}
}