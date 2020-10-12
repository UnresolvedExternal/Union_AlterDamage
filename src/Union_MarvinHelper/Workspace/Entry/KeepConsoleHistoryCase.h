namespace NAMESPACE
{
	int __fastcall Hook_zCConsole_Evaluate(zCConsole*, void*, zSTRING const&);
	COptionInvoke<int(__thiscall*)(zCConsole*, zSTRING const&), bool> Ivk_zCConsole_Evaluate(ZenDef<TInstance>(0x006DB5D0, 0x007135D0, 0x00724E20, 0x00784860), &Hook_zCConsole_Evaluate, IvkEnabled(ENGINE), Settings::ConsoleKeepHistoryCase);
	int __fastcall Hook_zCConsole_Evaluate(zCConsole* _this, void* vtable, zSTRING const& a0)
	{
		int result = Ivk_zCConsole_Evaluate(_this, a0);

		if (result && _this == zcon)
			_this->instr = CConsoleContext::GetInstance().GetCommand().ToString().GetVector();

		return result;
	}
}