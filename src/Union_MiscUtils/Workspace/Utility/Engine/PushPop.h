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
		else if constexpr (std::is_same_v<T, Symbol>)
			return Symbol(parser, parser->datastack.Pop());
		else if constexpr (std::is_convertible_v<T, zCObject*>)
		{
			Symbol symbol(parser, parser->datastack.Pop());

			if (!symbol)
				return nullptr;

			zCObject* instance = reinterpret_cast<zCObject*>(symbol.GetSymbol()->offset);
			return dynamic_cast<T>(instance);
		}
		else if constexpr (std::is_pointer_v<T>)
		{
			Symbol symbol(parser, parser->datastack.Pop());

			if (!symbol)
				return nullptr;

			return reinterpret_cast<T>(symbol.GetSymbol()->offset);
		}
		else
			static_assert(false, "Wrong type");
	}

	template <class ...TArgs>
	void PopArguments(zCParser* parser, TArgs&... args)
	{
		((args = PopArgument<TArgs>(parser)), ...);
	}

#define ZARGS(...) PopArguments(zCParser::GetParser(), __VA_ARGS__)

	namespace Helpers
	{
		class StringManager
		{
		private:
			std::vector<zSTRING*> strings;
			Sub<void> cleaner;

			void OnExit()
			{
				for (zSTRING* string : strings)
					delete string;

				strings.clear();
				cleaner = {};
			}

			StringManager()
			{
				strings.reserve(16u);
				cleaner = { GameEvent::Loop | GameEvent::MenuLoop | GameEvent::Exit, std::bind(&StringManager::OnExit, this) };
			}

		public:
			static StringManager& GetInstance()
			{
				static StringManager instance;
				return instance;
			}

			zSTRING& AddString(const char* string)
			{
				strings += new zSTRING{ string };
				return *strings.back();
			}
		};
	}

	template <class T> 
	void PushArgument(zCParser* parser, const T& arg, int argSymbol = -1)
	{
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>)
			parser->SetReturn((int)arg);
		else if constexpr (std::is_same_v<T, float>)
			parser->SetReturn(arg);
		else if constexpr (std::is_convertible_v<T, const char*>)
		{
			zSTRING& string = Helpers::StringManager::GetInstance().AddString(static_cast<const char*>(arg));
			parser->SetReturn(string)
		}
		else if constexpr (std::is_same_v<T, Symbol>)
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

#define ZRETURN(...) PushArguments(zCParser::GetParser(), __VA_ARGS__)

	template <class T, class TName, class ...TArgs>
	T CallParser(zCParser* parser, const TName& nameIndex, TArgs&&... args)
	{
		int index = -1;

		if constexpr (std::is_convertible_v<TName, const char*>)
			index = parser->GetIndex(static_cast<const char*>(nameIndex));
		else
			index = nameIndex;

		Symbol func(parser, index);
		int argument = func.GetIndex() + 1;
		(PushArgument(parser, args, argument++), ...);

		if (func.GetType() == Symbol::Type::ExternalFunc)
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
		else if constexpr (std::is_same_v<T, Symbol> || std::is_pointer_v<T>)
			return zPAR_TYPE_INSTANCE;
		else
			static_assert(false, "Wrong type");
	}

	template <class TReturn, class... TArgs>
	class ExternalRegistration
	{
	private:
		zCParser* parser;
		string name;
		int(__cdecl* func)();
		std::vector<Sub<bool>> subs;

		void OnDefineExternals()
		{
			parser->DefineExternal(name.GetVector(), func, GetParserType<TReturn>(), GetParserType<TArgs>()..., zPAR_TYPE_VOID);
		}

	public:
		ExternalRegistration(zCParser* parser, const string& name, int (__cdecl* func)()) :
			parser{ parser },
			name{ name },
			func{ func }
		{
			if (!CHECK_THIS_ENGINE)
				return;

			ADDSUB(DefineExternals);
		}
	};

#define ZEXTERNAL(name, ...) ExternalRegistration<__VA_ARGS__> name ## _reg(parser, #name, &name)
#define ZEXTERNAL_MENU(name, ...) ExternalRegistration<__VA_ARGS__> name ## _regMenu(parserMenu, #name, &name)
}