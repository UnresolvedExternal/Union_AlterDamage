namespace NAMESPACE
{
	class ParserScope
	{
	private:
		zCParser* parser;
		std::vector<VarScope<int>> symbols;
		VarScope<zCPar_Symbol*> instance;
		VarScope<void*> address;

	public:
		ParserScope(zCParser* parser) :
			parser{ parser }
		{
			if (parser == ::NAMESPACE::parser)
			{
				std::vector<string> names = { "SELF", "OTHER", "VICTIM", "ITEM" };
				for (const string& name : names)
				{
					zCPar_Symbol* symbol = parser->GetSymbol(name);
					if (symbol)
						symbols.push_back(VarScope<int>(symbol->offset));
				}
			}

			instance = VarScope<zCPar_Symbol*>(zCPar_Symbol::instance_sym);
			address = VarScope<void*>(zCPar_Symbol::instance_adr);
		}
	};
}