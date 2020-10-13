namespace NAMESPACE
{
	CSubscription loadSettings(ZSUB(Init), []()
		{
			Settings::Load();
		});
}