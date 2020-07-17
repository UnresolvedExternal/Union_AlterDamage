namespace NAMESPACE
{
	CPluginSettings settings;

	void CPluginSettings::Load()
	{
		allowChangeSpeed = zoptions->ReadBool(PLUGIN_NAME, "AllowChangeSpeed", true);
		allowSkipNoise = zoptions->ReadBool(PLUGIN_NAME, "AllowSkipNoise", true);
		restrictAcrobatic = zoptions->ReadBool(PLUGIN_NAME, "RestrictAcrobatic", true);

		string overlaysString = zoptions->ReadString(PLUGIN_NAME, "ReapplyOverlays", "HUMANS_SPRINT.MDS");
		overlaysString.Upper();
		Array<string> overlays = overlaysString.Regex_Search("[^|]+");
		for (const string& overlay : overlays)
			reapplyOverlays.InsertEnd(overlay);
	}
}