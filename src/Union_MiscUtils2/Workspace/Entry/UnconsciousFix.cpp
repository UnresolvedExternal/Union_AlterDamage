namespace NAMESPACE
{
	void __fastcall Hook_oCNpc_DropUnconscious(oCNpc*, void*, float, oCNpc*);
	COptionInvoke<void(__thiscall*)(oCNpc*, float, oCNpc*), bool> Ivk_oCNpc_DropUnconscious(ZenDef<TInstance>(0x00692C10, 0x006C42F0, 0x006D7750, 0x00735EB0), &Hook_oCNpc_DropUnconscious, IvkEnabled(ENGINE), Settings::UnconsciousFix);
	void __fastcall Hook_oCNpc_DropUnconscious(oCNpc* _this, void* vtable, float a0, oCNpc* a1)
	{
		if (_this->interactMob)
			_this->interactMob->InterruptInteraction(_this);
		Ivk_oCNpc_DropUnconscious(_this, a0, a1);
	}
}