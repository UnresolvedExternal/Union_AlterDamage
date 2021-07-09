namespace NAMESPACE
{
	bool CInstanceManager::IsPersistentConstInstance(const CSymbol& symbol)
	{
		if (symbol.GetType() != CSymbol::Type::Instance)
			return false;

		const zSTRING& name = symbol.GetSymbol()->name;
		return name.Length() > 2 && name[(unsigned)name.Length() - 2] == '_' && name.Last() == 'P';
	}

	void CInstanceManager::Initialize()
	{
		for (int i = 0; i < parser->symtab.GetNumInList(); i++)
		{
			CSymbol symbol(parser, i);
			ASSERT(symbol.GetSymbol());

			switch (symbol.GetType())
			{
			case CSymbol::Type::Class:
				classes[i] = std::make_unique<CClassDescription>(symbol);
				break;

			case CSymbol::Type::Instance:
			case CSymbol::Type::VarInstance:
			{
				if (!symbol.IsGlobal())
					break;

				const int baseClass = parser->GetBaseClass(symbol.GetSymbol());

				if (baseClass < 0)
					break;

				if (!classes[baseClass]->IsPersistent())
					break;

				if (symbol.GetType() == CSymbol::Type::Instance)
					if (IsPersistentConstInstance(symbol))
						persistentVariables.push_back(symbol);
					else
						automaticConstInstances.push_back(symbol);

				if (symbol.GetType() == CSymbol::Type::VarInstance)
					persistentVariables.push_back(symbol);
				break;
			}
			}
		}
	}

	void CInstanceManager::HandleReference(const CSymbol& reference, bool persistent)
	{
		const int offset = reference.GetSymbol()->offset;
		if (!offset)
			return;

		auto it = persistentInstances.find(offset);
		ASSERT(it != persistentInstances.end());

		if (persistent)
		{
			needToSaveCounter += it->second.persistentReferences.empty();
			it->second.persistentReferences.push_back(reference.GetIndex());
		}
		else
			it->second.automaticReferences.push_back(reference.GetIndex());
	}

	void CInstanceManager::OnLoadBegin()
	{
		if (!initialized)
		{
			Initialize();
			initialized = true;
		};

		if (SaveLoadGameInfo.changeLevel)
		{
			ClearUnused();
			return;
		}

		Clear();
		
		for (const CSymbol& symbol : automaticConstInstances)
			symbol.GetSymbol()->offset = 0;

		for (const CSymbol& symbol : persistentVariables)
			symbol.GetSymbol()->offset = 0;

		Load(TGameEvent::LoadBegin);

		for (const CSymbol& symbol : automaticConstInstances)
			CreateInstance(symbol, symbol, false);

		for (const CSymbol& symbol : persistentVariables)
			if (IsPersistentConstInstance(symbol) && !symbol.GetSymbol()->offset)
				CreateInstance(symbol, symbol, false);
	}

	void CInstanceManager::OnSaveBegin()
	{
		if (!SaveLoadGameInfo.changeLevel)
		{
			Save(TGameEvent::SaveBegin);
			return;
		}

		for (const auto& pair : persistentInstances)
		{
			for (int index : pair.second.automaticReferences)
				parser->GetSymbolInfo(index)->offset = pair.first;

			for (int index : pair.second.persistentReferences)
				parser->GetSymbolInfo(index)->offset = pair.first;
		}
	}

	void CInstanceManager::OnLoop()
	{
		temporaryInstances.clear();
		if (allocationCounter > 100)
			ClearUnused();
	}

	CInstanceManager::CInstanceManager() :
		CSGGlobal("INSTANCES")
	{
		ADDSUB(LoadBegin);
		ADDSUB(SaveBegin);
		ADDSUB(Loop);
	}

	void CInstanceManager::Clear()
	{
		needToSaveCounter = 0;
		allocationCounter = 0;
		temporaryInstances.clear();
		persistentInstances.clear();
	}

	void CInstanceManager::Archive(zCArchiver& arc)
	{
		ClearUnused();

		arc.WriteInt("InstanceCount", needToSaveCounter);

		for (const auto& pair : persistentInstances)
		{
			if (pair.second.persistentReferences.empty())
				continue;

			arc.WriteInt("ReferenceCount", pair.second.persistentReferences.size());

			for (int index : pair.second.persistentReferences)
				arc.WriteString("ReferenceName", parser->GetSymbolInfo(index)->name);

			pair.second.instance->GetValue().Archive(arc);
		}
	}

	void CInstanceManager::Unarchive(zCArchiver& arc)
	{
		Clear();

		int instanceCount = arc.ReadInt("InstanceCount");

		for (int i = 0; i < instanceCount; i++)
		{
			int referenceCount = arc.ReadInt("ReferenceCount");
			TInstanceInfo info;

			int baseClass = -2;

			for (int k = 0; k < referenceCount; k++)
			{
				CSymbol reference(parser, arc.ReadString("ReferenceName"));
				if (reference.GetType() == CSymbol::Type::Instance || reference.GetType() == CSymbol::Type::VarInstance)
				{
					const int currentBaseClass = parser->GetBaseClass(reference.GetSymbol());
					ASSERT(baseClass == -2 || baseClass == currentBaseClass || currentBaseClass == -1);

					if (currentBaseClass >= 0)
					{
						baseClass = currentBaseClass;
						info.persistentReferences.push_back(reference.GetIndex());
					}
				}
			}

			if (info.persistentReferences.empty())
			{
				CInstanceValue(nullptr).Unarchive(arc);
				continue;
			}

			auto classDesc = classes[baseClass];
			ASSERT(classDesc->IsPersistent());

			CInstanceValue value(classDesc);
			value.Unarchive(arc);

			info.instance = std::make_unique<CInstance>(classDesc);
			info.instance->SetValue(value);

			for (int i : info.persistentReferences)
				parser->GetSymbolInfo(i)->offset = info.instance->GetSymbolOffset();

			persistentInstances.insert(std::make_pair(info.instance->GetSymbolOffset(), std::move(info)));
		}
	}

	void CInstanceManager::CreateInstance(const CSymbol& var, const CSymbol& ctor, bool incrementCounter)
	{
		ASSERT(var.GetType() == CSymbol::Type::Instance || var.GetType() == CSymbol::Type::VarInstance);
		ASSERT(ctor.GetType() == CSymbol::Type::Unknown || ctor.GetType() == CSymbol::Type::Instance ||
			ctor.GetType() == CSymbol::Type::Prototype);

		const int baseClass = parser->GetBaseClass(var.GetSymbol());

		ASSERT(baseClass >= 0);
		ASSERT(!ctor.GetSymbol() || baseClass == parser->GetBaseClass(ctor.GetSymbol()));

		auto classDesc = classes[baseClass];
		ASSERT((bool)classDesc);
		ASSERT(classDesc->IsPersistent() || !var.IsGlobal());

		auto instance = std::make_unique<CInstance>(classDesc);
		const int offset = instance->GetSymbolOffset();

		if (ctor.GetSymbol())
		{
			CParserScope scope(parser);
			auto varScope = AssignTemp(ctor.GetSymbol()->offset, offset);
			zCPar_Symbol::SetUseInstance(ctor.GetSymbol());
			parser->DoStack(ctor.GetValue<int>(0));
		}

		var.GetSymbol()->offset = instance->GetSymbolOffset();

		if (classDesc->IsPersistent())
		{
			TInstanceInfo info;
			info.instance = std::move(instance);
			persistentInstances.insert(std::make_pair(offset, std::move(info)));
			allocationCounter += incrementCounter;
		}
		else
			temporaryInstances.push_back(std::move(instance));
	}

	void CInstanceManager::ClearUnused()
	{
		needToSaveCounter = 0;
		allocationCounter = 0;

		for (auto& pair : persistentInstances)
		{
			pair.second.persistentReferences.clear();
			pair.second.automaticReferences.clear();
		}

		for (const CSymbol& symbol : automaticConstInstances)
			HandleReference(symbol, false);

		for (const CSymbol& symbol : persistentVariables)
			HandleReference(symbol, true);

		decltype(persistentInstances) survived;

		for (auto& pair : persistentInstances)
			if (pair.second.persistentReferences.size() || pair.second.automaticReferences.size())
				survived.insert(std::move(pair));

		persistentInstances = std::move(survived);
	}
}
