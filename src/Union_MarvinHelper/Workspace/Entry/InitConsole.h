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

	void RegisterEvalFunc()
	{
		int evalNum = 0;

		for (int i = 1; i < zCON_MAX_EVAL; i++)
			if (zcon->evalfunc[i])
				evalNum = i;

		innerEvalFunc = zcon->evalfunc[evalNum];
		zcon->evalfunc[evalNum] = &ConsoleEvalFunc;
	}

	void RegisterCommands()
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

#pragma region zuku05

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

		context.AddCommand<CPlayAniCommand>();
		context.AddCommand<CPlayFaceAniCommand>();
		context.AddCommand<CAiGotoCommand>();
		context.AddCommand<CShowAnictrlCommand>();
	}

	void InitConsole()
	{
		RegisterEvalFunc();
		RegisterCommands();

		if (ogame && Settings::ActivateMarvinMode)
			ogame->game_testmode = true;

		if (zcon && Settings::ExecuteStartupScript)
		{
			zSTRING text = zcon->instr;
			auto scope = AssignTemp(CConsoleContext::GetInstance().GetCommand(), CConsoleContext::GetInstance().GetCommand());
			CConsoleContext::GetInstance().GetCommand().Parse("execute startup");
			zcon->Evaluate("execute startup");
			zcon->instr = text;
		}
	}

	extern CSubscription initConsole;
	CSubscription initConsole(ZSUB(PreLoop), []()
		{
			if (!zcon)
				return;

			initConsole.Reset();
			InitConsole();
		});
}