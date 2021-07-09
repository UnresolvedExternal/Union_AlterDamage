namespace NAMESPACE
{
	std::unique_ptr<CInstanceManager> instanceManager;

	CSubscription createInstanceManager(ZSUB(Init), []() { instanceManager = std::make_unique<CInstanceManager>(); });
	CSubscription deleteInstanceManager(ZSUB(Exit), []() { instanceManager.reset(); });

	int __cdecl Hlp_CreateInstance()
	{
		CSymbol var;
		int ctorIndex;
		ZPOP(var, ctorIndex);
		
		instanceManager->CreateInstance(var, (ctorIndex > 0) ? CSymbol(zCParser::GetParser(), ctorIndex) : CSymbol());
		return false;
	}

	int __cdecl Hlp_AssignInstance()
	{
		CSymbol left, right;
		ZPOP(left, right);

		ASSERT(left.GetType() == CSymbol::Type::Instance || left.GetType() == CSymbol::Type::VarInstance);
		ASSERT(right.GetType() == CSymbol::Type::Instance || right.GetType() == CSymbol::Type::VarInstance);

		const int leftBase = zCParser::GetParser()->GetBaseClass(left.GetSymbol());
		const int rightBase = zCParser::GetParser()->GetBaseClass(right.GetSymbol());

		ASSERT(leftBase == -1 || leftBase == rightBase || right.GetSymbol()->offset == 0);

		left.GetSymbol()->offset = right.GetSymbol()->offset;
		return false;
	}

	int __cdecl Hlp_FuncToInt()
	{
		return false;
	}

	ZEXTERNAL(Hlp_CreateInstance, void, CSymbol, int);
	ZEXTERNAL(Hlp_AssignInstance, void, CSymbol, CSymbol);
	ZEXTERNAL(Hlp_FuncToInt, int, unsigned);
}