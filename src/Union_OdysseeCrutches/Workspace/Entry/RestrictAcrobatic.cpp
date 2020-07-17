namespace NAMESPACE
{
	int __fastcall Hook_oCNpc_ApplyOverlay(oCNpc*, void*, zSTRING const&);
	CInvoke<int(__thiscall*)(oCNpc*, zSTRING const&)> Ivk_oCNpc_ApplyOverlay(ZenDef<TInstance>(0x0068AD40, 0x006BB2D0, 0x006CF0C0, 0x0072D2C0), &Hook_oCNpc_ApplyOverlay, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_ApplyOverlay(oCNpc* _this, void* vtable, zSTRING const& name)
	{
		if (settings.restrictAcrobatic && name.CompareI("HUMANS_ACROBATIC.MDS"))
		{
			zCModel* model = _this->GetModel();
			if (model && model->modelProtoList.GetNum() && !model->modelProtoList[0]->modelProtoName.CompareI("HUMANS"))
				return false;
		}

		return Ivk_oCNpc_ApplyOverlay(_this, name);
	}
}