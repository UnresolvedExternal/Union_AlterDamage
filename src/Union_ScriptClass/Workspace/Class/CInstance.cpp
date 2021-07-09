namespace NAMESPACE
{
	CInstance::CInstance(std::shared_ptr<const CClassDescription> classDesc) :
		classDesc(classDesc),
		data(std::make_unique<byte[]>(classDesc->GetSize()))
	{
		for (const CFieldInfo& field : classDesc->GetFields())
		{
			field.Initialize(data.get());
			field.SetValue(data.get(), CFieldValue(field.GetCoreInfo()));
		}
	}

	CInstance::CInstance(const CInstance& right) :
		classDesc(right.classDesc),
		data(std::make_unique<byte[]>(right.classDesc->GetSize()))
	{
		for (const CFieldInfo& field : classDesc->GetFields())
		{
			field.Initialize(data.get());
			field.SetValue(data.get(), field.GetValue(right.data.get()));
		}
	}

	CInstance& CInstance::operator=(const CInstance& right)
	{
		if (classDesc->GetSymbol().GetSymbol() == right.classDesc->GetSymbol().GetSymbol())
		{
			for (const CFieldInfo& field : classDesc->GetFields())
				field.SetValue(data.get(), field.GetValue(right.data.get()));

			return *this;
		}

		CInstance::~CInstance();

		classDesc = right.classDesc;
		data.reset(new byte[classDesc->GetSize()]);

		for (const CFieldInfo& field : classDesc->GetFields())
		{
			field.Initialize(data.get());
			field.SetValue(data.get(), field.GetValue(right.data.get()));
		}

		return *this;
	}

	bool CInstance::operator==(const CInstance & right) const
	{
		return classDesc == right.classDesc && data == right.data;
	}

	bool CInstance::operator!=(const CInstance & right) const
	{
		return !(*this == right);
	}

	int CInstance::GetSymbolOffset() const
	{
		return reinterpret_cast<int>(data.get() - classDesc->GetBaseOffset());
	}

	CInstanceValue CInstance::GetValue() const
	{
		CInstanceValue value(classDesc);

		for (size_t i = 0; i < classDesc->GetFields().size(); i++)
			value.GetValues()[i] = classDesc->GetFields()[i].GetValue(data.get());

		return value;
	}

	void CInstance::SetValue(const CInstanceValue & value)
	{
		for (size_t i = 0; i < classDesc->GetFields().size(); i++)
			classDesc->GetFields()[i].SetValue(data.get(), value.GetValues()[i]);
	}

	CInstance::~CInstance()
	{
		for (const CFieldInfo& field : classDesc->GetFields())
			field.Deinitialize(data.get());
	}
}