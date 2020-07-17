namespace NAMESPACE
{
	CPluginSettings settings;

	CFlagsSerializer<int> CPluginSettings::catsSerializer;

	CPluginSettings::CPluginSettings() :
		DropArmor(false),
		NameToDescFlags(0),
		AppendAmountInfo(false),
		XChar("õ")
	{
	}

	void CPluginSettings::Load()
	{
		OPT(Bool, LootItems, true);
		OPT(Bool, LootContainers, true);
		OPT(Bool, LootNpcs, true);

		if (Union.GetEngineVersion() == Engine_G2A)
			Union.GetSysPackOption().Read(DropArmor, "PARAMETERS", "ShowArmor", false);

		bool miscUtilsLoad = false;
		for (const CList<CPlugin>* list = CPlugin::GetPluginList().GetNext(); list; list = list->GetNext())
			if (list->GetData()->GetName().CompareI("Union_MiscUtils.dll"))
			{
				miscUtilsLoad = true;
				break;
			}

		if (!miscUtilsLoad)
			return;

		string cats = zoptions->ReadString("Union_MiscUtils", "NameToDescCats", "");
		NameToDescFlags = catsSerializer.ToFlags(cats);
		AppendAmountInfo = zoptions->ReadString("Union_MiscUtils", "AppendAmountInfo", false);
		XChar = zoptions->ReadString("Union_MiscUtils", "XChar", "õ").ToChar();
	}
}