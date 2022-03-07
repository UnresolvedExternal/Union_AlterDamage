namespace NAMESPACE
{
	COptionSubscription<CKeyCombo> toggleConsole(ZSUB(Loop), Settings::KeyConsoleToggle, []()
		{
			if (ogame && !ogame->singleStep && Settings::KeyConsoleToggle->GetToggled())
				Game_OpenConsole();
		});
}