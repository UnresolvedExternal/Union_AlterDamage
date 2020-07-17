namespace NAMESPACE
{
	CPluginSettings::CPluginSettings() : 
		initialized(false)
	{
	}

	void CPluginSettings::LazyLoad()
	{
		if (initialized)
			return;

		initialized = true;
		logicalKey = zoptions->ReadInt(PLUGIN_NAME, "GAME_TORCH", 30);
	}
}