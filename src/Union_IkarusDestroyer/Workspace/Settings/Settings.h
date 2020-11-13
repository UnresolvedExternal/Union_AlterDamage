namespace NAMESPACE
{
	namespace Settings
	{
		ZOPTION(Threshold, 50);
		ZOPTION(Divisor, 50);
		ZOPTION(TickRate, 2000);
		ZOPTION(PerTick, 0);
	}

	namespace Settings
	{
		CSubscription load(ZSUB(Init), []()
			{
				CSingleOptionBase::LoadAll();
			});
	}
}