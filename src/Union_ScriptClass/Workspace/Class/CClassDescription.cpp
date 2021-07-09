namespace NAMESPACE
{
	MEMPOOL_DEF(CClassDescription);

	CClassDescription::CClassDescription(const CSymbol& classSymbol) :
		classSymbol(classSymbol)
	{
		ASSERT(classSymbol.GetType() == CSymbol::Type::Class);

		fields.reserve(classSymbol.GetSymbol()->ele);

		for (int i = 1; i <= (int)classSymbol.GetSymbol()->ele; i++)
		{
			CSymbol classVarSymbol(classSymbol.GetParser(), classSymbol.GetIndex() + i);
			fields.push_back(CFieldInfo(std::make_shared<CFieldCoreInfo>(classVarSymbol), classVarSymbol.GetSymbol()->offset - GetBaseOffset()));
		}
	}

	bool CClassDescription::operator==(const CClassDescription & right) const
	{
		return classSymbol.GetParser() == right.classSymbol.GetParser() && classSymbol.GetIndex() == right.classSymbol.GetIndex();
	}

	bool CClassDescription::operator!=(const CClassDescription & right) const
	{
		return !(*this == right);
	}

	const std::vector<CFieldInfo>& CClassDescription::GetFields() const
	{
		return fields;
	}

	int CClassDescription::GetSize() const
	{
		return classSymbol.GetSymbol()->offset;
	}

	int CClassDescription::GetBaseOffset() const
	{
		return classSymbol.GetSymbol()->single_intdata;
	}

	const CSymbol& CClassDescription::GetSymbol() const
	{
		return classSymbol;
	}

	bool CClassDescription::IsPersistent() const
	{
		const zSTRING& name = classSymbol.GetSymbol()->name;
		return name.Length() > 2 && name[(unsigned)name.Length() - 2] == '_' && name.Last() == 'P';
	}
}