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
			text += " (x";
			text += item->amount;
			text += ")";
		}

		return text;
	}

	void __fastcall Hook_oCGame_UpdatePlayerStatus(oCGame*, void*);
	CInvoke<void(__thiscall*)(oCGame*)> Ivk_oCGame_UpdatePlayerStatus(ZenDef<TInstance>(0x00638F90, 0x0065F4E0, 0x00666640, 0x006C3140), &Hook_oCGame_UpdatePlayerStatus, IvkEnabled(CurrentEngine));
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
				zVEC3 pos = item->GetPositionWorld();
				pos = ogame->GetCamera()->GetTransform(zCAM_TRAFO_VIEW) * pos;

				if (pos[2] >= 0.0f)
				{
					int x, y;
					ogame->GetCamera()->Project(&pos, x, y);
					x = screen->anx(x);
					y = screen->any(y);

					zSTRING text = GetFocusText(item);
					float width = screen->FontSize(text);
					float height = screen->FontY();
					x -= width / 2;
					y -= height / 2;
					x = CoerceInRange(x, width, 0, 8192);
					y = CoerceInRange(y, height, 0, 8192);

					screen->Print(x + 0.5f, y + 0.5f, text);
				}

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