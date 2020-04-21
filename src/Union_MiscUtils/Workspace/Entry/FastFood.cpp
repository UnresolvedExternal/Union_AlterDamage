namespace NAMESPACE
{
	class CFastFoodController
	{
	private:
		static std::unordered_set<oCNpc*> npcs;

		oCNpc* const npc;
		const int instance;
		const int deltaHp;
		const int deltaMana;
		const float oldMotionFactor;
		zCEventMessage* activeMessage;
		
		CSubscription onLoop;
		CSubscription onLoadBegin;
		CSubscription onExit;

		bool NeedEat() const
		{
			return
				(!deltaHp || npc->attribute[NPC_ATR_HITPOINTS] + deltaHp <= npc->attribute[NPC_ATR_HITPOINTSMAX]) &&
				(!deltaMana || npc->attribute[NPC_ATR_MANA] + deltaMana <= npc->attribute[NPC_ATR_MANAMAX]);
		}

		bool TryCreateMessage()
		{
			oCItem* item = npc->IsInInv(instance, 1);
			if (!item)
				return false;

			activeMessage = new oCMsgManipulate(oCMsgManipulate::EV_USEITEMTOSTATE, item, 0);
			activeMessage->AddRef();
			npc->GetEM(false)->OnMessage(activeMessage, npc);

			return true;
		}

		void Delete()
		{
			delete this;
		}

		void OnLoop()
		{
			if (npc->sleepingMode == zTVobSleepingMode::zVOB_SLEEPING || IsLogicalPressed(GAME_PARADE) || zinput->GetMouseButtonPressedRight())
			{
				delete this;
				return;
			}

			if (COA2(activeMessage, IsDeleted()))
			{
				activeMessage->Release();
				activeMessage = nullptr;
				return;
			}

			if (!npc->interactItem && !activeMessage && NeedEat())
			{
				if (!TryCreateMessage())
					delete this;
				return;
			}

			if (!activeMessage && !npc->interactItem)
			{
				delete this;
				return;
			}
		}

		CFastFoodController(oCNpc* npc, int instance, int deltaHp, int deltaMana) :
			npc(npc),
			instance(instance),
			deltaHp(deltaHp),
			deltaMana(deltaMana),
			oldMotionFactor(ztimer->factorMotion),
			activeMessage(nullptr)
		{
			npcs.insert(npc);
			if (npc->IsAPlayer() && 5.0f > oldMotionFactor)
				ztimer->SetMotionFactor(5.0f);

			if (NeedEat())
				TryCreateMessage();

			onLoop.Reset(TGameEvent::Loop, std::bind(&CFastFoodController::OnLoop, this));
			onLoadBegin.Reset(TGameEvent::LoadBegin, std::bind(&CFastFoodController::Delete, this));
			onExit.Reset(TGameEvent::Exit, std::bind(&CFastFoodController::Delete, this));
		}

	public:
		static void TryStart(oCNpc* npc, int instance, int deltaHp, int deltaMana)
		{
			if (npc->sleepingMode == zTVobSleepingMode::zVOB_SLEEPING || deltaHp < 0 || deltaMana < 0 || npcs.find(npc) != end(npcs))
				return;
			if (!deltaHp && !deltaMana)
				return;
			new CFastFoodController(npc, instance, deltaHp, deltaMana);
		}

		~CFastFoodController()
		{
			npcs.erase(npc);
			COA2(activeMessage, Release());
			if (npc->IsAPlayer())
				ztimer->SetMotionFactor(oldMotionFactor);
		}
	};

	std::unordered_set<oCNpc*> CFastFoodController::npcs;

	int __fastcall Hook_oCNpc_EV_UseItemToState_FastFood(oCNpc*, void*, oCMsgManipulate*);
	CInvoke<int(__thiscall*)(oCNpc*, oCMsgManipulate*)> Ivk_oCNpc_EV_UseItemToState_FastFood(ZenDef<TInstance>(0x006AFC70, 0x006E3AF0, 0x006F6AC0, 0x007558F0), &Hook_oCNpc_EV_UseItemToState_FastFood, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpc_EV_UseItemToState_FastFood(oCNpc* _this, void* vtable, oCMsgManipulate* message)
	{
		if (!_this->IsAPlayer() || COA3(message, targetVob, GetVobType()) != zVOB_TYPE_ITEM)
			return Ivk_oCNpc_EV_UseItemToState_FastFood(_this, message);

		int oldHp = _this->attribute[NPC_ATR_HITPOINTS];
		int oldMana = _this->attribute[NPC_ATR_MANA];

		int result = Ivk_oCNpc_EV_UseItemToState_FastFood(_this, message);

		int deltaHp = _this->attribute[NPC_ATR_HITPOINTS] - oldHp;
		int deltaMana = _this->attribute[NPC_ATR_MANA] - oldMana;

		if (IsLogicalPressed(GAME_SLOW))
			CFastFoodController::TryStart(_this, static_cast<oCItem*>(message->targetVob)->instanz, deltaHp, deltaMana);

		return result;
	}
}