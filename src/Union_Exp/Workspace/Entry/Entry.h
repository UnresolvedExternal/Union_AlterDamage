namespace NAMESPACE
{
	CSub check(ZSUB(Loop), []()
		{
			if (oCMobLockable* chest = COA(player, focus_vob, CastTo<oCMobLockable>()))
			{
				int y = 4000;
				LOGS(chest->triggerTarget);
				LOGS(chest->onStateFuncName);
				LOGS(chest->conditionFunc);
				LOGS(chest->)
			}
		});
}
