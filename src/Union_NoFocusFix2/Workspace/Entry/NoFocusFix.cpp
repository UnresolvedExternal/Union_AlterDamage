namespace NAMESPACE
{
	bool IsValidPlayerFocus(zCVob* vob)
	{
		if (!vob)
			return true;

		oCItem* item = vob->CastTo<oCItem>();
		if (item)
			return !item->HasFlag(ITM_FLAG_NFOCUS);

		oCNpc* npc = vob->CastTo<oCNpc>();
		if (npc)
		{
			return
				npc != player && !npc->HasFlag(NPC_FLAG_NFOCUS)
#if ENGINE >= Engine_G2
				&& !npc->noFocus
#endif
				;
		}

		return true;
	}

	int __fastcall Hook_oCNpc_FocusCheck(oCNpc*, void*, zCVob const*, int, int, float, float&);
	CInvoke<int(__thiscall*)(oCNpc*, zCVob const*, int, int, float, float&)> Ivk_oCNpc_FocusCheck(ZenDef<TInstance>(0x006905D0, 0x006C19C0, 0x006D4BA0, 0x007331C0), &Hook_oCNpc_FocusCheck, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_FocusCheck(oCNpc* _this, void* vtable, zCVob const* a0, int a1, int a2, float a3, float& a4)
	{
		if (_this == player && !IsValidPlayerFocus(const_cast<zCVob*>(a0)))
			return false;
		return Ivk_oCNpc_FocusCheck(_this, a0, a1, a2, a3, a4);
	}

	int __fastcall Hook_oCNpc_FocusCheckBBox(oCNpc*, void*, zCVob const*, int, int, float, float&);
	CInvoke<int(__thiscall*)(oCNpc*, zCVob const*, int, int, float, float&)> Ivk_oCNpc_FocusCheckBBox(ZenDef<TInstance>(0x00690350, 0x006C16E0, 0x006D4920, 0x00732F40), &Hook_oCNpc_FocusCheckBBox, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_FocusCheckBBox(oCNpc * _this, void* vtable, zCVob const* a0, int a1, int a2, float a3, float& a4)
	{
		if (_this == player && !IsValidPlayerFocus(const_cast<zCVob*>(a0)))
			return false;
		return Ivk_oCNpc_FocusCheckBBox(_this, a0, a1, a2, a3, a4);
	}

	bool filterVobs;

	void __fastcall Hook_oCNpc_GetNearestValidVob(oCNpc*, void*, float);
	CInvoke<void(__thiscall*)(oCNpc*, float)> Ivk_oCNpc_GetNearestValidVob(ZenDef<TInstance>(0x00691410, 0x006C2A00, 0x006D5ED0, 0x007344F0), &Hook_oCNpc_GetNearestValidVob, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_GetNearestValidVob(oCNpc * _this, void* vtable, float a0)
	{
		auto scope = AssignTemp(filterVobs, true);
		if (_this != player)
			scope.Reset();
		Ivk_oCNpc_GetNearestValidVob(_this, a0);
	}

	void __fastcall Hook_oCNpc_GetNearestVob(oCNpc*, void*, float);
	CInvoke<void(__thiscall*)(oCNpc*, float)> Ivk_oCNpc_GetNearestVob(ZenDef<TInstance>(0x00691720, 0x006C2D20, 0x006D6240, 0x00734860), &Hook_oCNpc_GetNearestVob, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_GetNearestVob(oCNpc * _this, void* vtable, float a0)
	{
		auto scope = AssignTemp(filterVobs, true);
		if (_this != player)
			scope.Reset();
		Ivk_oCNpc_GetNearestVob(_this, a0);
	}

	void __fastcall Hook_oCNpc_ToggleFocusVob(oCNpc*, void*, int);
	CInvoke<void(__thiscall*)(oCNpc*, int)> Ivk_oCNpc_ToggleFocusVob(ZenDef<TInstance>(0x00690910, 0x006C1D70, 0x006D4F90, 0x007335B0), &Hook_oCNpc_ToggleFocusVob, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_ToggleFocusVob(oCNpc * _this, void* vtable, int a0)
	{
		auto scope = AssignTemp(filterVobs, true);
		if (_this != player)
			scope.Reset();
		Ivk_oCNpc_ToggleFocusVob(_this, a0);
	}

	// WARNING: supported versions are G1, G1A
	void __fastcall Hook_oCNpc_CollectFocusVob(oCNpc*, void*);
	CInvoke<void(__thiscall*)(oCNpc*)> Ivk_oCNpc_CollectFocusVob(ZenDef<TInstance>(0x00690D70, 0x006C2210, 0x00000000, 0x00000000), &Hook_oCNpc_CollectFocusVob, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_CollectFocusVob(oCNpc * _this, void* vtable)
	{
		auto scope = AssignTemp(filterVobs, true);
		if (_this != player)
			scope.Reset();
		Ivk_oCNpc_CollectFocusVob(_this);
	}

	// WARNING: supported versions are G2, G2A
	void __fastcall Hook_oCNpc_CollectFocusVob_2(oCNpc*, void*, int);
	CInvoke<void(__thiscall*)(oCNpc*, int)> Ivk_oCNpc_CollectFocusVob_2(ZenDef<TInstance>(0x00000000, 0x00000000, 0x006D53F0, 0x00733A10), &Hook_oCNpc_CollectFocusVob_2, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_CollectFocusVob_2(oCNpc * _this, void* vtable, int a0)
	{
		auto scope = AssignTemp(filterVobs, true);
		if (_this != player)
			scope.Reset();
		Ivk_oCNpc_CollectFocusVob_2(_this, a0);
	}

	void __fastcall Hook_zCBspBase_CollectVobsInBBox3D(zCBspBase*, zCArray<zCVob*>&, zTBBox3D const&);
	CInvoke<void(__fastcall*)(zCBspBase*, zCArray<zCVob*>&, zTBBox3D const&)> Ivk_zCBspBase_CollectVobsInBBox3D(ZenDef<TInstance>(0x0051E7C0, 0x005345A0, 0x0052E1A0, 0x00531110), &Hook_zCBspBase_CollectVobsInBBox3D, IvkEnabled(ENGINE));
	void __fastcall Hook_zCBspBase_CollectVobsInBBox3D(zCBspBase * _this, zCArray<zCVob*> & vobs, zTBBox3D const& bbox)
	{
		int lastSize = vobs.GetNum();
		Ivk_zCBspBase_CollectVobsInBBox3D(_this, vobs, bbox);

		if (!filterVobs)
			return;

		for (int i = lastSize; i < vobs.GetNum(); i++)
			if (!IsValidPlayerFocus(vobs[i]))
				vobs.RemoveIndex(i--);
	}

	CSubscription resetFocus(TGameEvent::Loop, []()
		{
			if (!IsValidPlayerFocus(player->focus_vob))
				player->ClearFocusVob();
			if (!IsValidPlayerFocus(player->enemy))
				player->SetEnemy(nullptr);
		}, CHECK_THIS_ENGINE);
}