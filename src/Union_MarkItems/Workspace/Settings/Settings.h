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

		ActiveValue<bool> HookRenderContainer;
		ActiveValue<bool> HookRenderItem;
		ActiveValue<bool> HookGetState;
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
					HookGetState = UnreadDocPrio || TrackReadDocs;
				};

				TrackReadDocs.onChange += setGetStateFunc;
				UnreadDocPrio.onChange += setGetStateFunc;

				ActiveOptionBase::LoadAll();
			});
	}
}
