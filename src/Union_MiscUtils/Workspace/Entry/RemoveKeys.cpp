namespace NAMESPACE
{
	bool NeedKey(const zSTRING& keyInstance)
	{
		for (zCVob* vob : COA3(ogame, GetGameWorld(), voblist))
		{
			oCMobLockable* mob = vob->CastTo<oCMobLockable>();
			if (mob && mob->locked && mob->keyInstance.CompareI(keyInstance))
				return true;
		}
		return false;
	}

	bool TryRemoveKey(oCNpc* npc, const zSTRING& instance)
	{
		if (!npc->IsAPlayer())
			return false;
		if (!instance.Length())
			return false;
		if (zinput->GetState(GAME_SLOW))
			return false;

		oCItem* key = npc->IsInInv(instance, 0);
		if (!key)
			return false;

		if (NeedKey(instance))
			return false;

		while (key)
		{
			key = npc->RemoveFromInv(key, key->amount);
			ogame->GetWorld()->RemoveVob(key);
			key = npc->IsInInv(instance, 0);
		}

		return true;
	}

	void __fastcall Hook_oCMobContainer_Open(oCMobContainer*, void*, oCNpc*);
	CInvoke<void(__thiscall*)(oCMobContainer*, oCNpc*)> Ivk_oCMobContainer_Open(ZenDef<TInstance>(0x006843D0, 0x006B3FA0, 0x006C8470, 0x00726500), &Hook_oCMobContainer_Open, IvkEnabled(ENGINE));
	void __fastcall Hook_oCMobContainer_Open(oCMobContainer* _this, void* vtable, oCNpc* npc)
	{
		if (settings.removeKeys)
		{
			bool removed = TryRemoveKey(npc, _this->keyInstance);
			if (removed && ogame->GetTextView())
				ogame->GetTextView()->PrintTimedCXY(Z"Key removed: " + _this->keyInstance, 5000.0f, nullptr);
		}

		Ivk_oCMobContainer_Open(_this, npc);
	}

	void __fastcall Hook_oCMobDoor_Open(oCMobDoor*, void*, oCNpc*);
	CInvoke<void(__thiscall*)(oCMobDoor*, oCNpc*)> Ivk_oCMobDoor_Open(ZenDef<TInstance>(0x00679250, 0x006A7890, 0x006BC3A0, 0x0071A430), &Hook_oCMobDoor_Open, IvkEnabled(ENGINE));
	void __fastcall Hook_oCMobDoor_Open(oCMobDoor* _this, void* vtable, oCNpc* npc)
	{
		if (settings.removeKeys)
		{
			bool removed = TryRemoveKey(npc, _this->keyInstance);
			if (removed && ogame->GetTextView())
				ogame->GetTextView()->PrintTimedCXY(Z"Key removed: " + _this->keyInstance, 5000.0f, nullptr);
		}

		Ivk_oCMobDoor_Open(_this, npc);
	}
}