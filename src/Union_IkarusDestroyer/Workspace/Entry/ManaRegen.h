namespace NAMESPACE
{
	CSubscription regenerateMana(ZSUB(Loop), []()
		{
			if (!ogame || ogame->singleStep || !player || player->attribute[NPC_ATR_HITPOINTS] <= 0)
				return;

			static int timer = 0;
			timer += ztimer->frameTime;

			if (timer < Settings::TickRate)
				return;

			timer -= Settings::TickRate;

			const int manaMax = player->attribute[NPC_ATR_MANAMAX];
			int& mana = player->attribute[NPC_ATR_MANA];

			if (manaMax < Settings::Threshold)
				return;

			mana += Settings::PerTick ? Settings::PerTick : (manaMax / Settings::Divisor);
			mana = CoerceInRange(mana, 0, 0, manaMax);
		});
}