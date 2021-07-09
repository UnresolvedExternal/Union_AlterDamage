namespace NAMESPACE
{
	void CSGInventoryMonitor::SaveInventory(oCNpcInventory& inv, std::unordered_map<int, int>& dest)
	{
		dest.clear();
		for (oCItem& item : inv)
			dest[item.GetInstance()] += item.amount;
	}

	void CSGInventoryMonitor::Archive(zCArchiver& arc)
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

	void CSGInventoryMonitor::Unarchive(zCArchiver& arc)
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

	CSGInventoryMonitor::CSGInventoryMonitor(const string& name) :
		CSGGlobal(name)
	{
	}

	void CSGInventoryMonitor::Clear()
	{
		savedInv.clear();
		states.clear();
	}

	void CSGInventoryMonitor::Update(oCNpcInventory& inv)
	{
		SaveInventory(inv, savedInv);
	}

	void CSGInventoryMonitor::BuildStates(oCNpcInventory& inv)
	{
		states.clear();

		std::unordered_map<int, int> diff;
		SaveInventory(inv, diff);
		
		for (std::pair<const int, int>& p : diff)
			p.second -= savedInv[p.first];

		for (oCItem& item : inv)
		{
			if (item.HasFlag(ITM_FLAG_ACTIVE))
			{
				states[&item] = TItemState::Old;
				continue;
			}

			int instance = item.GetInstance();
			auto it = diff.find(instance);
			
			if (it->second > 0)
				states[&item] = (savedInv[instance] <= 0) ? TItemState::New : TItemState::Add;
			else
				states[&item] = TItemState::Old;

			it->second -= item.amount;
		}
	}

	TItemState CSGInventoryMonitor::GetState(oCItem* item)
	{
		return states[item];
	}
}