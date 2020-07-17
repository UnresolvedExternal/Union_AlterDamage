namespace NAMESPACE
{
	extern CSubscription changeMotionFactor;
	CSubscription changeMotionFactor(TGameEvent::Loop, []()
		{
			if (!settings.allowChangeSpeed)
				return changeMotionFactor.Reset();

			static CTimer timer;
			timer.Attach();

			float currentFactor = ztimer->factorMotion;
			float newFactor = currentFactor;

			if (IsLogicalPressed(GAME_SLOW) && zinput->KeyPressed(KEY_ADD) && timer(0, 200, TM_PRIMARY))
				newFactor = (currentFactor < 1.0f) ? MIN(1.0f, currentFactor + 0.2f) : MIN(5.0f, currentFactor + 1.0f);

			if (IsLogicalPressed(GAME_SLOW) && zinput->KeyPressed(KEY_SUBTRACT) && timer(0, 200, TM_PRIMARY))
				newFactor = (currentFactor <= 1.0f) ? MAX(0.2f, currentFactor - 0.2f) : MAX(1.0f, currentFactor - 1.0f);

			if (newFactor == currentFactor)
				return;

			ztimer->SetMotionFactor(newFactor);
			COA3(ogame, GetTextView(), PrintTimedCXY(newFactor, 200.0f, nullptr));
		}, CHECK_THIS_ENGINE);
}
