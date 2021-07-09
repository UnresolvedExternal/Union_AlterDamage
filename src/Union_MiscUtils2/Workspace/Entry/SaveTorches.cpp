namespace NAMESPACE
{
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

			for (oCItem* item : ogame->GetGameWorld()->voblist_items)
				if (IsBurningTorch(item) && item->globalVobTreeNode)
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
			oCWorld* world = ogame->GetGameWorld();
			bool didPut = false;

			std::vector<oCItem*> torches;

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

				if (!item->HasFlag(ITM_FLAG_NFOCUS))
					torches.push_back(item);
			}

			for (oCItem* torch : torches)
			{
				zVEC3 pos = torch->GetPositionWorld();
				world->RemoveVobSubtree(torch);
				world->RemoveVob(torch);

				ZOwner<oCItem> newTorch = (ZOwner<oCItem>)(oCItem*)world->CreateVob_novt(zVOB_TYPE_ITEM, "ITLSTORCHBURNING");
				newTorch->SetPositionWorld(pos);
				world->AddVob(newTorch.get());
			}

			if (worldChange)
				TryExchangeTorch(player);

			worldChange = false;
		});

	COptionSubscription<bool> removeFarTorches(ZSUB(Loop), Settings::SaveTorches, []()
		{
			if (!playerLastPosChanged)
				return;

			oCWorld* world = ogame->GetGameWorld();
			zCVob* cam = ogame->GetCameraVob();

			if (!world || !cam)
				return;

			for (oCItem* oldTorch : world->voblist_items)
			{
				if (!IsBurningTorch(oldTorch))
					continue;

				if (oldTorch->globalVobTreeNode && !oldTorch->HasFlag(ITM_FLAG_NFOCUS) && cam->GetDistanceToVob(*oldTorch) > ogame->GetSpawnManager()->GetRemoveRange())
				{
					world->RemoveVobSubtree(oldTorch);
					world->RemoveVob(oldTorch);
				}
			}
		});

	COptionSubscription<bool> exchangeTorchOnTeleport(ZSUB(Loop), Settings::ExchangeTorchOnTeleport, []()
		{
			if (playerLastPosChanged && distanceChange > ogame->GetSpawnManager()->GetRemoveRange() / 2)
				TryExchangeTorch(player);
		});

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