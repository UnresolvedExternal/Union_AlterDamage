namespace NAMESPACE
{
	bool IsBurningTorch(zCVob* vob)
	{
		static int instance = parser->GetIndex("ITLSTORCHBURNING");

		if (instance < 0)
			return false;

		return vob && vob->GetVobType() == zVOB_TYPE_ITEM && static_cast<oCItem*>(vob)->instanz == instance;
	}

	bool TryExchangeTorch(oCNpc* npc)
	{
		oCWorld* world = ogame->GetWorld()->CastTo<oCWorld>();

		if (!world)
			return false;

		oCItem* torch = npc->GetSlotItem(NPC_NODE_LEFTHAND);

		if (!IsBurningTorch(torch))
			return false;

		int instance = torch->instanz;
		npc->DropFromSlot(NPC_NODE_LEFTHAND);
		world->RemoveVobSubtree(torch);
		world->RemoveVob(torch);

		torch = static_cast<oCItem*>(world->CreateVob(zVOB_TYPE_ITEM, instance));

		if (!torch)
			return false;

		world->AddVob(torch);
		npc->PutInSlot(NPC_NODE_LEFTHAND, torch, 0);
		torch->Release();

		return true;
	}

	class TorchData : public SaveData
	{
	public:
		TorchData(const string& name) :
			SaveData{ name }
		{

		}

		virtual void Clear() override
		{

		}

		virtual void Archive(zCArchiver& arc) override
		{
			arc.WriteBool("PlayerHasTorch", IsBurningTorch(COA(player, GetLeftHand())));

			std::vector<oCItem*> torches;
			torches.reserve(64u);

			for (oCItem* item : ogame->GetGameWorld()->voblist_items)
				if (IsBurningTorch(item) && !item->HasFlag(ITM_FLAG_NFOCUS) && item->homeWorld == ogame->world)
					torches += item;

			arc.WriteInt("Count", torches.size());

			for (oCItem* torch : torches)
			{
				torch->dontWriteIntoArchive = true;
				arc.WriteRaw("Matrix", &torch->trafoObjToWorld, sizeof(torch->trafoObjToWorld));
			}
		}

		virtual void Unarchive(zCArchiver& arc) override
		{
			oCWorld* world = ogame->GetGameWorld();
			int instance = parser->GetIndex("ITLSTORCHBURNING");

			if (instance < 0)
				return;

			bool playerHasTorch = arc.ReadBool("PlayerHasTorch");

			if (player && playerHasTorch && !player->GetLeftHand())
				if (ZOwner<oCItem> torch{ static_cast<oCItem*>(world->CreateVob(zVOB_TYPE_ITEM, instance)) })
				{
					world->AddVob(torch.get());
					player->PutInSlot(NPC_NODE_LEFTHAND, torch.get(), 0);
				}

			int count = arc.ReadInt("Count");

			for (int i = 0; i < count; i++)
				if (ZOwner<oCItem> torch{ static_cast<oCItem*>(world->CreateVob(zVOB_TYPE_ITEM, instance)) })
				{
					zMAT4 mat;
					arc.ReadRaw("Matrix", &mat, sizeof(mat));
					torch->SetTrafoObjToWorld(mat);
					world->AddVob(torch.get());
				}
				else
					break;
		}
	};

	oCItem* __fastcall Hook_oCNpc_PutInInv(oCNpc*, void*, oCItem*);
	Hook<oCItem*(__thiscall*)(oCNpc*, oCItem*), ActiveOption<bool>> Ivk_oCNpc_PutInInv(ZENFOR(0x006A4FF0, 0x006D7AC0, 0x006EA870, 0x00749350), &Hook_oCNpc_PutInInv, HookMode::Patch, Settings::TakeTorchFix);
	oCItem* __fastcall Hook_oCNpc_PutInInv(oCNpc* _this, void* vtable, oCItem* a0)
	{
		if (a0)
			a0->AddRef();

		oCItem* result = Ivk_oCNpc_PutInInv(_this, a0);

		if (a0)
		{
			ogame->GetGameWorld()->RemoveVobSubtree(a0);
			ogame->GetGameWorld()->RemoveVob(a0);
			a0->Release();
		}

		return result;
	}

	Sub saveTorches(ZSUB(GameEvent::SaveBegin), Settings::SaveTorches, []()
		{
			SaveData::Get<TorchData>(ogame->GetGameWorld()->GetWorldName() + ".Torches").Save(GameEvent::SaveBegin);
		});

	Sub loadTorches(ZSUB(GameEvent::LoadEnd), Settings::SaveTorches, []()
		{
			SaveData::Get<TorchData>(ogame->GetGameWorld()->GetWorldName() + ".Torches").Load(GameEvent::LoadEnd);
		});

	Sub clearTorches(ZSUB(GameEvent::LoadBegin_ChangeLevel), Settings::TorchRemoveRange, []()
		{
			std::vector<oCItem*> torches;
			torches.reserve(64u);

			for (oCItem* item : ogame->GetGameWorld()->voblist_items)
				if (IsBurningTorch(item) && !item->HasFlag(ITM_FLAG_NFOCUS))
					torches.push_back(item);

			for (oCItem* torch : torches)
			{
				torch->AddRef();
				ogame->GetGameWorld()->RemoveVobSubtree(torch);
				ogame->GetGameWorld()->RemoveVob(torch);
				torch->Release();
			}
		});

	Sub removeFarTorches(ZSUB(GameEvent::Loop), Settings::TorchRemoveRange, []()
		{
			static Timer timer;
			
			if (!ogame->GetCameraVob() || !timer[0u].Await(1000u))
				return;

			oCWorld* world = ogame->GetGameWorld();

			std::vector<oCItem*> torches;
			torches.reserve(64u);

			for (oCItem* item : ogame->GetGameWorld()->voblist_items)
				if (IsBurningTorch(item) && !item->HasFlag(ITM_FLAG_NFOCUS) && item->GetDistanceToVobApprox(*ogame->GetCameraVob()) > Settings::TorchRemoveRange)
					torches.push_back(item);

			for (oCItem* torch : torches)
			{
				torch->AddRef();
				world->RemoveVobSubtree(torch);
				world->RemoveVob(torch);
				torch->Release();
			}
		});

	Sub exchangeTorchOnTeleport(ZSUB(GameEvent::Loop), Settings::ExchangeTorchOnTeleport, []()
		{
			if (!player)
				return;

			static zVEC3 lastPos = player->GetPositionWorld();
			zVEC3 pos = player->GetPositionWorld();

			if (lastPos.Distance(pos) > 500.0f)
				TryExchangeTorch(player);

			lastPos = pos;
		});

	Sub exchangeTorchOnLevelChange(ZSUB(GameEvent::LoadEnd_ChangeLevel), Settings::ExchangeTorchOnTeleport, []()
		{
			if (player)
				TryExchangeTorch(player);
		});
}
