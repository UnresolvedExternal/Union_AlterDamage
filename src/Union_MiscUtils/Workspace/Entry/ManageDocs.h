namespace NAMESPACE
{
	class InstanceSet : public SaveData
	{
	private:
		std::unordered_set<int> instances;

	public:
		InstanceSet(const string& name) :
			SaveData{ name }
		{

		}

		virtual void Clear() override
		{
			instances.clear();
		}

		virtual void Archive(zCArchiver& arc) override
		{
			arc.WriteInt("SIZE", instances.size());

			for (int instance : instances)
				arc.WriteString("NAME", parser->GetSymbol(instance)->name);
		}

		virtual void Unarchive(zCArchiver& arc) override
		{
			int size = arc.ReadInt("SIZE");
			instances.reserve(size);
		
			for (int i = 0; i < size; i++)
			{
				zSTRING instanceName = arc.ReadString("NAME");
				int instance = parser->GetIndex(instanceName);

				if (instance >= 0)
					TryAdd(instance);
			}
		}

		bool Contains(int instance) const
		{
			return instances.find(instance) != instances.end();
		}

		bool TryAdd(int instance)
		{
			return instances.insert(instance).second;
		}

		bool TryRemove(int instance)
		{
			auto it = instances.find(instance);
			
			if (it != instances.end())
			{
				instances.erase(it);
				return true;
			}

			return false;
		}
	};

	InstanceSet& GetDocs()
	{
		static InstanceSet& docs = SaveData::Get<InstanceSet>("DocsRead");
		return docs;
	}

	Sub clearDocsRead(ZSUB(GameEvent::Execute), []()
		{
			Settings::TrackReadDocs.onChange += []()
			{
				if (!Settings::TrackReadDocs)
					GetDocs().Clear();
			};
		});

	Sub loadDocsRead(ZSUB(GameEvent::LoadEnd), Settings::TrackReadDocs, []()
		{
			GetDocs().Load(GameEvent::LoadEnd);
		});

	Sub saveDocsRead(ZSUB(GameEvent::SaveBegin), Settings::TrackReadDocs, []()
		{
			GetDocs().Save(GameEvent::SaveBegin);
		});

	oCItem* getStateEffectCalledOn = nullptr;
	int getStateEffectState = 0;

	int __fastcall Hook_oCItem_GetStateEffectFunc(oCItem*, void*, int);
	Hook<int(__thiscall*)(oCItem*, int), ActiveValue<bool>> Ivk_oCItem_GetStateEffectFunc(ZENFOR(0x00672420, 0x0069FDE0, 0x006B4D80, 0x00712B80), &Hook_oCItem_GetStateEffectFunc, HookMode::Patch, Settings::HookGetState);
	int __fastcall Hook_oCItem_GetStateEffectFunc(oCItem* _this, void* vtable, int a0)
	{
		getStateEffectCalledOn = _this;
		getStateEffectState = a0;
		return Ivk_oCItem_GetStateEffectFunc(_this, a0);
	}

	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc*, void*, oCMsgManipulate*);
	Hook<int(__thiscall*)(oCNpc*, oCMsgManipulate*), ActiveOption<bool>> Ivk_oCNpc_EV_UseItemToState(ZENFOR(0x006AFC70, 0x006E3AF0, 0x006F6AC0, 0x007558F0), &Hook_oCNpc_EV_UseItemToState, HookMode::Patch, Settings::TrackReadDocs);
	int __fastcall Hook_oCNpc_EV_UseItemToState(oCNpc* _this, void* vtable, oCMsgManipulate* message)
	{
		getStateEffectCalledOn = nullptr;
		int result = Ivk_oCNpc_EV_UseItemToState(_this, message);

		if (getStateEffectCalledOn && getStateEffectState == 0 && _this->IsAPlayer() && message && message->subType == oCMsgManipulate::EV_USEITEMTOSTATE && message->targetVob == getStateEffectCalledOn)
		{
			oCItem* item = getStateEffectCalledOn;

			if (item->HasFlag(ITM_CAT_DOCS) && item->objectName.Length())
				GetDocs().TryAdd(item->GetInstance());
		}

		return result;
	}
}
