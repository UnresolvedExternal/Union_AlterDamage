namespace NAMESPACE
{
	namespace Settings
	{
	}

	namespace Settings
	{
		CSub load{ ZTEST(TGameEvent::DefineExternals), []()
			{
				CSingleOptionBase::LoadAll();
			}
		};
	}
}