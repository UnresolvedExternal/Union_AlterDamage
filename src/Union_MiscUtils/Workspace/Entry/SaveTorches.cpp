namespace NAMESPACE
{
	CArray<oCItem*> torches;
	bool worldChange;
	zVEC3 playerLastPos;
	bool playerLastPosChanged;
	float distanceChange;

	bool IsBurningTorch(zCVob* vob)
	{
		return vob && vob->GetVobType() == zVOB_TYPE_ITEM && vob->objectName == "ITLSTORCHBURNING";
	}

	bool TryExchangeTorch(oCNpc* npc)
	{
		oCWorld* world = ogame->GetWorld()->CastTo<oCWorld>();
		if (!world)
			return false;

		oCItem* torch = npc->GetSlotItem(NPC_NODE_LEFTHAND);
		if (!IsBurningTorch(torch))
			return false;

		int instance = torch->GetInstance();
		npc->DropFromSlot(NPC_NODE_LEFTHAND);
		world->RemoveVobSubtree(torch);
		world->RemoveVob(torch);

		torch = (oCItem*)world->CreateVob(zVOB_TYPE_ITEM, instance);
		world->AddVob(torch);
		npc->PutInSlot(NPC_NODE_LEFTHAND, torch, 0);
		torch->Release();

		return true;
	}

	extern CSubscription saveTorches;
	CSubscription saveTorches(TGameEvent::SaveBegin, []()
		{
			if (!settings.saveTorches)
				return saveTorches.Reset();

			oCItem* playerTorch = nullptr;
			worldChange = true;

			if (player)
			{
				worldChange = false;
				playerTorch = player->GetSlotItem(NPC_NODE_LEFTHAND);
				if (!IsBurningTorch(playerTorch))
					playerTorch = nullptr;
			}

			for (oCItem* item : torches)
				item->dontWriteIntoArchive = item != playerTorch && item->HasFlag(ITM_FLAG_NFOCUS);
		}, CHECK_THIS_ENGINE);

	extern CSubscription trajectPlayerPos_LoadEnd;
	CSubscription trajectPlayerPos_LoadEnd(TGameEvent::LoadEnd, []()
		{
			if (!settings.saveTorches && !settings.exchangeTorchOnTeleport)
				return trajectPlayerPos_LoadEnd.Reset();
			playerLastPos = player->GetPositionWorld();
			playerLastPosChanged = false;
		}, CHECK_THIS_ENGINE);

	extern CSubscription trajectPlayerPos_Loop;
	CSubscription trajectPlayerPos_Loop(TGameEvent::Loop, []()
		{
			if (!settings.saveTorches && !settings.exchangeTorchOnTeleport)
				return trajectPlayerPos_Loop.Reset();

			playerLastPosChanged = false;
			zVEC3 playerPos = player->GetPositionWorld();
			
			if (playerPos.Distance(playerLastPos) > 500)
			{
				distanceChange = playerPos.Distance(playerLastPos);
				playerLastPos = playerPos;
				playerLastPosChanged = true;
			}
		}, CHECK_THIS_ENGINE);

	extern CSubscription loadTorches;
	CSubscription loadTorches(TGameEvent::LoadEnd, []()
		{
			if (!settings.saveTorches)
				return loadTorches.Reset();

			torches.Clear();
			oCWorld* world = ogame->GetGameWorld();
			CVobCollector<oCItem> itemCollector(world);
			bool didPut = false;

			for (oCItem* item : itemCollector.vobs)
			{
				if (!IsBurningTorch(item))
					continue;


				if (!worldChange && !didPut && item->HasFlag(ITM_FLAG_NFOCUS))
				{
					player->DropFromSlot(NPC_NODE_LEFTHAND);
					world->RemoveVobSubtree(item);
					world->RemoveVob(item);
					oCItem* torch = (oCItem*)world->CreateVob_novt(zVOB_TYPE_ITEM, "ITLSTORCHBURNING");
					world->AddVob(torch);
					player->PutInSlot(NPC_NODE_LEFTHAND, torch, 0);
					torch->Release();
					didPut = true;
				}
				else
					torches.InsertEnd(item);
			}

			if (worldChange)
				TryExchangeTorch(player);

			worldChange = false;
		}, CHECK_THIS_ENGINE);

	extern CSubscription removeFarTorches;
	CSubscription removeFarTorches(TGameEvent::Loop, []()
		{
			if (!settings.saveTorches)
				return removeFarTorches.Reset();

			if (!playerLastPosChanged)
				return;

			zCWorld* world = ogame->GetWorld();
			zCVob* cam = ogame->GetCameraVob();
			CArray<oCItem*> oldTorches;
			for (oCItem* oldTorch : torches)
				oldTorches.InsertEnd(oldTorch);
			torches.Clear();

			for (oCItem* oldTorch : oldTorches)
				if (oldTorch->globalVobTreeNode && !oldTorch->HasFlag(ITM_FLAG_NFOCUS) && cam->GetDistanceToVob(*oldTorch) > ogame->GetSpawnManager()->GetRemoveRange())
				{
					world->RemoveVobSubtree(oldTorch);
					world->RemoveVob(oldTorch);
				}
				else
					torches.InsertEnd(oldTorch);
		}, CHECK_THIS_ENGINE);

	CSubscription exchangeTorchOnTeleport(TGameEvent::Loop, []()
		{
			if (playerLastPosChanged && distanceChange > ogame->GetSpawnManager()->GetRemoveRange() / 2)
				TryExchangeTorch(player);
		}, CHECK_THIS_ENGINE);

	void __fastcall Hook_zCVob_ThisVobAddedToWorld(zCVob*, void*, zCWorld*);
	CInvoke<void(__thiscall*)(zCVob*, zCWorld*)> Ivk_zCVob_ThisVobAddedToWorld(ZenDef<TInstance>(0x005D66B0, 0x005F5BF0, 0x005FAE50, 0x00601C80), &Hook_zCVob_ThisVobAddedToWorld, IvkEnabled(ENGINE));
	void __fastcall Hook_zCVob_ThisVobAddedToWorld(zCVob* _this, void* vtable, zCWorld* a0)
	{
		Ivk_zCVob_ThisVobAddedToWorld(_this, a0);

		if (!settings.saveTorches)
		{
			Ivk_zCVob_ThisVobAddedToWorld.Detach();
			return;
		}

		if (IsBurningTorch(_this))
			torches.InsertEnd((oCItem*)_this);
	}

	void __fastcall Hook_zCVob_ThisVobRemovedFromWorld(zCVob*, void*, zCWorld*);
	CInvoke<void(__thiscall*)(zCVob*, zCWorld*)> Ivk_zCVob_ThisVobRemovedFromWorld(ZenDef<TInstance>(0x005D66D0, 0x005F5C10, 0x005FAE70, 0x00601CA0), &Hook_zCVob_ThisVobRemovedFromWorld, IvkEnabled(ENGINE));
	void __fastcall Hook_zCVob_ThisVobRemovedFromWorld(zCVob* _this, void* vtable, zCWorld* a0)
	{
		Ivk_zCVob_ThisVobRemovedFromWorld(_this, a0);
		
		if (!settings.saveTorches)
		{
			Ivk_zCVob_ThisVobRemovedFromWorld.Detach();
			return;
		}

		if (IsBurningTorch(_this))
			torches.Remove((oCItem*)_this);
	}

	int __fastcall Hook_oCNpc_UseItem(oCNpc*, void*, oCItem*);
	CInvoke<int(__thiscall*)(oCNpc*, oCItem*)> Ivk_oCNpc_UseItem(ZenDef<TInstance>(0x00698810, 0x006CA590, 0x006DD450, 0x0073BC10), &Hook_oCNpc_UseItem, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_UseItem(oCNpc* _this, void* vtable, oCItem* a0)
	{
		int result = Ivk_oCNpc_UseItem(_this, a0);

		if (!settings.saveTorches)
		{
			Ivk_oCNpc_UseItem.Detach();
			return result;
		}

		if (IsBurningTorch(_this->GetSlotItem(NPC_NODE_LEFTHAND)) && a0 && a0->HasFlag(ITM_FLAG_TORCH))
			TryExchangeTorch(_this);

		return result;
	}
}