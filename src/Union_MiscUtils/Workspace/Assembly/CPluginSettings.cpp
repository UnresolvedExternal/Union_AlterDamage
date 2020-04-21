namespace NAMESPACE
{
	CPluginSettings settings;

	TItemMarkSettings::TItemMarkSettings(const string& name) :
		name(name)
	{
	}

	string TItemMarkSettings::GetTexPosString()
	{
		string posText;
		for (int i = 0; i < 4; i++)
		{
			if (i)
				posText += "|";
			posText += texPos[i];
		}
		return posText;
	}

	void TItemMarkSettings::SetTexPosString(const string& texPosStr)
	{
		for (int i = 0; i < 4; i++)
			texPos[i] = texPosStr.GetWord("|", i + 1).ToInt32();
	}

	void TItemMarkSettings::Save()
	{
		zoptions->WriteInt(CPluginSettings::pluginName, (name + "Prio").GetVector(), prio, false);
		zoptions->WriteString(CPluginSettings::pluginName, (name + "TexName").GetVector(), texName.GetVector(), false);
		zoptions->WriteString(CPluginSettings::pluginName, (name + "TexPos").GetVector(), GetTexPosString().GetVector(), false);
		zoptions->WriteString(CPluginSettings::pluginName, (name + "Opacity").GetVector(), opacity, false);
	}

	void TItemMarkSettings::Load()
	{
		prio = zoptions->ReadInt(CPluginSettings::pluginName, (name + "Prio").GetVector(), prio);
		texName = zoptions->ReadString(CPluginSettings::pluginName, (name + "TexName").GetVector(), texName.GetVector()).ToChar();
		SetTexPosString(zoptions->ReadString(CPluginSettings::pluginName, (name + "TexPos").GetVector(), GetTexPosString().GetVector()).ToChar());
		opacity = zoptions->ReadInt(CPluginSettings::pluginName, (name + "Opacity").GetVector(), opacity);
	}

	CFlagsSerializer<int> CPluginSettings::catsSerializer;
	const char* const CPluginSettings::pluginName = "Union_MiscUtils";

	CPluginSettings::CPluginSettings() :
		unreadDoc("UnreadDoc"),
		newItem("NewItem"),
		addItem("AddItem")
	{
		unreadDoc.prio = 1;
		unreadDoc.texName = "GreenBook.tga";
		unreadDoc.SetTexPosString("1000|4500|4000|7500");
		unreadDoc.opacity = 96;

		newItem.prio = 3;
		newItem.texName = "GreenPlus.tga";
		newItem.SetTexPosString("3900|900|7100|4100");
		newItem.opacity = 200;

		addItem.prio = 2;
		addItem.texName = "ArrowUp.tga";
		addItem.SetTexPosString("4200|1200|6800|3800");
		addItem.opacity = 128;
	}

	void CPluginSettings::Save()
	{
		unreadDoc.Save();
		newItem.Save();
		addItem.Save();

		zoptions->WriteBool(pluginName, "RemoveKeys", removeKeys, false);
		
		zoptions->WriteBool(pluginName, "SaveTorches", saveTorches, false);
		zoptions->WriteBool(pluginName, "ExchangeTorchOnTeleport", exchangeTorchOnTeleport, false);
		zoptions->WriteBool(pluginName, "PrintTorchName", printTorchName, false);
		
		zoptions->WriteString(pluginName, "NameToDescCats", catsSerializer.ToString(nameToDescFlags).GetVector(), false);
		zoptions->WriteBool(pluginName, "AppendAmountInfo", appendAmountInfo, false);
	}

	void CPluginSettings::Load()
	{
		unreadDoc.Load();
		newItem.Load();
		addItem.Load();

		removeKeys = zoptions->ReadBool(pluginName, "RemoveKeys", true);

		saveTorches = zoptions->ReadBool(pluginName, "SaveTorches", true);
		exchangeTorchOnTeleport = zoptions->ReadBool(pluginName, "ExchangeTorchOnTeleport", true);
		printTorchName = zoptions->ReadBool(pluginName, "PrintTorchName", true);
		
		string cats = zoptions->ReadString(pluginName, "NameToDescCats", A"MAGIC|RUNE|POTION");
		nameToDescFlags = catsSerializer.ToFlags(cats);

		appendAmountInfo = zoptions->ReadBool(pluginName, "AppendAmountInfo", true);

		messageLimit = zoptions->ReadInt(pluginName, "MessageLimit", 1);
		midnightFix = zoptions->ReadBool(pluginName, "MidnightFix", true);
		fastFood = zoptions->ReadBool(pluginName, "FastFood", true);
	}
}