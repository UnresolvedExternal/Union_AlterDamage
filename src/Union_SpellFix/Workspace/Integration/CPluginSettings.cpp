namespace NAMESPACE
{
	void CPluginSettings::Load()
	{
		collideTargetFix = zoptions->ReadBool(pluginName.GetVector(), "CollideTargetFix", true);
	}

	CPluginSettings settings;
}