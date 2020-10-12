namespace NAMESPACE
{
	namespace Settings
	{
		ZOPTION(ConsoleFreezePlayer, true);
		ZOPTION(ConsoleShowHints, true);
		ZOPTION(ConsoleEditItemAmount, true);
		ZOPTION(ConsoleKeepHistoryCase, true);

		ZOPTION(KeySelectionPrev, CKeyCombo({ {KEY_UP}, {MOUSE_WHEELUP} }));
		ZOPTION(KeySelectionNext, CKeyCombo({ {KEY_DOWN}, {MOUSE_WHEELDOWN} }));
		ZOPTION(KeySelectionInsert, CKeyCombo({ {KEY_TAB}, {KEY_RIGHT}, {MOUSE_BUTTONLEFT} }));
		ZOPTION(KeyConsoleToggle, CKeyCombo({ {KEY_GRAVE}, {KEY_LSHIFT, KEY_F2} }));
		ZOPTION(KeyConsoleDeleteWord, CKeyCombo({ {KEY_RCONTROL, KEY_BACK}, {KEY_LCONTROL, KEY_BACK}, {KEY_LEFT} }));

		ZOPTION(ChangeSelectionThrottleMs, 50);

		ZOPTION(SkipInputFrames, 2);
	}

	namespace Settings
	{
		void Load()
		{
			CSingleOptionBase::LoadAll();
		}
	}
}