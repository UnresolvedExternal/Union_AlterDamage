namespace NAMESPACE
{
	class FastFoodController
	{
	private:
		static std::vector<oCNpc*> npcs;

		std::vector<Sub<void>> subs;

		oCNpc* const npc;
		const int instance;
		const int deltaHp;
		const int deltaMana;
		const float oldMotionFactor;
		zCEventMessage* activeMessage;

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

		void OnLoadBegin()
		{
			delete this;
		}

		void OnExit()
		{
			delete this;
		}

		void OnLoop()
		{
			if (npc->sleepingMode == zTVobSleepingMode::zVOB_SLEEPING)
			{
				delete this;
				return;
			}

			if (zinput->GetMouseButtonPressedRight() || zKeyPressed(KEY_ESCAPE))
			{
				delete this;
				return;
			}

			if (COA(activeMessage, IsDeleted()))
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

		FastFoodController(oCNpc* npc, int instance, int deltaHp, int deltaMana) :
			npc{ npc },
			instance{ instance },
			deltaHp{ deltaHp },
			deltaMana{ deltaMana },
			oldMotionFactor{ ztimer->factorMotion },
			activeMessage{}
		{
			npcs += npc;

			if (npc->IsAPlayer() && 5.0f > oldMotionFactor)
				ztimer->SetMotionFactor(5.0f);

			if (NeedEat())
				TryCreateMessage();

			ADDSUB(Loop);
			ADDSUB(LoadBegin);
			ADDSUB(Exit);
		}

	public:
		static void TryStart(oCNpc* npc, int instance, int deltaHp, int deltaMana)
		{
			if (npcs & npc)
				return;

			if (npc->sleepingMode == zTVobSleepingMode::zVOB_SLEEPING || deltaHp < 0 || deltaMana < 0)
				return;

			if (!deltaHp && !deltaMana)
				return;

			new FastFoodController(npc, instance, deltaHp, deltaMana);
		}

		~FastFoodController()
		{
			COA(activeMessage, Release());

			if (npc->IsAPlayer())
				ztimer->SetMotionFactor(oldMotionFactor);

			npcs -= npc;
		}
	};

	std::vector<oCNpc*> FastFoodController::npcs;

	int __fastcall Hook_oCNpc_EV_UseItemToState_FastFood(oCNpc*, void*, oCMsgManipulate*);
	Hook<int(__thiscall*)(oCNpc*, oCMsgManipulate*), ActiveOption<bool>> Ivk_oCNpc_EV_UseItemToState_FastFood(ZENFOR(0x006AFC70, 0x006E3AF0, 0x006F6AC0, 0x007558F0), &Hook_oCNpc_EV_UseItemToState_FastFood, HookMode::Patch, Settings::FastFood);
	int __fastcall Hook_oCNpc_EV_UseItemToState_FastFood(oCNpc* _this, void* vtable, oCMsgManipulate* message)
	{
		if (!_this->IsAPlayer())
			return Ivk_oCNpc_EV_UseItemToState_FastFood(_this, message);

		int oldHp = _this->attribute[NPC_ATR_HITPOINTS];
		int oldMana = _this->attribute[NPC_ATR_MANA];

		getStateEffectCalledOn = nullptr;
		int result = Ivk_oCNpc_EV_UseItemToState_FastFood(_this, message);

		if (!getStateEffectCalledOn)
			return result;

		int deltaHp = _this->attribute[NPC_ATR_HITPOINTS] - oldHp;
		int deltaMana = _this->attribute[NPC_ATR_MANA] - oldMana;

		if (Settings::FastFoodKey->GetPressed())
			FastFoodController::TryStart(_this, getStateEffectCalledOn->instanz, deltaHp, deltaMana);

		return result;
	}
}
