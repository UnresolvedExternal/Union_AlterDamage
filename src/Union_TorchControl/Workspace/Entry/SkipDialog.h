namespace NAMESPACE
{
	int __fastcall Hook_CGameManager_HandleCancelKey(CGameManager*, void*, int);
	Hook<int(__thiscall*)(CGameManager*, int), ActiveOption<KeyCombo>> Ivk_CGameManager_HandleCancelKey(ZENFOR(0x00428D50, 0x0042B9A0, 0x0042A3E0, 0x0042A700), &Hook_CGameManager_HandleCancelKey, HookMode::Patch, Settings::DiaHyperskipKey);
	int __fastcall Hook_CGameManager_HandleCancelKey(CGameManager* _this, void* vtable, int a0)
	{
		int result = Ivk_CGameManager_HandleCancelKey(_this, a0);

		if (!result || !Settings::DiaHyperskipKey->GetPressed())
			return result;
		
		std::vector<oCNpc*> npcs{ player, player->talkOther, player->GetFocusNpc() };

		for (oCNpc* npc : npcs)
			if (npc)
				for (zCEventMessage* message : COA(npc, GetEM(), messageList))
					if (oCMsgConversation* conversation = message->CastTo<oCMsgConversation>())
						if (conversation->subType == oCMsgConversation::EV_OUTPUT || conversation->subType == oCMsgConversation::EV_OUTPUTSVM ||
							conversation->subType == oCMsgConversation::EV_OUTPUTSVM_OVERLAY)
						{
							conversation->Delete();
							result += 1;
						}

		return result;
	}
}
