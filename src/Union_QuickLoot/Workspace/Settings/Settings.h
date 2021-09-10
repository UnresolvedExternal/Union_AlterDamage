namespace NAMESPACE
{
	namespace Settings
	{
		bool DropArmor = false;
		bool zMiscUtilsLoaded = false;
		string XChar = "x";
		bool AppendAmountInfo = false;
		bool RemoveKeys;

		ZOPTION(LootItems, true);
		ZOPTION(LootNpcs, true);
		ZOPTION(LootContainers, true);
		ZOPTION(LootInDanger, true);

#if ENGINE == Engine_G1
		ZOPTION(Key, CKeyCombo({ { KEY_V } }));
#else
		ZOPTION(Key, CKeyCombo({ { MOUSE_BUTTONRIGHT }, { KEY_V } }));
#endif

		ZOPTION(AnimatedText, true);
		ZOPTION(ForceAmountInfo, true);

		std::unordered_set<int> Cats;
	}

	namespace Settings
	{

#if ENGINE == Engine_G2A
		CSubscription loadShowArmor(ZSUB(Entry), []()
			{
				Union.GetSysPackOption().Read(DropArmor, "PARAMETERS", "ShowArmor", false);
			});
#endif

		extern CSubscription load;
		CSubscription load(ZSUB(LoadBegin), []()
			{
				load.Reset();

				for (const CList<CPlugin>* list = CPlugin::GetPluginList().GetNext(); list; list = list->GetNext())
					if (list->GetData()->GetName().CompareI("zMiscUtils.dll"))
					{
						zMiscUtilsLoaded = true;
						break;
					}

				if (!zMiscUtilsLoaded)
				{
					CSingleOptionBase::LoadAll();
					return;
				}

				static auto XChar = CreateOption(zoptions, "ZMISCUTILS", "XChar", A"");
				static auto NameToDescCats = CreateOption(zoptions, "ZMISCUTILS", "NameToDescCats", CVectorSetting<string>());
				static auto AppendAmountInfo = CreateOption(zoptions, "ZMISCUTILS", "AppendAmountInfo", false);
				static auto RemoveKeys = CreateOption(zoptions, "ZMISCUTILS", "RemoveKeys", false);

				XChar.onChange += [](const auto& opt)
				{
					::NAMESPACE::Settings::XChar = *XChar;
				};

				NameToDescCats.onChange += [](const auto& opt)
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

				AppendAmountInfo.onChange += [](const auto& opt)
				{
					::NAMESPACE::Settings::AppendAmountInfo = *AppendAmountInfo;
				};

				RemoveKeys.onChange += [](const auto& opt)
				{
					::NAMESPACE::Settings::RemoveKeys = *RemoveKeys;
				};

				CSingleOptionBase::LoadAll();
			});
	}
}