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

	COptionSubscription<bool> saveTorches(ZSUB(SaveBegin), Settings::SaveTorches, []()
		{
			oCItem* playerTorch = nullptr;
			worldChange = true;

			if (player)
			{
				worldChange = false;
				playerTorch = COA2(player, GetSlotItem(NPC_NODE_LEFTHAND));
				if (!IsBurningTorch(playerTorch))
					playerTorch = nullptr;
			}

			for (oCItem* item : torches)
				item->dontWriteIntoArchive = item != playerTorch && item->HasFlag(ITM_FLAG_NFOCUS);
		});

	CSubscription trajectPlayerPos_LoadEnd(ZSUB(LoadEnd), []()
		{
			if (!Settings::SaveTorches && !Settings::ExchangeTorchOnTeleport)
				return;

			if (!player)
				return;

			playerLastPos = player->GetPositionWorld();
			playerLastPosChanged = false;
		});

	CSubscription trajectPlayerPos_Loop(ZSUB(Loop), []()
		{
			if (!Settings::SaveTorches && !Settings::ExchangeTorchOnTeleport)
				return;

			if (!player)
				return;

			playerLastPosChanged = false;
			zVEC3 playerPos = player->GetPositionWorld();
			
			if (playerPos.Distance(playerLastPos) > 500)
			{
				distanceChange = playerPos.Distance(playerLastPos);
				playerLastPos = playerPos;
				playerLastPosChanged = true;
			}
		});

	COptionSubscription<bool> loadTorches(ZSUB(LoadEnd), Settings::SaveTorches, []()
		{
			torches.Clear();
			oCWorld* world = ogame->GetGameWorld();
			CVobTraverser traverser;
			bool didPut = false;

			for (oCItem* item : world->voblist_items)
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
		});

	COptionSubscription<bool> removeFarTorches(ZSUB(Loop), Settings::SaveTorches, []()
		{
			if (!playerLastPosChanged)
				return;

			zCWorld* world = ogame->GetWorld();
			zCVob* cam = ogame->GetCameraVob();
			Array<oCItem*> oldTorches;
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
		});

	COptionSubscription<bool> exchangeTorchOnTeleport(ZSUB(Loop), Settings::ExchangeTorchOnTeleport, []()
		{
			if (playerLastPosChanged && distanceChange > ogame->GetSpawnManager()->GetRemoveRange() / 2)
				TryExchangeTorch(player);
		});

	void __fastcall Hook_zCVob_ThisVobAddedToWorld(zCVob*, void*, zCWorld*);
	COptionInvoke<void(__thiscall*)(zCVob*, zCWorld*), bool> Ivk_zCVob_ThisVobAddedToWorld(ZenDef<TInstance>(0x005D66B0, 0x005F5BF0, 0x005FAE50, 0x00601C80), &Hook_zCVob_ThisVobAddedToWorld, IvkEnabled(ENGINE), Settings::SaveTorches);
	void __fastcall Hook_zCVob_ThisVobAddedToWorld(zCVob* _this, void* vtable, zCWorld* a0)
	{
		Ivk_zCVob_ThisVobAddedToWorld(_this, a0);

		if (IsBurningTorch(_this))
			torches.InsertEnd((oCItem*)_this);
	}

	void __fastcall Hook_zCVob_ThisVobRemovedFromWorld(zCVob*, void*, zCWorld*);
	COptionInvoke<void(__thiscall*)(zCVob*, zCWorld*), bool> Ivk_zCVob_ThisVobRemovedFromWorld(ZenDef<TInstance>(0x005D66D0, 0x005F5C10, 0x005FAE70, 0x00601CA0), &Hook_zCVob_ThisVobRemovedFromWorld, IvkEnabled(ENGINE), Settings::SaveTorches);
	void __fastcall Hook_zCVob_ThisVobRemovedFromWorld(zCVob* _this, void* vtable, zCWorld* a0)
	{
		Ivk_zCVob_ThisVobRemovedFromWorld(_this, a0);
		
		if (IsBurningTorch(_this))
			torches.Remove((oCItem*)_this);
	}

	int __fastcall Hook_oCNpc_UseItem(oCNpc*, void*, oCItem*);
	COptionInvoke<int(__thiscall*)(oCNpc*, oCItem*), bool> Ivk_oCNpc_UseItem(ZenDef<TInstance>(0x00698810, 0x006CA590, 0x006DD450, 0x0073BC10), &Hook_oCNpc_UseItem, IvkEnabled(ENGINE), Settings::SaveTorches);
	int __fastcall Hook_oCNpc_UseItem(oCNpc* _this, void* vtable, oCItem* a0)
	{
		int result = Ivk_oCNpc_UseItem(_this, a0);

		if (IsBurningTorch(_this->GetSlotItem(NPC_NODE_LEFTHAND)) && a0 && a0->HasFlag(ITM_FLAG_TORCH))
			TryExchangeTorch(_this);

		return result;
	}
}