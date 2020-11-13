namespace NAMESPACE
{
	namespace Settings
	{
		string XChar = "";
		bool AppendAmountInfo = false;
		bool DropArmor = false;
		bool RemoveKeys = false;

		ZOPTION(LootItems, true);
		ZOPTION(LootNpcs, true);
		ZOPTION(LootContainers, true);
		ZOPTION(Key, CKeyCombo({ { MOUSE_BUTTONRIGHT }, { KEY_V } }));

		std::unordered_set<int> Cats;
	}

	namespace Settings
	{
		extern CSubscription load;
		CSubscription load(ZSUB(LoadBegin), []()
			{
				load.Reset();

				bool miscUtilsLoad = false;
				for (const CList<CPlugin>* list = CPlugin::GetPluginList().GetNext(); list; list = list->GetNext())
					if (list->GetData()->GetName().CompareI("Union_MiscUtils.dll"))
					{
						miscUtilsLoad = true;
						break;
					}

				if (!miscUtilsLoad)
				{
					CSingleOptionBase::LoadAll();
					return;
				}

				auto XChar = CreateOption(zoptions, "UNION_MISCUTILS", "XChar", A"");
				auto NameToDescCats = CreateOption(zoptions, "UNION_MISCUTILS", "NameToDescCats", CVectorSetting<string>());
				auto AppendAmountInfo = CreateOption(zoptions, "UNION_MISCUTILS", "AppendAmountInfo", false);
				auto RemoveKeys = CreateOption(zoptions, "UNION_MISCUTILS", "RemoveKeys", false);

				NameToDescCats.onChange += [&NameToDescCats](const CSingleOption<CVectorSetting<string>>& opt)
				{
					Cats.clear();

					for (string name : *NameToDescCats)
					{
						name.Upper();

						if (name == "NF") Cats.insert(ITM_CAT_NF);
						if (name == "FF") Cats.insert(ITM_CAT_FF);
						if (name == "MUN") Cats.insert(ITM_CAT_MUN);
						if (name == "ARMOR") Cats.insert(ITM_CAT_ARMOR);
						if (name == "FOOD") Cats.insert(ITM_CAT_FOOD);
						if (name == "DOCS") Cats.insert(ITM_CAT_DOCS);
						if (name == "POTION") Cats.insert(ITM_CAT_POTION);
						if (name == "LIGHT") Cats.insert(ITM_CAT_LIGHT);
						if (name == "RUNE") Cats.insert(ITM_CAT_RUNE);
						if (name == "MAGIC") Cats.insert(ITM_CAT_MAGIC);
					}
				};

				CSingleOptionBase::LoadAll();

				::NAMESPACE::Settings::XChar = XChar;
				::NAMESPACE::Settings::AppendAmountInfo = AppendAmountInfo;
				::NAMESPACE::Settings::RemoveKeys = RemoveKeys;

				if (Union.GetEngineVersion() == Engine_G2A)
					Union.GetSysPackOption().Read(DropArmor, "PARAMETERS", "ShowArmor", false);
			});
	}
}