namespace NAMESPACE
{
	template <class T> T PopArgument(zCParser* parser) { ASSERT(std::is_pointer_v<T>); return (T)parser->GetInstance(); }
	template <> int PopArgument(zCParser* parser) { int arg; parser->GetParameter(arg); return arg; }
	template <> float PopArgument(zCParser* parser) { float arg; parser->GetParameter(arg); return arg; }
	template <> zSTRING PopArgument(zCParser* parser) { zSTRING arg; parser->GetParameter(arg); return arg; }
	template <> string PopArgument(zCParser* parser) { zSTRING arg; parser->GetParameter(arg); return arg; }
	template <> zCPar_Symbol* PopArgument(zCParser* parser) { int index = parser->datastack.Pop(); return parser->GetSymbol(index); }
	template <> CSymbolHelper PopArgument(zCParser* parser) { int index = parser->datastack.Pop(); return CSymbolHelper(parser, index); }

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

#define ZPOP(...) PopArguments(parser, __VA_ARGS__);

	template <class T> int GetParserType() { ASSERT(std::is_pointer_v<T>); return zPAR_TYPE_INSTANCE; }
	template <> int GetParserType<int>() { return zPAR_TYPE_INT; }
	template <> int GetParserType<float>() { return zPAR_TYPE_FLOAT; }
	template <> int GetParserType<zSTRING>() { return zPAR_TYPE_STRING; }
	template <> int GetParserType<string>() { return zPAR_TYPE_STRING; }
	template <> int GetParserType<CSymbolHelper>() { return zPAR_TYPE_INSTANCE; }
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

#define ZEXTERNAL(name, ...) CExternalRegistration<__VA_ARGS__> name ## _reg(parser, #name, &name);
}