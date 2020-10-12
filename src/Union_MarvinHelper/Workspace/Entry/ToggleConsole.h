namespace NAMESPACE
{
	CInvoke<void(__cdecl*)()> Ivk__Game_OpenConsole(ZenDef<TInstance>(0x00647120, 0x0066EDB0, 0x00675310, 0x006D2090), nullptr, IVK_DISABLED);

	COptionSubscription<CKeyCombo> toggleConsole(ZSUB(Loop), Settings::KeyConsoleToggle, []()
		{
			if (ogame && !ogame->singleStep && Settings::KeyConsoleToggle->GetToggled())
				Ivk__Game_OpenConsole();
		});
}