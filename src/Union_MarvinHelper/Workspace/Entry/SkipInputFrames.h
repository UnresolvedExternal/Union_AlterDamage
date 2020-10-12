namespace NAMESPACE
{
	void __fastcall Hook_zCInput_Win32_ProcessInputEvents(zCInput_Win32*, void*);
	COptionInvoke<void(__thiscall*)(zCInput_Win32*), int> Ivk_zCInput_Win32_ProcessInputEvents(ZenDef<TInstance>(0x004C8BA0, 0x004D8CC0, 0x004D3140, 0x004D5700), &Hook_zCInput_Win32_ProcessInputEvents, IvkEnabled(ENGINE), Settings::SkipInputFrames);
	void __fastcall Hook_zCInput_Win32_ProcessInputEvents(zCInput_Win32* _this, void* vtable)
	{
		Ivk_zCInput_Win32_ProcessInputEvents(_this);

		static int skipCounter;

		if (hWndApp != GetFocus())
			skipCounter = Settings::SkipInputFrames;

		if (skipCounter)
		{
			skipCounter -= 1;
			_this->ClearKeyBuffer();
		}
	}
}