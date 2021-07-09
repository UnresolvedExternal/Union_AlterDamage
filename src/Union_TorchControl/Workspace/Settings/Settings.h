namespace NAMESPACE
{
	namespace Settings
	{
		ZOPTION(TrackReadDocs, true);
		ZOPTION(TrackNewItems, true);

		ZOPTION(UnreadDocPrio, 1);
		ZOPTION(UnreadDocTexName, A"GreenBook.tga");
		ZOPTION(UnreadDocTexPos, VectorOption<int>({ 1000, 4500, 4000, 7500 }));
		ZOPTION(UnreadDocOpacity, 96);

		ZOPTION(NewItemPrio, 3);
		ZOPTION(NewItemTexName, A"GreenPlus.tga");
		ZOPTION(NewItemTexPos, VectorOption<int>({ 3900, 900, 7100, 4100 }));
		ZOPTION(NewItemOpacity, 200);

		ZOPTION(AddItemPrio, 2);
		ZOPTION(AddItemTexName, A"ArrowUp.tga");
		ZOPTION(AddItemTexPos, VectorOption<int>({ 4200, 1200, 6800, 3800 }));
		ZOPTION(AddItemOpacity, 128);

		ZOPTION(RemoveKeys, false);
		ZOPTION(SuppressRemoveKeysKey, KeyCombo({ { KEY_LSHIFT } }));

		ZOPTION(SaveTorches, ZENDEF(false, false, true, true));
		ZOPTION(ExchangeTorchOnTeleport, true);
		ZOPTION(TorchRemoveRange, 2000.0f);
		ZOPTION(PrintTorchName, true);
		ZOPTION(MidnightFix, true);
		ZOPTION(FastFood, true);
		ZOPTION(FastFoodKey, KeyCombo({ {KEY_LSHIFT} }));
		ZOPTION(MoveLogEntry, true);
		ZOPTION(AppendAmountInfo, true);
		ZOPTION(DiaHyperskipKey, KeyCombo({ { KEY_LSHIFT } }));
		ZOPTION(UnconsciousFix, true);
		ZOPTION(WaterWeaponFix, true);
		ZOPTION(PackStringFix, true);
		ZOPTION(GetAmountFix, true);
		ZOPTION(TakeTorchFix, true);

		ZOPTION(NameToDescCats, VectorOption<string>({ "MAGIC", "POTION", "RUNE" }));
		ZOPTION(MessageLimit, 1);
		ZOPTION(XChar, A"x");

		ActiveValue<bool> HookUpdatePlayerStatus;
		ActiveValue<bool> HookRenderContainer;
		ActiveValue<bool> HookRenderItem;
		ActiveValue<bool> HookGetState;

		std::unordered_set<int> Cats;
	}

	namespace Settings
	{
		Sub load(ZSUB(GameEvent::DefineExternals), []()
			{
				auto setRenderHooks = []()
				{
					HookRenderItem = (NewItemPrio || AddItemPrio) && TrackNewItems || UnreadDocPrio && TrackReadDocs;
					HookRenderContainer = HookRenderItem || TrackNewItems;
				};

				TrackNewItems.onChange += setRenderHooks;
				TrackReadDocs.onChange += setRenderHooks;
				UnreadDocPrio.onChange += setRenderHooks;
				NewItemPrio.onChange += setRenderHooks;
				AddItemPrio.onChange += setRenderHooks;

				auto setGetStateFunc = []()
				{
					HookGetState = UnreadDocPrio && TrackReadDocs || FastFood;
				};

				TrackReadDocs.onChange += setGetStateFunc;
				UnreadDocPrio.onChange += setGetStateFunc;
				FastFood.onChange += setGetStateFunc;

				NameToDescCats.onChange += []()
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

				auto setUpdatePlayerStatusHook = []()
				{
					HookUpdatePlayerStatus = PrintTorchName || !Cats.empty() || AppendAmountInfo;
				};

				PrintTorchName.onChange += setUpdatePlayerStatusHook;
				NameToDescCats.onChange += setUpdatePlayerStatusHook;
				AppendAmountInfo.onChange += setUpdatePlayerStatusHook;

				ActiveOptionBase::LoadAll();
			});
	}
}
