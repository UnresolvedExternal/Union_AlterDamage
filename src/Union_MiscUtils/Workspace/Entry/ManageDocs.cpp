namespace NAMESPACE
{
	CSubscription loadDocsRead(ZSUB(LoadEnd), []()
		{
			static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");
			docsRead.Load();
		});

	CSubscription saveDocsRead(ZSUB(SaveBegin), []()
		{
			static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");
			docsRead.Save();
		});

	oCItem* getStateEffectCalledOn = nullptr;

	int __fastcall Hook_oCItem_GetStateEffectFunc(oCItem*, void*, int);
	CInvoke<int(__thiscall*)(oCItem*, int)> Ivk_oCItem_GetStateEffectFunc(ZenDef<TInstance>(0x00672420, 0x0069FDE0, 0x006B4D80, 0x00712B80), &Hook_oCItem_GetStateEffectFunc, IvkEnabled(ENGINE));
	int __fastcall Hook_oCItem_GetStateEffectFunc(oCItem* _this, void* vtable, int a0)
	{
		getStateEffectCalledOn = _this;
		int result = Ivk_oCItem_GetStateEffectFunc(_this, a0);
		return result;
	}

	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc*, void*, oCMsgManipulate*);
	CInvoke<int(__thiscall*)(oCNpc*, oCMsgManipulate*)> Ivk_oCNpc_EV_UseItemToState(ZenDef<TInstance>(0x006AFC70, 0x006E3AF0, 0x006F6AC0, 0x007558F0), &Hook_oCNpc_EV_UseItemToState, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc* _this, void* vtable, oCMsgManipulate* message)
	{
		getStateEffectCalledOn = nullptr;
		int result = Ivk_oCNpc_EV_UseItemToState(_this, message);

		static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");

		if (getStateEffectCalledOn && _this->IsAPlayer() && message && message->subType == oCMsgManipulate::EV_USEITEMTOSTATE && message->targetState == 0 && message->targetVob == getStateEffectCalledOn)
		{
			oCItem* item = getStateEffectCalledOn;
			if (item->HasFlag(ITM_CAT_DOCS) && item->objectName.Length())
				docsRead.TryAdd(item->GetInstance());
		}

		return result;
	}
}