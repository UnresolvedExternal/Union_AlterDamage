namespace NAMESPACE
{
	int ConsoleEvalFunc(const zSTRING& inpstr, zSTRING& msg)
	{
		string output;
		
		if (CConsoleContext::GetInstance().TryExecute(output))
		{
			msg = output.GetVector();
			return true;
		}

		return false;
	}

	void __cdecl Hook__Game_InitConsole();
	CInvoke<void(__cdecl*)()> Ivk__Game_InitConsole(ZenDef<TInstance>(0x00645280, 0x0066CBA0, 0x00673470, 0x006D01F0), &Hook__Game_InitConsole, IvkEnabled(ENGINE));
	void __cdecl Hook__Game_InitConsole()
	{
		CConsoleContext& context = CConsoleContext::GetInstance();
		
		context.AddCommand<CConDatCommand>();
		context.AddCommand<CShowVarsCommand>();
		context.AddCommand<CHideVarsCommand>();
		context.AddCommand<CSetVarCommand>();
		context.AddCommand<CCallCommand>();
		context.AddCommand<CShowCallCommand>();
		context.AddCommand<CGotoWaypointCommand>();
		context.AddCommand<CGotoVobCommand>();
		context.AddCommand<CInsertCommand>();
		context.AddCommand<CExecuteCommand>();
		context.AddCommand<CSaveShowListCommand>();

		zcon->AddEvalFunc(&ConsoleEvalFunc);
		Ivk__Game_InitConsole();
	}
}