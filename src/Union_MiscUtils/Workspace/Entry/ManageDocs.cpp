namespace NAMESPACE
{
	CSubscription loadDocsRead(TGameEvent::LoadEnd, []()
		{
			static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");
			docsRead.Load();
		}, SubEnabled(CurrentEngine));

	CSubscription saveDocsRead(TGameEvent::SaveBegin, []()
		{
			static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");
			docsRead.Save();
		}, SubEnabled(CurrentEngine));

	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc*, void*, oCMsgManipulate*);
	CInvoke<int(__thiscall*)(oCNpc*, oCMsgManipulate*)> Ivk_oCNpc_EV_UseItemToState(ZenDef<TInstance>(0x006AFC70, 0x006E3AF0, 0x006F6AC0, 0x007558F0), &Hook_oCNpc_EV_UseItemToState, IvkEnabled(CurrentEngine));
	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc* _this, void* vtable, oCMsgManipulate* message)
	{
		int result = Ivk_oCNpc_EV_UseItemToState(_this, message);
		static CSGInstanceSet& docsRead = CSGGlobal::Get<CSGInstanceSet>("DocsRead");

		if (_this->IsAPlayer() && result && message && message->subType == oCMsgManipulate::EV_USEITEMTOSTATE && message->targetState == 0 && message->targetVob && message->targetVob->GetVobType() == zVOB_TYPE_ITEM)
		{
			oCItem* item = (oCItem*)message->targetVob;
			if (item->HasFlag(ITM_CAT_DOCS) && item->objectName.Length())
				docsRead.TryAdd(item->GetInstance());
		}

		return result;
	}
}