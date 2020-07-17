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
		zoptions->WriteInt(PLUGIN_NAME, (name + "Prio").GetVector(), prio, false);
		zoptions->WriteString(PLUGIN_NAME, (name + "TexName").GetVector(), texName.GetVector(), false);
		zoptions->WriteString(PLUGIN_NAME, (name + "TexPos").GetVector(), GetTexPosString().GetVector(), false);
		zoptions->WriteString(PLUGIN_NAME, (name + "Opacity").GetVector(), opacity, false);
	}

	void TItemMarkSettings::Load()
	{
		prio = zoptions->ReadInt(PLUGIN_NAME, (name + "Prio").GetVector(), prio);
		texName = zoptions->ReadString(PLUGIN_NAME, (name + "TexName").GetVector(), texName.GetVector()).ToChar();
		SetTexPosString(zoptions->ReadString(PLUGIN_NAME, (name + "TexPos").GetVector(), GetTexPosString().GetVector()).ToChar());
		opacity = zoptions->ReadInt(PLUGIN_NAME, (name + "Opacity").GetVector(), opacity);
	}

	CFlagsSerializer<int> CPluginSettings::catsSerializer;

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

	void CPluginSettings::Load()
	{
		unreadDoc.Load();
		newItem.Load();
		addItem.Load();

		removeKeys = zoptions->ReadBool(PLUGIN_NAME, "RemoveKeys", true);

		saveTorches = zoptions->ReadBool(PLUGIN_NAME, "SaveTorches", true);
		exchangeTorchOnTeleport = zoptions->ReadBool(PLUGIN_NAME, "ExchangeTorchOnTeleport", true);
		printTorchName = zoptions->ReadBool(PLUGIN_NAME, "PrintTorchName", true);
		
		string cats = zoptions->ReadString(PLUGIN_NAME, "NameToDescCats", A"MAGIC|RUNE|POTION");
		nameToDescFlags = catsSerializer.ToFlags(cats);

		appendAmountInfo = zoptions->ReadBool(PLUGIN_NAME, "AppendAmountInfo", true);
		xChar = zoptions->ReadString(PLUGIN_NAME, "XChar", "õ").ToChar();

		messageLimit = zoptions->ReadInt(PLUGIN_NAME, "MessageLimit", 1);
		midnightFix = zoptions->ReadBool(PLUGIN_NAME, "MidnightFix", true);
		fastFood = zoptions->ReadBool(PLUGIN_NAME, "FastFood", true);
		moveLogEntry = zoptions->ReadBool(PLUGIN_NAME, "MoveLogEntry", true);
	}
}