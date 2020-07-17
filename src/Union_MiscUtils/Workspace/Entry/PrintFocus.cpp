namespace NAMESPACE
{
	zSTRING GetFocusText(oCItem* item)
	{
		zSTRING text;

		if (item->mainflag & settings.nameToDescFlags)
			text += item->description;
		else
			text += item->name;

		if (settings.appendAmountInfo && item->amount != 1)
		{
			text += " (";
			text += settings.xChar.GetVector();
			text += item->amount;
			text += ")";
		}

		return text;
	}

	void __fastcall Hook_oCGame_UpdatePlayerStatus(oCGame*, void*);
	CInvoke<void(__thiscall*)(oCGame*)> Ivk_oCGame_UpdatePlayerStatus(ZenDef<TInstance>(0x00638F90, 0x0065F4E0, 0x00666640, 0x006C3140), &Hook_oCGame_UpdatePlayerStatus, IvkEnabled(ENGINE));
	void __fastcall Hook_oCGame_UpdatePlayerStatus(oCGame* _this, void* vtable)
	{
		if (!settings.printTorchName && !settings.appendAmountInfo && !settings.nameToDescFlags)
		{
			Ivk_oCGame_UpdatePlayerStatus(_this);
			Ivk_oCGame_UpdatePlayerStatus.Detach();
			return;
		}

		oCItem* item = player->GetFocusVob()->CastTo<oCItem>();
		
		if (item)
		{
			if (settings.printTorchName && IsBurningTorch(item))
			{
				Print(screen, item->GetPositionWorld(), GetFocusText(item));
				Ivk_oCGame_UpdatePlayerStatus(_this);
			}
			else
			{
				auto scope = AssignTemp(item->name, GetFocusText(item));
				Ivk_oCGame_UpdatePlayerStatus(_this);
			}
		}
		else
			Ivk_oCGame_UpdatePlayerStatus(_this);
	}
}