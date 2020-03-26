namespace NAMESPACE
{
	bool CKeyRemover::NeedKey(const zSTRING& keyInstance)
	{
		needKey = false;
		key = &keyInstance;
		zCWorld* world = ogame->GetWorld();
		world->TraverseVobTree(*this, nullptr, &world->globalVobTree);
		return needKey;
	}

	bool CKeyRemover::TryRemoveKey(oCNpc* npc, const zSTRING& instance)
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

	void CKeyRemover::HandleVob(zCVob* vob, void*)
	{
		if (needKey)
			return;

		oCMobLockable* mob = vob->CastTo<oCMobLockable>();
		if (mob && mob->locked && mob->keyInstance.CompareI(*key))
			needKey = true;
	}
}