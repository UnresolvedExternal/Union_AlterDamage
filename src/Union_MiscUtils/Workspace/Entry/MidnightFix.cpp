namespace NAMESPACE
{
	void __fastcall Hook_oCGame_RefreshNpcs(oCGame*, void*);
	CInvoke<void(__thiscall*)(oCGame*)> Ivk_oCGame_RefreshNpcs(ZenDef<TInstance>(0x006409B0, 0x00667A70, 0x0066E870, 0x006CB5F0), &Hook_oCGame_RefreshNpcs, IvkEnabled(ENGINE));
	void __fastcall Hook_oCGame_RefreshNpcs(oCGame* _this, void* vtable)
	{
		if (!settings.midnightFix)
		{
			Ivk_oCGame_RefreshNpcs.Detach();
			return Ivk_oCGame_RefreshNpcs(_this);
		}

		for (oCNpc* npc : _this->GetGameWorld()->voblist_npcs)
			if (!npc->IsAPlayer() && npc->sleepingMode != zTVobSleepingMode::zVOB_AWAKE)
				npc->RefreshNpc();
	}
}