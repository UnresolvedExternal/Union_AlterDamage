namespace NAMESPACE
{
	int (*innerEvalFunc)(const zSTRING&, zSTRING&);

	int ConsoleEvalFunc(const zSTRING& inpstr, zSTRING& msg)
	{
		string output;
		
		if (CConsoleContext::GetInstance().TryExecute(output))
		{
			msg = output.GetVector();
			return true;
		}

		if (innerEvalFunc && innerEvalFunc(inpstr, msg))
			return true;

		return false;
	}

	void RegisterEvalFunc()
	{
		innerEvalFunc = zcon->evalfunc[0];
		zcon->evalfunc[0] = &ConsoleEvalFunc;
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
		context.AddCommand<CPlayThemeCommand>();
		context.AddCommand<CPlayJingleCommand>();
		context.AddCommand<CSetOptionCommand>();
		context.AddCommand<CFullDecompileCommand>();
		context.AddCommand<CPlayAudio3dCommand>();
	}

	void(__cdecl* Game_OpenConsole)() = reinterpret_cast<void(__cdecl*)()>(ZENFOR(0x00647120, 0x0066EDB0, 0x00675310, 0x006D2090));

	extern CSubscription initConsole;
	CSubscription initConsole(ZSUB(LoadBegin), []()
		{
			if (!zcon)
				return;

			initConsole.Reset();
			RegisterEvalFunc();
			RegisterCommands();
		});

	extern CSubscription executeStartupScript;
	CSubscription executeStartupScript(ZSUB(LoadEnd), []()
		{
			executeStartupScript.Reset();

			if (!zcon || !Settings::ExecuteStartupScript)
				return;

			Game_OpenConsole();
			Game_OpenConsole();

			auto scope = AssignTemp(CConsoleContext::GetInstance().GetCommand(), CConsoleContext::GetInstance().GetCommand());
			CConsoleContext::GetInstance().GetCommand().Parse("EXECUTE STARTUP");
			zcon->Evaluate("EXECUTE STARTUP");
			zcon->instr = "";
		});

	extern CSubscription activateMarvinMode;
	CSubscription activateMarvinMode(ZSUB(PostLoop), []()
		{
			activateMarvinMode.Reset();
			
			if (ogame && Settings::ActivateMarvinMode)
				ogame->game_testmode = true;
		});
}