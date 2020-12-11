namespace NAMESPACE
{
	template <class T> T PopArgument(zCParser* parser) { static_assert(std::is_pointer_v<T>, "Wrong type"); return (T)parser->GetInstance(); }
	template <> int PopArgument(zCParser* parser) { int arg; parser->GetParameter(arg); return arg; }
	template <> bool PopArgument(zCParser* parser) { return PopArgument<int>(parser); }
	template <> float PopArgument(zCParser* parser) { float arg; parser->GetParameter(arg); return arg; }
	template <> zSTRING PopArgument(zCParser* parser) { zSTRING arg; parser->GetParameter(arg); return arg; }
	template <> string PopArgument(zCParser* parser) { zSTRING arg; parser->GetParameter(arg); return arg; }
	template <> zCPar_Symbol* PopArgument(zCParser* parser) { int index = parser->datastack.Pop(); return parser->GetSymbol(index); }
	template <> CSymbol PopArgument(zCParser* parser) { int index = parser->datastack.Pop(); return CSymbol(parser, index); }

	template <class... TArgs>
	void PopArguments(zCParser* parser, TArgs&... args)
	{

	}

	template <class THead, class... TTail>
	void PopArguments(zCParser* parser, THead& head, TTail&... tail)
	{
		PopArguments(parser, tail...);
		head = PopArgument<THead>(parser);
	}

#define ZPOP(...) PopArguments(zCParser::GetParser(), __VA_ARGS__)

	struct TTemporayParserString
	{
		zSTRING string;
		int liveCounter;

		TTemporayParserString(const char* value) :
			string(value),
			liveCounter(2)
		{
			
		}
	};

	std::vector<std::unique_ptr<TTemporayParserString>> parserStrings;

	CSubscription cleanParserStrings(ZSUB(Loop), []()
		{
			size_t deleted = 0;

			for (size_t i = 0; i < parserStrings.size(); i++)
			{
				auto& ptr = parserStrings[i - deleted];
				if (deleted)
					ptr = std::move(parserStrings[i]);

				ptr->liveCounter -= 1;
				
				if (ptr->liveCounter <= 0)
					deleted += 1;
			}

			parserStrings.erase(parserStrings.begin() + (parserStrings.size() - deleted), parserStrings.end());
		});

	template <class T> void PushArgument(zCParser* parser, const T& arg) { static_assert(std::is_pointer_v<T>, "Wrong type"); parser->SetReturn((void*)arg); }
	template <> void PushArgument(zCParser* parser, const int& arg) { parser->SetReturn(arg); }
	template <> void PushArgument(zCParser* parser, const bool& arg) { parser->SetReturn((int)arg); }
	template <> void PushArgument(zCParser* parser, const float& arg) { parser->SetReturn(arg); }
	
	template <> void PushArgument(zCParser* parser, const char* const& arg)
	{
		parserStrings.push_back(std::make_unique<TTemporayParserString>(arg));
		parser->SetReturn(parserStrings.back()->string);
	}

	template <> void PushArgument(zCParser* parser, char* const& arg) { PushArgument<const char*>(parser, arg); }
	template <> void PushArgument(zCParser* parser, const string& arg) { PushArgument<const char*>(parser, arg.GetVector()); }
	template <> void PushArgument(zCParser* parser, const zSTRING& arg) { PushArgument<const char*>(parser, arg.ToChar()); }
	
	template <> void PushArgument(zCParser* parser, const CSymbol& arg) 
	{ 
		ASSERT(parser == arg.GetParser()); 
		ASSERT(arg.GetType() == CSymbol::Type::Instance || arg.GetType() == CSymbol::Type::VarInstance);
		parser->datastack.Push(arg.GetIndex());
	}

	template <class... TArgs>
	void PushArguments(zCParser* parser, const TArgs&... args)
	{

	}

	template <class THead, class... TTail>
	void PushArguments(zCParser* parser, const THead& head, const TTail&... tail)
	{
		PushArgument<THead>(parser, head);
		PushArguments(parser, tail...);
	}

#define ZPUSH(...) PushArguments(zCParser::GetParser(), __VA_ARGS__)

	template <class T> int GetParserType() { static_assert(std::is_pointer_v<T>, "Wrong type"); return zPAR_TYPE_INSTANCE; }
	template <> int GetParserType<int>() { return zPAR_TYPE_INT; }
	template <> int GetParserType<bool>() { return zPAR_TYPE_INT; }
	template <> int GetParserType<float>() { return zPAR_TYPE_FLOAT; }
	template <> int GetParserType<zSTRING>() { return zPAR_TYPE_STRING; }
	template <> int GetParserType<string>() { return zPAR_TYPE_STRING; }
	template <> int GetParserType<CSymbol>() { return zPAR_TYPE_INSTANCE; }
	template <> int GetParserType<void>() { return zPAR_TYPE_VOID; }
	template <> int GetParserType<unsigned>() { return zPAR_TYPE_FUNC; }

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
			parser(parser),
			name(name),
			func(func)
		{
			if (!CHECK_THIS_ENGINE)
				return;

			ASSERT(parser == ::NAMESPACE::parser || parser == parserCamera || parser == parserMenu || parser == parserMusic || 
				parser == parserParticleFX || parser == parserSoundFX || parser == parserVisualFX);

			ASSERT(name.Length());
			ASSERT(func);
			ADDSUB(DefineExternals);
		}
	};

#define ZEXTERNAL(name, ...) CExternalRegistration<__VA_ARGS__> name ## _reg(parser, #name, &name)
}