namespace NAMESPACE
{
	namespace Settings
	{
		ZOPTION(UnreadDocPrio, 1);
		ZOPTION(UnreadDocTexName, A"GreenBook.tga");
		ZOPTION(UnreadDocTexPos, CVectorSetting<int>({ 1000, 4500, 4000, 7500 })); 
		ZOPTION(UnreadDocOpacity, 96);
		
		ZOPTION(NewItemPrio, 3);
		ZOPTION(NewItemTexName, A"GreenPlus.tga");
		ZOPTION(NewItemTexPos, CVectorSetting<int>({ 3900, 900, 7100, 4100 }));
		ZOPTION(NewItemOpacity, 200);

		ZOPTION(AddItemPrio, 2);
		ZOPTION(AddItemTexName, A"ArrowUp.tga");
		ZOPTION(AddItemTexPos, CVectorSetting<int>({ 4200, 1200, 6800, 3800 }));
		ZOPTION(AddItemOpacity, 128);

		ZOPTION(RemoveKeys, true);
		ZOPTION(SaveTorches, true);
		ZOPTION(ExchangeTorchOnTeleport, true);
		ZOPTION(PrintTorchName, true);
		ZOPTION(MidnightFix, true);
		ZOPTION(FastFood, true);
		ZOPTION(MoveLogEntry, true);
		ZOPTION(AppendAmountInfo, true);
		ZOPTION(UnconsciousFix, true);
		ZOPTION(WaterWeaponFix, true);

		ZOPTION(NameToDescCats, CVectorSetting<string>({ "MAGIC", "POTION", "RUNE" }));
		ZOPTION(MessageLimit, 1);
		ZOPTION(XChar, A"x");

		std::unordered_set<int> Cats;
	}

	namespace Settings
	{
		void Load()
		{
			NameToDescCats.onChange += [](const CSingleOption<CVectorSetting<string>> & opt)
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
		}
	}
}