namespace NAMESPACE
{
	MEMPOOL_DEF(CFieldCoreInfo);

	CFieldCoreInfo::CFieldCoreInfo(const CSymbol& classVar)
	{
		ASSERT(classVar.GetType() == CSymbol::Type::ClassVar);

		parser = classVar.GetParser();
		ASSERT(parser);

		name = classVar.GetSymbol()->name.ToChar();
		name = name.GetWord(".", 2);
		ASSERT(name.Length());

		count = (int)classVar.GetSymbol()->ele;
		ASSERT(count > 0);

		switch (classVar.GetSymbol()->type)
		{
		case zPAR_TYPE_INT:
			type = name.EndWith("_P") ? Type::Func : Type::Int;
			break;

		case zPAR_TYPE_FLOAT:
			type = Type::Float;
			break;

		case zPAR_TYPE_FUNC:
			type = Type::Func;
			break;

		case zPAR_TYPE_STRING:
			type = Type::String;
			break;

		default:
			ASSERT(false);
		}
	}

	CFieldCoreInfo::CFieldCoreInfo(zCParser* parser, zCArchiver& arc) :
		parser(parser)
	{
		ASSERT(parser);

		name = arc.ReadString("FieldName").ToChar();
		type = (Type)arc.ReadInt("FieldType");
		count = arc.ReadInt("FieldCount");
	}

	CFieldCoreInfo::TCompareResult CFieldCoreInfo::Compare(const CFieldCoreInfo& right) const
	{
		if (!name.Compare(right.name))
			return TCompareResult::Distinct;

		return (parser == right.parser && type == right.type && count == right.count) ? TCompareResult::Identical : TCompareResult::Incompatible;
	}

	zCParser* CFieldCoreInfo::GetParser() const
	{
		return parser;
	}

	const string& CFieldCoreInfo::GetName() const
	{
		return name;
	}

	CFieldCoreInfo::Type CFieldCoreInfo::GetType() const
	{
		return type;
	}

	int CFieldCoreInfo::GetCount() const
	{
		return count;
	}

	void CFieldCoreInfo::Archive(zCArchiver & arc) const
	{
		arc.WriteString("FieldName", name);
		arc.WriteInt("FieldType", (int)type);
		arc.WriteInt("FieldCount", count);
	}
}