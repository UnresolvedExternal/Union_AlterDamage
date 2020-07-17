namespace NAMESPACE
{
	bool inReapplyOverlay;

	void ReapplyOverlays(oCNpc* npc)
	{
		if (inReapplyOverlay)
			return;

		auto scope = AssignTemp(inReapplyOverlay, true);
		zCModel* model = nullptr;
		oCAniCtrl_Human* anictrl = nullptr;

		for (const zSTRING& overlay : settings.reapplyOverlays)
		{
			if (npc->activeOverlays.IsInList(overlay))
			{
				npc->RemoveOverlay(overlay);
				npc->ApplyOverlay(overlay);
				continue;
			}

			for (oCNpc::oCNpcTimedOverlay* timedOverlay : npc->timedOverlays)
				if (timedOverlay && timedOverlay->mdsOverlayName.CompareI(overlay))
				{
					model = model ? model : npc->GetModel();
					if (!model)
						break;

					anictrl = anictrl ? anictrl : npc->anictrl;

					COA2(model, RemoveModelProtoOverlay(overlay));
					COA2(anictrl, InitAnimations());
					COA2(model, ApplyModelProtoOverlay(overlay));
					COA2(anictrl, InitAnimations());
					break;
				}
		}
	}

	int __fastcall Hook_oCNpc_ApplyOverlay_Reapply(oCNpc*, void*, zSTRING const&);
	CInvoke<int(__thiscall*)(oCNpc*, zSTRING const&)> Ivk_oCNpc_ApplyOverlay_Reapply(ZenDef<TInstance>(0x0068AD40, 0x006BB2D0, 0x006CF0C0, 0x0072D2C0), &Hook_oCNpc_ApplyOverlay_Reapply, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_ApplyOverlay_Reapply(oCNpc* _this, void* vtable, zSTRING const& a0)
	{
		int result = Ivk_oCNpc_ApplyOverlay_Reapply(_this, a0);
		if (result)
			ReapplyOverlays(_this);
		return result;
	}

	int __fastcall Hook_oCNpc_ApplyTimedOverlayMds(oCNpc*, void*, zSTRING const&, float);
	CInvoke<int(__thiscall*)(oCNpc*, zSTRING const&, float)> Ivk_oCNpc_ApplyTimedOverlayMds(ZenDef<TInstance>(0x006B0C60, 0x006E4C40, 0x006F7A60, 0x00756890), &Hook_oCNpc_ApplyTimedOverlayMds, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_ApplyTimedOverlayMds(oCNpc* _this, void* vtable, zSTRING const& a0, float a1)
	{
		int result = Ivk_oCNpc_ApplyTimedOverlayMds(_this, a0, a1);
		if (result)
			ReapplyOverlays(_this);
		return result;
	}
}