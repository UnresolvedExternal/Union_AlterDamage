namespace NAMESPACE
{
	void __fastcall Hook_oCNpc_TurnToEnemy(oCNpc*, void*);
	CInvoke<void(__thiscall*)(oCNpc*)> Ivk_oCNpc_TurnToEnemy(ZenDef<TInstance>(0x00694920, 0x006C6200, 0x006D9590, 0x00737CD0), &Hook_oCNpc_TurnToEnemy, IvkEnabled(ENGINE));
	void __fastcall Hook_oCNpc_TurnToEnemy(oCNpc* _this, void* vtable)
	{
		if (_this == player && player->anictrl)
		{
			if (!IsLogicalPressed(GAME_ACTION) && COA3(player->anictrl->IsParadeRunning(), protoAni, aniName).HasWord("JUMPB"))
				return;
		}

		Ivk_oCNpc_TurnToEnemy(_this);
	}

	int __fastcall Hook_CGameManager_HandleCancelKey(CGameManager*, void*, int);
	CInvoke<int(__thiscall*)(CGameManager*, int)> Ivk_CGameManager_HandleCancelKey(ZenDef<TInstance>(0x00428D50, 0x0042B9A0, 0x0042A3E0, 0x0042A700), &Hook_CGameManager_HandleCancelKey, IvkEnabled(ENGINE));
	int __fastcall Hook_CGameManager_HandleCancelKey(CGameManager* _this, void* vtable, int a0)
	{
		int result = Ivk_CGameManager_HandleCancelKey(_this, a0);

		if (result && player && zKeyPressed(KEY_LSHIFT))
		{
			std::vector<oCNpc*> npcs = { player, player->GetFocusNpc(), player->talkOther };

			for (oCNpc* npc : npcs)
			{
				if (!npc || !npc->GetEM(false))
					continue;

				for (zCEventMessage* message : npc->GetEM(false)->messageList)
					if (message && !message->IsDeleted())
					{
						oCMsgConversation* conversation = message->CastTo<oCMsgConversation>();

						if (conversation && conversation->subType == oCMsgConversation::EV_OUTPUT)
							conversation->Delete();
					}
			}
		}

		return result;
	}
}