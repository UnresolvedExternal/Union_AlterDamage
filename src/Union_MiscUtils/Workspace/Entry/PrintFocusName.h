namespace NAMESPACE
{
	zSTRING GetFocusText(oCItem* item)
	{
		zSTRING text;

		if (Settings::Cats.find(item->mainflag) != Settings::Cats.end())
			text += item->description;
		else
			text += item->name;

		if (Settings::AppendAmountInfo && item->amount != 1)
		{
			text += " (";
			text += Settings::XChar->GetVector();
			text += item->amount;
			text += ")";
		}

		return text;
	}

	void __fastcall Hook_oCGame_UpdatePlayerStatus(oCGame*, void*);
	Hook<void(__thiscall*)(oCGame*), ActiveValue<bool>> Ivk_oCGame_UpdatePlayerStatus(ZENFOR(0x00638F90, 0x0065F4E0, 0x00666640, 0x006C3140), &Hook_oCGame_UpdatePlayerStatus, HookMode::Patch, Settings::HookUpdatePlayerStatus);
	void __fastcall Hook_oCGame_UpdatePlayerStatus(oCGame* _this, void* vtable)
	{
		if (oCItem* item = COA(player, GetFocusVob(), CastTo<oCItem>()))
		{
			if (Settings::PrintTorchName && IsBurningTorch(item))
			{
				Print(screen, item->GetPositionWorld(), GetFocusText(item));
				Ivk_oCGame_UpdatePlayerStatus(_this);
			}
			else
			{
				auto scope = AssignTemp(item->name, GetFocusText(item));
				Ivk_oCGame_UpdatePlayerStatus(_this);
			}

			return;
		}

		Ivk_oCGame_UpdatePlayerStatus(_this);
	}
}
