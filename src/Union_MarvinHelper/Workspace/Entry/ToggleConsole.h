namespace NAMESPACE
{
	COptionSubscription<CKeyCombo> toggleConsole(ZSUB(Loop), Settings::KeyConsoleToggle, []()
		{
			if (ogame && Settings::KeyConsoleToggle->GetToggled())
				Game_OpenConsole();
		});
}