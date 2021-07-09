namespace NAMESPACE
{
	bool inContainerDraw;
	bool inPlayerContainer;
	std::unordered_map<oCItem*, TItemState> itemStates;

	struct TDrawElement
	{
		int prio;
		std::function<bool(oCItem*, zCWorld*, zCViewBase*, float)> func;

		TDrawElement& operator=(const TDrawElement& right)
		{
			prio = right.prio;
			func = right.func;
			return *this;
		}
	};

	std::vector<TDrawElement> drawElements;

	void __fastcall Hook_oCItemContainer_Draw(oCItemContainer*, void*);
	CInvoke<void(__thiscall*)(oCItemContainer*)> Ivk_oCItemContainer_Draw(ZenDef<TInstance>(0x00667660, 0x00695040, 0x006A9BC0, 0x007076B0), &Hook_oCItemContainer_Draw, IvkEnabled(ENGINE));
	void __fastcall Hook_oCItemContainer_Draw(oCItemContainer* _this, void* vtable)
	{
		auto a = AssignTemp(inContainerDraw, true);
		auto b = AssignTemp(inPlayerContainer, _this == &player->inventory2);

		if (inPlayerContainer)
		{
			static CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");
			invMon.BuildStates(player->inventory2);
		}

		Ivk_oCItemContainer_Draw(_this);
	}

	bool TryDrawUnreadDoc(oCItem* item, zCWorld* rndWorld, zCViewBase* baseView, float time)
	{
		static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");

		if (!inContainerDraw || !Settings::UnreadDocPrio || !item->HasFlag(ITM_CAT_DOCS) || docsRead.Contains(item->GetInstance()) || !item->scemeName.Length())
			return false;

		zCView* view = dynamic_cast<zCView*>(baseView);
		if (!view)
			return false;

		const auto& pos = *Settings::UnreadDocTexPos;

		zCView itemView(pos[0], pos[1], pos[2], pos[3]);
		itemView.InsertBack(Settings::UnreadDocTexName->GetVector());
		itemView.SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
		itemView.SetTransparency(Settings::UnreadDocOpacity);
		view->InsertItem(&itemView);
		itemView.Blit();
		view->RemoveItem(&itemView);

		return true;
	}

	bool TryDrawNewItem(oCItem* item, zCWorld* rndWorld, zCViewBase* baseView, float time)
	{
		static CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");

		if (!inContainerDraw || !inPlayerContainer || !Settings::NewItemPrio || invMon.GetState(item) != TItemState::New)
			return false;

		zCView* view = dynamic_cast<zCView*>(baseView);
		if (!view)
			return false;

		const auto& pos = *Settings::NewItemTexPos;

		zCView itemView(pos[0], pos[1], pos[2], pos[3]);
		itemView.InsertBack(Settings::NewItemTexName->GetVector());
		itemView.SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
		itemView.SetTransparency(Settings::NewItemOpacity);
		view->InsertItem(&itemView);
		itemView.Blit();
		view->RemoveItem(&itemView);

		return true;
	}

	bool TryDrawAddItem(oCItem* item, zCWorld* rndWorld, zCViewBase* baseView, float time)
	{
		CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");

		if (!inContainerDraw || !inPlayerContainer || !Settings::AddItemPrio || invMon.GetState(item) != TItemState::Add)
			return false;

		zCView* view = dynamic_cast<zCView*>(baseView);
		if (!view)
			return false;

		const auto& pos = *Settings::AddItemTexPos;

		zCView itemView(pos[0], pos[1], pos[2], pos[3]);
		itemView.InsertBack(Settings::AddItemTexName->GetVector());
		itemView.SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
		itemView.SetTransparency(Settings::AddItemOpacity);
		view->InsertItem(&itemView);
		itemView.Blit();
		view->RemoveItem(&itemView);

		return true;
	}

	extern CInvoke<void(__thiscall*)(oCItem*, zCWorld*, zCViewBase*, float)> Ivk_oCItem_RenderItem;
	bool TryDrawItem(oCItem* item, zCWorld* rndWorld, zCViewBase* baseView, float time)
	{
		Ivk_oCItem_RenderItem(item, rndWorld, baseView, time);
		return true;
	}

	void __fastcall Hook_oCItemContainer_DrawItemInfo(oCItemContainer*, void*, oCItem*, zCWorld*);
	CInvoke<void(__thiscall*)(oCItemContainer*, oCItem*, zCWorld*)> Ivk_oCItemContainer_DrawItemInfo(ZenDef<TInstance>(0x00667220, 0x00694AD0, 0x006A9350, 0x00706E40), &Hook_oCItemContainer_DrawItemInfo, IvkEnabled(ENGINE));
	void __fastcall Hook_oCItemContainer_DrawItemInfo(oCItemContainer* _this, void* vtable, oCItem* a0, zCWorld* a1)
	{
		auto scope = AssignTemp(inContainerDraw, false);
		Ivk_oCItemContainer_DrawItemInfo(_this, a0, a1);
	}

	void __fastcall Hook_oCItem_RenderItem(oCItem*, void*, zCWorld*, zCViewBase*, float);
	CInvoke<void(__thiscall*)(oCItem*, zCWorld*, zCViewBase*, float)> Ivk_oCItem_RenderItem(ZenDef<TInstance>(0x00672F70, 0x006A0A20, 0x006B5A30, 0x00713AC0), &Hook_oCItem_RenderItem, IvkEnabled(ENGINE));
	void __fastcall Hook_oCItem_RenderItem(oCItem* _this, void* vtable, zCWorld* world, zCViewBase* viewBase, float time)
	{
		if (!drawElements.size())
		{
			TDrawElement ele;
			
			ele.prio = Settings::UnreadDocPrio;
			ele.func = &TryDrawUnreadDoc;
			drawElements.push_back(ele);

			ele.prio = Settings::NewItemPrio;
			ele.func = &TryDrawNewItem;
			drawElements.push_back(ele);

			ele.prio = Settings::AddItemPrio;
			ele.func = &TryDrawAddItem;
			drawElements.push_back(ele);

			ele.prio = 0;
			ele.func = &TryDrawItem;
			drawElements.push_back(ele);

			std::sort(begin(drawElements), end(drawElements), [](const TDrawElement& left, const TDrawElement& right)
				{
					return left.prio < right.prio;
				});
		}

		for (const TDrawElement& ele : drawElements)
			ele.func(_this, world, viewBase, time);
	}

	CSubscription updateDrawElements(ZSUB(Loop), []()
		{
			drawElements.clear();
		});
}