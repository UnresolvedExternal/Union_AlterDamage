namespace NAMESPACE
{
	typedef std::pair<CSymbolHelper, string> TSymbolInfo;

	class CSymbolHelperCache : public CCache<TSymbolInfo>
	{
	protected:
		zCParser* parser;
		std::vector<TSymbolInfo> entities;
		
		virtual void Initialize() override
		{
			for (int i = 0; i < parser->symtab.GetNumInList(); i++)
			{
				CSymbolHelper symbol(parser, parser->symtab.tablesort[i]);
				string name = symbol.GetSymbol()->name;
				name.Lower();
				entities.push_back(std::move(std::make_pair(symbol, name)));
			}
		}

		virtual void Clear() override
		{
			entities.clear();
		}

		virtual std::vector<const TSymbolInfo*> Filter(const std::function<bool(const TSymbolInfo&)>& predicate) const override
		{
			std::vector<const TSymbolInfo*> filtered;

			for (const TSymbolInfo& entity : entities)
			{
				if (predicate(entity))
					filtered.push_back(&entity);
			}

			return std::move(filtered);
		}

	public:
		CSymbolHelperCache(zCParser* parser) :
			parser(parser)
		{

		}
	};

	class CScriptVarNameCache : public CCache<string>
	{
	private:
		CSymbolHelperCache& symbolCash;
		std::vector<string> entities;
		int c_info;

	protected:
		virtual void Initialize() override
		{
			c_info = parser->GetIndex("C_INFO");

			for (const std::pair<CSymbolHelper, string>* sym : symbolCash.Select(GetPredicate()))
			{
				const string& name = sym->second;
				if (name.Length())
					entities.push_back(name);
			}
		}

		virtual void Clear() override
		{
			entities.clear();
		}

		virtual std::vector<const string*> Filter(const std::function<bool(const string&)>& predicate) const override
		{
			std::vector<const string*> filtered;

			for (const string& entity : entities)
				if (predicate(entity))
					filtered.push_back(&entity);

			return std::move(filtered);
		}

	public:
		CScriptVarNameCache(CSymbolHelperCache& symbolCash) :
			symbolCash(symbolCash)
		{

		}

		std::function<bool(const std::pair<CSymbolHelper, string>&)> GetPredicate() const
		{
			return [=](const std::pair<CSymbolHelper, string>& symbolInfo)
			{
				const CSymbolHelper& symbol = symbolInfo.first;

				switch (symbol.GetType())
				{
				case CSymbolHelper::Type::VarInt:
				case CSymbolHelper::Type::VarFloat:
				case CSymbolHelper::Type::VarString:
					return symbol.IsGlobal();

				case CSymbolHelper::Type::Instance:
					if (parser->GetBaseClass(symbol.GetIndex()) == c_info)
						return symbol.IsGlobal();

				default:
					return false;
				}
			};
		}
	};

	class CWaypointNameCache : public CCache<string>
	{
	protected:
		std::vector<string> entities;

		virtual void Initialize() override
		{
			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length())
					entities.push_back((A wp->name).Lower());

			std::sort(begin(entities), end(entities));
			auto newEnd = std::unique(begin(entities), end(entities));
			entities.erase(newEnd, end(entities));
		}

		virtual void Clear() override
		{
			entities.clear();
		}

		virtual std::vector<const string*> Filter(const std::function<bool(const string&)>& predicate) const override
		{
			std::vector<const string*> filtered;

			for (const string& entity : entities)
				if (predicate(entity))
					filtered.push_back(&entity);

			return std::move(filtered);
		}

		void OnLoadBegin()
		{
			Invalidate();
		}

	public:
		CWaypointNameCache()
		{
			ADDSUB(LoadBegin);
		}
	};

	class CVobNameCache : public CCache<string>
	{
	protected:
		std::vector<string> entities;

		virtual void Initialize() override
		{
			CVobTraverser traverser;
			traverser.handle = [&](zCVob* vob)
			{
				if (!vob->objectName.Length())
					return;

				string name = vob->objectName;
				name.Lower();

				entities.push_back(name);
			};

			traverser.TraverseVobTree();
			traverser.TraverseVobList();

			std::sort(begin(entities), end(entities));
			auto newEnd = std::unique(begin(entities), end(entities));
			entities.erase(newEnd, end(entities));
		}

		virtual std::vector<const string*> Filter(const std::function<bool(const string&)>& predicate) const override
		{
			std::vector<const string*> filtered;

			for (const string& entity : entities)
				if (predicate(entity))
					filtered.push_back(&entity);

			return std::move(filtered);
		}

		void OnLoadBegin()
		{
			Invalidate();
		}

	public:
		CVobNameCache()
		{
			ADDSUB(LoadBegin);
		}
	};
}