namespace NAMESPACE
{
	// WARNING: supported versions are G2, G2A
	CInvoke<void(__cdecl*)(short, zSTRING, zCArray<unsigned short>&)> Ivk___GetDefaultBinding(ZenDef<TInstance>(0x00000000, 0x00000000, 0x004CD6A0, 0x004CFC00), nullptr, IVK_DISABLED);

	CSubscription useTorch(TGameEvent::Loop, []()
		{
			if (ogame->singleStep) 
				return;
			if (player->anictrl && player->anictrl->IsInWeaponChoose()) 
				return;
			if (player->interactItem)
				return;
			if (player->interactMob)
				return;
			if (player->inventory2.IsOpen())
				return;
			if (!player->IsHuman())
				return;

			int bodyState = player->GetBodyState();

			if (bodyState == BS_JUMP || bodyState == BS_FALL || bodyState == BS_SWIM ||
				bodyState == BS_DIVE || player->IsUnconscious() || player->attribute[NPC_ATR_HITPOINTS] <= 0)
			{
				return;
			}
			if (player->GetWeaponMode() != NPC_WEAPON_NONE)
				return;
			if (!player->GetEM(true)->IsEmpty(false))
				return;

			int index = parser->GetIndex("ITLSTORCH");
			if (index < 0)
				return;

			if (!player->HasTorch())
			{
				if (zinput->GetToggled(settings.logicalKey))
					player->UseItem(player->IsInInv(index, 1));
				return;
			}

			if (!zinput->GetToggled(settings.logicalKey))
				return;

			player->UseItem(player->PutInInv(index, 1));
			player->RemoveFromInv(index, 1);
			player->SetTorchAni(true, true);
			player->SetTorchAni(false, true);
		}, CHECK_THIS_ENGINE);

	void __fastcall Hook_zCInput_BindKeys(zCInput*, void*, int);
	CInvoke<void(__thiscall*)(zCInput*, int)> Ivk_zCInput_BindKeys(ZenDef<TInstance>(0x004C5930, 0x004D5420, 0x004CD890, 0x004CFE00), &Hook_zCInput_BindKeys, IvkEnabled(ENGINE));
	void __fastcall Hook_zCInput_BindKeys(zCInput* _this, void* vtable, int mode)
	{
		settings.LazyLoad();
		Ivk_zCInput_BindKeys(_this, mode);

		zCArray<unsigned short> ctrls;
		ctrls.DeleteList();
		ctrls.Insert(KEY_T);
		ctrls.Insert(KEY_NUMPAD9);
#if ENGINE >= Engine_G2
		Ivk___GetDefaultBinding(mode, "keyTorch", ctrls);
#endif
		_this->BindOption("keyTorch", settings.logicalKey, ctrls);
	}
}