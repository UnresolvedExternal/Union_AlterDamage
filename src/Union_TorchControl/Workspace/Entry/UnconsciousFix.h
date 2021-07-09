namespace NAMESPACE
{
	void __fastcall Hook_oCNpc_DropUnconscious(oCNpc*, void*, float, oCNpc*);
	Hook<void(__thiscall*)(oCNpc*, float, oCNpc*), ActiveOption<bool>> Ivk_oCNpc_DropUnconscious(ZENFOR(0x00692C10, 0x006C42F0, 0x006D7750, 0x00735EB0), &Hook_oCNpc_DropUnconscious, HookMode::Patch, Settings::UnconsciousFix);
	void __fastcall Hook_oCNpc_DropUnconscious(oCNpc* _this, void* vtable, float a0, oCNpc* a1)
	{
		if (_this->interactMob)
			_this->interactMob->InterruptInteraction(_this);

		Ivk_oCNpc_DropUnconscious(_this, a0, a1);
	}
}
