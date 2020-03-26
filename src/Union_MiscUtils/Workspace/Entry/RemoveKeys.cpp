namespace NAMESPACE
{
	void __fastcall Hook_oCMobContainer_Open(oCMobContainer*, void*, oCNpc*);
	CInvoke<void(__thiscall*)(oCMobContainer*, oCNpc*)> Ivk_oCMobContainer_Open(ZenDef<TInstance>(0x006843D0, 0x006B3FA0, 0x006C8470, 0x00726500), &Hook_oCMobContainer_Open, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCMobContainer_Open(oCMobContainer* _this, void* vtable, oCNpc* npc)
	{
		if (settings.removeKeys)
		{
			bool removed = CKeyRemover().TryRemoveKey(npc, _this->keyInstance);
			if (removed && ogame->GetTextView())
				ogame->GetTextView()->PrintTimedCXY(Z"Key removed: " + _this->keyInstance, 5000.0f, nullptr);
		}
		Ivk_oCMobContainer_Open(_this, npc);
	}

	void __fastcall Hook_oCMobDoor_Open(oCMobDoor*, void*, oCNpc*);
	CInvoke<void(__thiscall*)(oCMobDoor*, oCNpc*)> Ivk_oCMobDoor_Open(ZenDef<TInstance>(0x00679250, 0x006A7890, 0x006BC3A0, 0x0071A430), &Hook_oCMobDoor_Open, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCMobDoor_Open(oCMobDoor* _this, void* vtable, oCNpc* npc)
	{
		if (settings.removeKeys)
		{
			bool removed = CKeyRemover().TryRemoveKey(npc, _this->keyInstance);
			if (removed && ogame->GetTextView())
				ogame->GetTextView()->PrintTimedCXY(Z"Key removed: " + _this->keyInstance, 5000.0f, nullptr);
		}
		Ivk_oCMobDoor_Open(_this, npc);
	}
}