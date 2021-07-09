namespace NAMESPACE
{
	template <class T>
	T PopArgument(zCParser* parser)
	{
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>)
		{
			T arg;
			parser->GetParameter(arg);
			return arg;
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			int arg;
			parser->GetParameter(arg);
			return arg;
		}
		else if constexpr (std::is_same_v<T, zSTRING> || std::is_same_v<T, string>)
		{
			zSTRING arg;
			parser->GetParameter(arg);
			return arg;
		}
		else if constexpr (std::is_same_v<T, zCPar_Symbol*>)
			return parser->GetSymbol(parser->datastack.Pop());
		else if constexpr (std::is_same_v<T, CSymbol>)
			return CSymbol(parser, parser->datastack.Pop());
		else if constexpr (std::is_convertible_v<T, zCObject*>)
			return dynamic_cast<T>(static_cast<zCObject*>(parser->GetInstance()));
		else if constexpr (std::is_pointer_v<T>)
			return (T)parser->GetInstance();
		else
			static_assert(false, "Wrong type");
	}

	template <class ...TArgs>
	void PopArguments(zCParser* parser, TArgs&... args)
	{
		((args = PopArgument<TArgs>(parser)), ...);
	}

#define ZPOP(...) PopArguments(zCParser::GetParser(), __VA_ARGS__)

	std::vector<std::unique_ptr<zSTRING>> parserStrings;
	CSubscription cleanParserStrings;

	void CleanParserStrings()
	{
		parserStrings.clear();
	}

	CSubscription cleanParserStrigsOnExit(ZSUB(Exit), &CleanParserStrings);

	template <class T> 
	void PushArgument(zCParser* parser, const T& arg, int argSymbol = -1)
	{
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>)
			parser->SetReturn((int)arg);
		else if constexpr (std::is_same_v<T, float>)
			parser->SetReturn(arg);
		else if constexpr (std::is_convertible_v<T, const char*>)
		{
			if (parserStrings.empty())
				cleanParserStrings.Reset(ZSUB(Loop), &CleanParserStrings);

			parserStrings.push_back(std::make_unique<zSTRING>(arg));
			parser->SetReturn(*parserStrings.back());
		}
		else if constexpr (std::is_same_v<T, CSymbol>)
			if (argSymbol == -1)
				parser->SetReturn(reinterpret_cast<void*>(arg.GetSymbol()->offset));
			else
			{
				parser->GetSymbol(argSymbol)->offset = arg.GetSymbol()->offset;
				parser->datastack.Push(argSymbol);
			}
		else if constexpr (std::is_pointer_v<T>)
			if (argSymbol == -1)
				parser->SetReturn((void*)arg);
			else
			{
				parser->GetSymbol(argSymbol)->offset = reinterpret_cast<int>(arg);
				parser->datastack.Push(argSymbol);
			}
		else
			static_assert(false, "Wrong type");
	}

	template <class... TArgs>
	void PushArguments(zCParser* parser, const TArgs&... args)
	{
		(PushArgument(parser, args), ...);
	}

#define ZPUSH(...) PushArguments(zCParser::GetParser(), __VA_ARGS__)

	template <class T, class TName, class ...TArgs>
	T CallParser(zCParser* parser, const TName& nameIndex, TArgs&&... args)
	{
		int index = -1;

		if constexpr (std::is_convertible_v<TName, const char*>)
			index = parser->GetIndex(static_cast<const char*>(nameIndex));
		else
			index = nameIndex;

		CSymbol func(parser, index);
		int argument = func.GetIndex() + 1;
		(PushArgument(parser, args, argument++), ...);

		if (func.GetType() == CSymbol::Type::ExternalFunc)
			reinterpret_cast<int(__cdecl*)()>(func.GetValue<int>(0))();
		else
			parser->DoStack(func.GetValue<int>(0));

		if constexpr (std::is_same_v<T, void>)
			return;
		else
			return PopArgument<T>(parser);
	}

	template <class T>
	int GetParserType()
	{
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>)
			return zPAR_TYPE_INT;
		else if constexpr (std::is_same_v<T, float>)
			return zPAR_TYPE_FLOAT;
		else if constexpr (std::is_same_v<T, zSTRING> || std::is_same_v<T, string> || std::is_same_v<T, const char*>)
			return zPAR_TYPE_STRING;
		else if constexpr (std::is_same_v<T, void>)
			return zPAR_TYPE_VOID;
		else if constexpr (std::is_same_v<T, CSymbol> || std::is_pointer_v<T>)
			return zPAR_TYPE_INSTANCE;
		else
			static_assert(false, "Wrong type");
	}

	template <class TReturn, class... TArgs>
	class CExternalRegistration
	{
	private:
		zCParser* parser;
		string name;
		int(__cdecl* func)();
		std::vector<CSubscription> subs;

		void OnDefineExternals()
		{
			parser->DefineExternal(name.GetVector(), func, GetParserType<TReturn>(), GetParserType<TArgs>()..., zPAR_TYPE_VOID);
		}

	public:
		CExternalRegistration(zCParser* parser, const string& name, int (__cdecl* func)()) :
			parser{ parser },
			name{ name },
			func{ func }
		{
			if (!CHECK_THIS_ENGINE)
				return;

			ADDSUB(DefineExternals);
		}
	};

#define ZEXTERNAL(name, ...) CExternalRegistration<__VA_ARGS__> name ## _reg(parser, #name, &name)
#define ZEXTERNAL_MENU(name, ...) CExternalRegistration<__VA_ARGS__> name ## _regMenu(parserMenu, #name, &name)
}