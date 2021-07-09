namespace NAMESPACE
{
	int (*innerEvalFunc)(const zSTRING&, zSTRING&);

	int ConsoleEvalFunc(const zSTRING& inpstr, zSTRING& msg)
	{
		if (innerEvalFunc && innerEvalFunc(inpstr, msg))
			return true;

		string output;
		
		if (CConsoleContext::GetInstance().TryExecute(output))
		{
			msg = output.GetVector();
			return true;
		}

		return false;
	}

	void __cdecl Hook__Game_InitConsole();
	ModulePatchCallInvoker<void(__cdecl*)()> Ivk__Game_InitConsole(ZENFOR(0x00645280, 0x0066CBA0, 0x00673470, 0x006D01F0), &Hook__Game_InitConsole, IvkEnabled(ENGINE));
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
		context.AddCommand<CShowLowfpsCommand>();
		context.AddCommand<CDecompileCommand>();
		context.AddCommand<CShowCursorCommand>();
		context.AddCommand<CHideCursorCommand>();
		context.AddCommand<CPrintWeaponsCommand>();
		context.AddCommand<CGoRouteCommand>();		
		
#pragma region Zuku05

		context.AddCommand<CWpConnectCommand>();
		context.AddCommand<CWpAddCommand>();
		context.AddCommand<CWpRemoveCommand>();
		context.AddCommand<CWpRenameCommand>();
		context.AddCommand<CWpMoveCommand>();
		context.AddCommand<CFpAddCommand>();
		context.AddCommand<CFpRemoveCommand>();
		context.AddCommand<CFpRenameCommand>();
		context.AddCommand<CFpMoveCommand>();
		context.AddCommand<CWpLinkingCommand>();

#pragma endregion

		int evalNum = 0;

		for (int i = 1; i < zCON_MAX_EVAL; i++)
			if (zcon->evalfunc[i])
				evalNum = i;

		innerEvalFunc = zcon->evalfunc[evalNum];
		zcon->evalfunc[evalNum] = &ConsoleEvalFunc;

		Ivk__Game_InitConsole();
	}
}