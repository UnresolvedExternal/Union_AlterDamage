namespace NAMESPACE
{
	void __fastcall Hook_oCGame_DefineExternals_Ulfi(oCGame*, void*, zCParser*);
	ModulePatchCallInvoker<void(__thiscall*)(oCGame*, zCParser*)> Ivk_oCGame_DefineExternals_Ulfi(CHECK_THIS_ENGINE ? ZENDEF(0x006495B0, 0x006715F0, 0x00677A00, 0x006D4780) : 0x00000000, &Hook_oCGame_DefineExternals_Ulfi);
	void __fastcall Hook_oCGame_DefineExternals_Ulfi(oCGame* _this, void* vtable, zCParser* a0)
	{
		Ivk_oCGame_DefineExternals_Ulfi(_this, a0);

		if (!Settings::ActivateParserExtenderExternals)
			return;

		if (a0 != parser)
			return;

		zCParser::cur_parser = parser;

		if (HMODULE extender = CPlugin::FindModule("zParserExtender.dll"))
		{
			Array<string>&(__cdecl* getDynamicExternalFuncList)() = reinterpret_cast<decltype(getDynamicExternalFuncList)>(GetProcAddress(extender, "GetDynamicExternalFuncList"));
			bool(__cdecl* activateExternalFunc)(const string&) = reinterpret_cast<decltype(activateExternalFunc)>(GetProcAddress(extender, "ActivateExternalFunc"));

			if (getDynamicExternalFuncList && activateExternalFunc)
			{
				Array<string> externals;

				for (const string& external : getDynamicExternalFuncList())
					externals += external;

				for (const string& external : externals)
					activateExternalFunc(external);
			}
		}
	}
}