namespace NAMESPACE
{
	void __fastcall Hook_oCGame_RefreshNpcs(oCGame*, void*);
	Hook<void(__thiscall*)(oCGame*), ActiveOption<bool>> Ivk_oCGame_RefreshNpcs(ZENFOR(0x006409B0, 0x00667A70, 0x0066E870, 0x006CB5F0), &Hook_oCGame_RefreshNpcs, HookMode::Patch, Settings::MidnightFix);
	void __fastcall Hook_oCGame_RefreshNpcs(oCGame* _this, void* vtable)
	{
		for (oCNpc* npc : _this->GetGameWorld()->voblist_npcs)
			if (!npc->IsAPlayer() && npc->sleepingMode != zTVobSleepingMode::zVOB_AWAKE)
				npc->RefreshNpc();
	}
}
