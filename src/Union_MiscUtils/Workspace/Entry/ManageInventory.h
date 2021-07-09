namespace NAMESPACE
{
	enum class ItemState
	{
		Old = 0,
		New,
		Add
	};

	class InventoryMonitor : public SaveData
	{
	private:
		std::unordered_map<int, int> savedInv;
		std::unordered_map<oCItem*, ItemState> states;

		void SaveInventory(oCNpcInventory& inv, std::unordered_map<int, int>& dest)
		{
			dest.clear();

			for (oCItem* item : inv)
				dest[item->instanz] += item->amount;

			if (oCItem* item = COA(inv.owner, interactItem))
				if (item->homeWorld)
					dest[item->instanz] += item->amount;
		}

	public:
		InventoryMonitor(const string& name) :
			SaveData{ name }
		{

		}

		virtual void Clear() override
		{
			savedInv.clear();
			states.clear();
		}

		virtual void Archive(zCArchiver& arc) override
		{
			arc.WriteInt("instanceCount", savedInv.size());

			for (const std::pair<int, int>& p : savedInv)
			{
				zSTRING name = parser->GetSymbol(p.first)->name;
				int amount = p.second;
				arc.WriteString("instance", name);
				arc.WriteString("amount", amount);
			}
		}

		virtual void Unarchive(zCArchiver& arc) override
		{
			int size = arc.ReadInt("instanceCount");

			for (int i = 0; i < size; i++)
			{
				zSTRING name = arc.ReadString("instance");
				int amount = arc.ReadInt("amount");
				int instance = parser->GetIndex(name);

				if (instance >= 0)
					savedInv[instance] += amount;
			}
		}

		void Update(oCNpcInventory& inv)
		{
			SaveInventory(inv, savedInv);
		}

		void BuildStates(oCNpcInventory& inv)
		{
			states.clear();

			std::unordered_map<int, int> diff;
			SaveInventory(inv, diff);

			for (std::pair<const int, int>& p : diff)
				p.second -= savedInv[p.first];

			for (oCItem* item : inv)
			{
				if (item->HasFlag(ITM_FLAG_ACTIVE))
				{
					states[item] = ItemState::Old;
					continue;
				}

				auto it = diff.find(item->instanz);

				if (it->second > 0)
					states[item] = (savedInv[item->instanz] <= 0) ? ItemState::New : ItemState::Add;
				else
					states[item] = ItemState::Old;

				it->second -= item->amount;
			}
		}

		ItemState GetState(oCItem* item)
		{
			return states[item];
		}
	};

	InventoryMonitor& GetInventory()
	{
		static InventoryMonitor& inventory = SaveData::Get<InventoryMonitor>("Inventory");
		return inventory;
	}

	Sub clearInventory(ZSUB(GameEvent::Execute), []()
		{
			Settings::TrackNewItems.onChange += []()
			{
				if (!Settings::TrackNewItems)
					GetInventory().Clear();
			};
		});

	Sub loadInventory(ZSUB(GameEvent::LoadEnd), Settings::TrackNewItems, []()
		{
			GetInventory().Load(GameEvent::LoadEnd);
		});

	Sub saveInventory(ZSUB(GameEvent::SaveBegin), Settings::TrackNewItems, []()
		{
			GetInventory().Save(GameEvent::SaveBegin);
		});

	void __fastcall Hook_oCNpcInventory_Close(oCNpcInventory*, void*);
	Hook<void(__thiscall*)(oCNpcInventory*), ActiveOption<bool>> Ivk_oCNpcInventory_Close(ZENFOR(0x0066C1E0, 0x00699960, 0x006AE810, 0x0070C2F0), &Hook_oCNpcInventory_Close, HookMode::Patch, Settings::TrackNewItems);
	void __fastcall Hook_oCNpcInventory_Close(oCNpcInventory* _this, void* vtable)
	{
		if (player && &player->inventory2 == _this && _this->IsOpen() && _this->GetEnableHandleEvent())
			GetInventory().Update(*_this);

		Ivk_oCNpcInventory_Close(_this);
	}
}
