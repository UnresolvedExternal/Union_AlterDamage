namespace NAMESPACE
{
	const float PIF = (float)PI;

	string GetFocusText(oCItem* item)
	{
		zSTRING text;

		if (Settings::Cats.find(item->mainflag) != Settings::Cats.end())
			text += item->description;
		else
			text += item->name;

		if ((Settings::AppendAmountInfo || Settings::ForceAmountInfo) && item->amount != 1)
		{
			text += " (";
			text += Settings::AppendAmountInfo ? Settings::XChar.GetVector() : "x";
			text += item->amount;
			text += ")";
		}

		return text;
	}

	struct TLootInfo
	{
		enum class TState { Created, Showing, Closed };

		static constexpr float openingTime = 1.0f;
		static constexpr float openedTime = 5.0f;
		static constexpr float closingTime = 0.5f;
		static constexpr float fullTime = openingTime + openedTime + closingTime;

		TState state;
		float timestamp;
		zVEC3 worldPos;
		zSTRING text;
		float width;
		zCOLOR color;
		CCompositePointPath path;

		TLootInfo(oCItem* item, const zVEC3& worldPos, float timestamp) :
			state(TState::Created),
			timestamp(timestamp),
			worldPos(worldPos),
			text(GetFocusText(item)),
			width(screen->FontSize(text)),
			color(Settings::textColor)
		{

		}

		TLootInfo(const TLootInfo& right) = default;
		TLootInfo(TLootInfo&& right) = default;
		TLootInfo& operator=(const TLootInfo& right) = default;
		TLootInfo& operator=(TLootInfo&& right) = default;

		void Start(const zVEC2& targetPos, float timestamp)
		{
			if (state != TState::Created)
				throw std::exception("Attempting to Start TLootInfo when state is not TState::Create");

			state = TState::Showing;
			this->timestamp = timestamp;

			const zVEC3 sourcePos = WorldToViewText(worldPos, screen, text, false);
			const zVEC3 destPos(targetPos[0], targetPos[1], 0.0f);
			const zVEC3 closedPos(8192.0f, destPos[1], 0.0f);

			{
				auto startPath = std::make_unique<CLinearPointPath>();
				startPath->time = 0.0f;
				startPath->duration = fullTime;
				startPath->target = sourcePos;
				startPath->scale = [](float x) { return 1.0f; };
				path.paths.push_back(std::move(startPath));
			}

			{
				auto openingPath = std::make_unique<CLinearPointPath>();
				openingPath->time = 0.0f;
				openingPath->duration = openingTime;
				openingPath->target = destPos - sourcePos;

				const float x = -PIF / 2.0f;
				const float y = std::sinf(x);

				if (Settings::AnimatedText)
					openingPath->SetScale(&std::sinf, zVEC2(x, y), zVEC2(-x, -y));
				else
					openingPath->scale = [](float x) { return 1.0f; };

				path.paths.push_back(std::move(openingPath));
			}

			{
				auto closingPath = std::make_unique<CLinearPointPath>();
				closingPath->time = openingTime + openedTime;
				closingPath->duration = closingTime;
				closingPath->target = closedPos - destPos;

				if (Settings::AnimatedText)
					closingPath->scale = [](float x) { return x * x; };
				else
					closingPath->scale = [](float x) { return 1.0f; };

				path.paths.push_back(std::move(closingPath));
			}
		}

		zVEC2 GetPosition(float time)
		{
			time -= timestamp;
			zVEC3 position = path.GetPosition(time);
			if (time >= fullTime)
				state = TState::Closed;
			return zVEC2(position[0], position[1]);
		}

		void Resize(zVEC2 offset, float time)
		{
			time -= timestamp;
			const float duration = std::min(fullTime - closingTime - time, closingTime);
			if (duration < 1e-6)
				return;

			offset *= duration / closingTime;

			auto overlay = std::make_unique<CLinearPointPath>();
			overlay->time = time;
			overlay->duration = duration;
			overlay->target = zVEC3(offset[0], offset[1], 0.0f);

			if (Settings::AnimatedText)
				overlay->scale = &std::sqrtf;
			else
				overlay->scale = [](float x) { return 1.0f; };

			path.paths.push_back(std::move(overlay));

			static_cast<CLinearPointPath*>(path.paths[2].get())->target -= zVEC3(offset[0], offset[1], 0.0f);
		}
	};

	class CLootInformer
	{
	private:
		std::vector<CSubscription> subs;
		std::vector<std::vector<TLootInfo>> buckets;
		std::unique_ptr<zCView> view;
		size_t nextIndex;
		const zVEC4 prefferedViewPort;
		zVEC4 viewPort;
		const float delay = 0.01f;
		float startTime;
		float mainTimer;

		static constexpr int X1 = 0, Y1 = 1, X2 = 2, Y2 = 3;

		static bool IsLess(const std::vector<TLootInfo>& left, const std::vector<TLootInfo>& right)
		{
			if (left.size() != right.size())
				return left.size() < right.size();

			if (left.empty())
				return false;

			if (left[0].state != right[0].state)
				return left[0].state < right[0].state;

			return left[0].timestamp < right[0].timestamp;
		}

		void OnLoadBegin()
		{
			startTime = -delay;
			nextIndex = 0;

			for (std::vector<TLootInfo>& bucket : buckets)
				bucket.clear();
		}

		float CalcResizeByInventory()
		{
			const float invX = 8192.0f * 0.6f;
			float dx = 0;

			if (player->inventory2.IsOpen() && oCNpc::game_mode != NPC_GAME_PLUNDER)
			{
				if (IsMoreF(viewPort[X2], invX))
					dx = invX - viewPort[X2];
			}
			else
			{
				if (IsLessF(viewPort[X2], prefferedViewPort[X2]))
					dx = prefferedViewPort[X2] - viewPort[X2];
			}

			return dx;
		}

		void ResizeMessages(float dx)
		{
			for (std::vector<TLootInfo>& bucket : buckets)
				if (bucket.size() && bucket[0].state == TLootInfo::TState::Showing)
					bucket[0].Resize(zVEC2(dx, 0), mainTimer);
		}

		void AdjustTimer()
		{
			size_t maxSize = 0;
			for (const std::vector<TLootInfo>& bucket : buckets)
				maxSize = std::max(maxSize, bucket.size());

			const float mult = std::max(1.0f, (float)maxSize);
			mainTimer += ztimer->frameTimeFloat / ztimer->factorMotion / 1000.0f * mult;
		}

		void AdjustNextIndex()
		{
			while (nextIndex > 0 && buckets[nextIndex - 1].empty())
				nextIndex -= 1;
		}

		void AdjustSize()
		{
			float dx = CalcResizeByInventory();
			viewPort[X1] += dx;
			viewPort[X2] += dx;

			const float currentWidth = viewPort[X2] - viewPort[X1];
			float width = prefferedViewPort[X2] - prefferedViewPort[X1];

			for (const std::vector<TLootInfo>& bucket : buckets)
				if (bucket.size() && mainTimer < bucket[0].timestamp + TLootInfo::fullTime - TLootInfo::closingTime)
					width = std::max(width, bucket[0].width);

			const float dWidth = currentWidth - width;
			viewPort[X1] += dWidth;
			dx += dWidth;

			if (!IsEqualF(dx, 0.0f))
				ResizeMessages(dx);
		}

		void OpenMessages()
		{
			for (size_t k = 0; k < buckets.size() && startTime + delay <= mainTimer; k++)
			{
				const size_t index = (nextIndex + k) % buckets.size();
				if (buckets[index].empty() || buckets[index][0].state != TLootInfo::TState::Created)
					continue;

				buckets[index][0].Start(zVEC2(viewPort[0], viewPort[1] + screen->FontY() * index), mainTimer);
				nextIndex = (index + 1) % buckets.size();
				startTime = mainTimer;
			}
		}

		void DrawMessages()
		{
			screen->InsertItem(view.get());
			view->ClrPrintwin();

			for (std::vector<TLootInfo>& bucket : buckets)
				if (bucket.size() && bucket[0].state == TLootInfo::TState::Showing)
				{
					zVEC2 position = bucket[0].GetPosition(mainTimer);
					if (bucket[0].state == TLootInfo::TState::Showing)
						view->PrintTimed(position[0] + 0.5f, position[1] + 0.5f, bucket[0].text, -2.0f, &bucket[0].color);
					else
						bucket.erase(begin(bucket));
				}

			view->BlitText();
			screen->RemoveItem(view.get());
		}

		void OnLoop()
		{
			if (ogame->singleStep)
				return;

			AdjustTimer();
			AdjustNextIndex();
			AdjustSize();
			OpenMessages();
			DrawMessages();
		}

	public:
		CLootInformer(int x1, int y1, int x2, int y2) :
			view(new zCView(0, 0, 8192, 8192)),
			prefferedViewPort(x1, y1, x2, y2),
			viewPort(x1, y1, x2, y2),
			nextIndex(0),
			startTime(-99999.9f),
			mainTimer(0.0f)
		{
			int size = (int)((viewPort[3] - viewPort[1]) / screen->FontY());
			size = std::max(1, size);
			buckets.resize(size);
			ADDSUB(Loop);
			ADDSUB(LoadBegin);
		}

		void AddLoot(oCItem* item, const zVEC3& worldPos)
		{
			TLootInfo info(item, worldPos, mainTimer);
			if (info.text.IsEmpty())
				return;

			std::vector<TLootInfo>* bestBucket = &buckets[0];
			for (size_t i = 1; i < buckets.size(); i++)
				if (IsLess(buckets[i], *bestBucket))
					bestBucket = &buckets[i];

			bestBucket->push_back(std::move(info));
		}
	};

	CLootInformer* lootInformer;
	CSubscription deleteLootInformer(ZSUB(Exit), []() { if (lootInformer) delete lootInformer; });

	void CreateLootInformer()
	{
		lootInformer = new CLootInformer(8192 / 4 * 3, 8192 / 4, 8192, 8192 / 4 * 3);
	}

	void PrintItemName(void* pItem)
	{
		if (!pItem)
			return;

		if (!lootInformer)
			CreateLootInformer();

		oCItem* const item = static_cast<oCItem*>(pItem);
		lootInformer->AddLoot(item, item->GetPositionWorld());
	}

	Array<oCItem*> GetDrop(oCNpc* npc)
	{
		Array<oCItem*> drop;

		if (!npc)
			return drop;

		if (Settings::LootNpcSafe)
			if (!npc->state.curState.valid)
				return drop;
			else if (npc->state.curState.prgIndex != NPC_AISTATE_DEAD && npc->state.curState.prgIndex != NPC_AISTATE_UNCONSCIOUS)
				return drop;

		for (oCItem& item : npc->inventory2)
			if ((!item.HasFlag(ITM_FLAG_ACTIVE) || Union.GetEngineVersion() <= Engine_G1A) && (!item.HasFlag(ITM_CAT_ARMOR) || Settings::DropArmor))
				drop.InsertEnd(&item);

		for (oCItem*& item : drop)
			item = npc->RemoveFromInv(item, item->amount);

		return drop;
	}

	Array<oCItem*> GetDrop(oCMobContainer* container)
	{
		Array<oCItem*> drop;

		if (!container)
			return drop;

		for (oCItem* item : container->containList)
			drop.InsertEnd(item);

		if (!drop.GetNum())
			return drop;

		for (oCItem* item : drop)
			container->Remove(item);

		return drop;
	}

	bool NeedKey(const zSTRING& keyInstance)
	{
		for (zCVob* vob : COA3(ogame, GetGameWorld(), voblist))
		{
			oCMobLockable* mob = vob->CastTo<oCMobLockable>();
			if (mob && mob->locked && mob->keyInstance.CompareI(keyInstance))
				return true;
		}
		return false;
	}

	bool TryRemoveKey(oCNpc* npc, const zSTRING& instance)
	{
		if (!npc->IsAPlayer())
			return false;
		if (!instance.Length())
			return false;
		if (zinput->GetState(GAME_SLOW))
			return false;

		oCItem* key = npc->IsInInv(instance, 0);
		if (!key)
			return false;

		if (NeedKey(instance))
			return false;

		while (key)
		{
			key = npc->RemoveFromInv(key, key->amount);
			ogame->GetWorld()->RemoveVob(key);
			key = npc->IsInInv(instance, 0);
		}

		return true;
	}

	struct TKeyToggler
	{
	public:
		bool toggle;

		TKeyToggler() :
			toggle(true)
		{

		}

		~TKeyToggler()
		{
			if (toggle && COA3(player, GetEM(false), messageList).GetNum())
				Settings::Key->GetToggled();
		}
	};
	
	CSubscription quickLoot(ZSUB(Loop), []()
		{
			if (!Settings::LootItems && !Settings::LootContainers && !Settings::LootNpcs)
				return;

			if (!ogame || !player)
				return;

			if (!oCInformationManager::GetInformationManager().HasFinished())
				return;

			if (ogame->singleStep)
				return;

			if (!Settings::Key->GetPressed())
				return;
			
			if (!oCNpcFocus::focus || (void*)oCNpcFocus::focus != (void*)&oCNpcFocus::focuslist[FOCUS_NORMAL])
				return;

			if (!COA2(player, focus_vob) || player->GetWeaponMode() != NPC_WEAPON_NONE || player->IsUnconscious())
				return;

			if (player->interactMob)
				return;

			if (player->attribute[NPC_ATR_HITPOINTS] <= 0)
				return;

			if (!Settings::LootInDanger && ogame->GetHeroStatus() != oHERO_STATUS_STD)
				return;

			Array<oCItem*> drop;
			zVEC3 pos;

			oCItem* item = Settings::LootItems ? COA3(player, focus_vob, CastTo<oCItem>()) : nullptr;
			oCNpc* npc = Settings::LootNpcs ? COA3(player, focus_vob, CastTo<oCNpc>()) : nullptr;
			oCMobContainer* chest = Settings::LootContainers ? COA3(player, focus_vob, CastTo<oCMobContainer>()) : nullptr;

			if (!item && !npc && !chest)
				return;

			if (!oCNpcFocus::focus->IsInRange(player->focus_vob->type, player->GetDistanceToVob2(*player->focus_vob)))
				return;
			
			if (npc || chest)
				pos = player->focus_vob->GetPositionWorld();
			
			if (item)
			{
				zCModel* model = COA3(item, GetVisual(), CastTo<zCModel>());

				if (model)
				{
					pos = item->GetPositionWorld();
					pos[1] += model->bbox3DLocalFixed.maxs[1] * 1.6f;
					pos[1] = std::min(pos[1], item->bbox3D.maxs[1] + 100.0f);
				}
				else
				{
					pos = item->bbox3D.GetCenter();
					pos[1] += (item->bbox3D.maxs[1] - item->bbox3D.mins[1]) * 0.82f;
				}
			}
			
			if (item && !player->inventory2.IsOpen())
				drop.InsertEnd(item);
			
			if (npc)
			{
				drop = GetDrop(npc);

				if (drop.GetNum())
				{
					npc->AssessTheft_S(player);
					player->CloseDeadNpc();
				}
			}
			
			if (chest && chest->containList.next && !player->inventory2.IsOpen())
			{
				TKeyToggler keyToggler;

				if (chest->conditionFunc.Length())
				{
					parser->SetInstance("SELF", player);
					bool canUse = *(bool*)parser->CallFunc(chest->conditionFunc);

					if (!canUse)
						return;
				}

				if (chest->locked)
				{
					if (chest->keyInstance.IsEmpty())
						return;

					if (!player->IsInInv(chest->keyInstance, 1))
						return;

					chest->locked = false;
					
#if ENGINE == Engine_G1
					auto subtype = oCMsgConversation::EV_PLAYSOUND;
#else
					auto subtype = oCMsgConversation::EV_SNDPLAY;
#endif

					auto message = new oCMsgConversation(subtype, "PICKLOCK_SUCCESS");
					message->f_yes = true;
					message->target = chest;
					player->GetEM(false)->OnMessage(message, player);

					if (Settings::RemoveKeys && TryRemoveKey(player, chest->keyInstance) && ogame->GetTextView())
						ogame->GetTextView()->PrintTimedCXY(Z"Key removed: " + chest->keyInstance, 5000.0f, &Settings::textColor);
				}

				drop = GetDrop(chest);

				if (drop.GetNum())
				{
					player->AssessUseMob_S(chest);

					// suppress inv open
					chest->locked = true;

					for (int i = 0; i < chest->state_num; i++)
					{
						chest->OnBeginStateChange(player, i, i + 1);
						chest->OnEndStateChange(player, i, i + 1);
					}

					for (int i = 0; i < chest->state_num; i++)
					{
						chest->OnBeginStateChange(player, chest->state_num - i, chest->state_num - i - 1);
						chest->OnEndStateChange(player, chest->state_num - i, chest->state_num - i - 1);
					}

					chest->locked = false;
				}

				keyToggler.toggle = false;
			}

			if (drop.GetNum() && !lootInformer)
				CreateLootInformer();

			for (oCItem* item : drop)
			{
				if (item->objectName.CompareI("ITLSTORCHBURNING"))
				{
					if (player->HasTorch())
					{
						ZOwner<oCItem> torch = (ZOwner<oCItem>)static_cast<oCItem*>(ogame->GetGameWorld()->CreateVob_novt(zVOB_TYPE_ITEM, "ITLSTORCH"));
						if (torch)
							lootInformer->AddLoot(torch.get(), pos);
					}
				}
				else
					lootInformer->AddLoot(item, pos);

				if (item->homeWorld)
					player->DoTakeVob(item);
				else
					player->DoPutInInventory(item);
			}

			if (item)
#if ENGINE <= Engine_G1A
				player->CollectFocusVob();
#else
				player->CollectFocusVob(true);
#endif
		});

}
