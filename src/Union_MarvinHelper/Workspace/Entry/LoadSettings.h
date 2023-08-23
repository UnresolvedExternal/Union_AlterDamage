namespace NAMESPACE
{
	CSubscription loadSettings(ZSUB(DefineExternals), []()
		{
			Settings::Load();
		});
}