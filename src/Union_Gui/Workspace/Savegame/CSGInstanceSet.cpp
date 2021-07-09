namespace NAMESPACE
{
	void CSGInstanceSet::Archive(zCArchiver& arc)
	{
		arc.WriteInt("SIZE", Size());
		for (int e : *this)
			arc.WriteString("NAME", parser->GetSymbol(e)->name);
	}

	void CSGInstanceSet::Unarchive(zCArchiver & arc)
	{
		int size = arc.ReadInt("SIZE");
		for (int i = 0; i < size; i++)
		{
			zSTRING instanceName = arc.ReadString("NAME");
			int instance = parser->GetIndex(instanceName);
			if (instance >= 0)
				TryAdd(instance);
		}
	}

	CSGInstanceSet::CSGInstanceSet(const string& name) :
		CSGGlobal(name)
	{
	}

	void CSGInstanceSet::Clear()
	{
		instances.clear();
	}

	bool CSGInstanceSet::Contains(int instance) const
	{
		return instances.find(instance) != instances.end();
	}

	bool CSGInstanceSet::TryAdd(int instance)
	{
		if (Contains(instance))
			return false;
		instances.insert(instance);
		return true;
	}

	bool CSGInstanceSet::TryRemove(int instance)
	{
		auto it = instances.find(instance);
		bool contains = it != instances.end();
		if (contains)
			instances.erase(it);
		return contains;
	}

	int CSGInstanceSet::Size() const
	{
		return instances.size();
	}

	std::unordered_set<int>::const_iterator CSGInstanceSet::begin() const
	{
		return instances.cbegin();
	}

	std::unordered_set<int>::const_iterator CSGInstanceSet::end() const
	{
		return instances.cend();
	}
}