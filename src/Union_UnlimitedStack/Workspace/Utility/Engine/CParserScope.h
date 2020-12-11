namespace NAMESPACE
{
	class CParserScope
	{
	private:
		zCParser* parser;
		std::vector<TScope<int>> symbols;
		TScope<zCPar_Symbol*> instance;
		TScope<void*> address;
		static int nesting;

	public:
		CParserScope(zCParser* parser) :
			parser(parser)
		{
			if (parser == ::NAMESPACE::parser)
			{
				std::vector<string> names = { "SELF", "OTHER", "VICTIM", "ITEM" };
				for (const string& name : names)
				{
					zCPar_Symbol* symbol = parser->GetSymbol(name);
					if (symbol)
						symbols.push_back(TScope<int>(symbol->offset));
				}
			}

			instance = TScope<zCPar_Symbol*>(zCPar_Symbol::instance_sym);
			address = TScope<void*>(zCPar_Symbol::instance_adr);
		}
	};
}