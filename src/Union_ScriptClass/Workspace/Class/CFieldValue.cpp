namespace NAMESPACE
{
	void CFieldValue::AllocateValues()
	{
		switch (fieldCoreInfo->GetType())
		{
		case CFieldCoreInfo::Type::Int:
		case CFieldCoreInfo::Type::Func:
			intValues = std::make_unique<int[]>(fieldCoreInfo->GetCount());
			ZeroMemory(intValues.get(), sizeof(int) * fieldCoreInfo->GetCount());
			break;

		case CFieldCoreInfo::Type::Float:
			floatValues = std::make_unique<float[]>(fieldCoreInfo->GetCount());
			ZeroMemory(floatValues.get(), sizeof(float) * fieldCoreInfo->GetCount());
			break;

		case CFieldCoreInfo::Type::String:
			stringValues = std::make_unique<string[]>(fieldCoreInfo->GetCount());
			break;

		default:
			ASSERT(false);
		}
	}

	void CFieldValue::CopyValuesFrom(const CFieldValue& right)
	{
		if (intValues)
			CopyMemory(intValues.get(), right.intValues.get(), sizeof(int) * fieldCoreInfo->GetCount());

		if (floatValues)
			CopyMemory(floatValues.get(), right.floatValues.get(), sizeof(float) * fieldCoreInfo->GetCount());

		if (stringValues)
			for (int i = 0; i < fieldCoreInfo->GetCount(); i++)
				stringValues[i] = right.stringValues[i];
	}

	CFieldValue::CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo) :
		fieldCoreInfo(fieldCoreInfo)
	{
		AllocateValues();
	}

	CFieldValue::CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo, zCArchiver & arc) :
		CFieldValue(fieldCoreInfo)
	{
		for (int i = 0; i < fieldCoreInfo->GetCount(); i++)
		{
			switch (fieldCoreInfo->GetType())
			{
			case CFieldCoreInfo::Type::Int:
				intValues[i] = arc.ReadInt(Z i);
				break;

			case CFieldCoreInfo::Type::Func:
			{
				const zSTRING funcName = arc.ReadString(Z i);

				if (funcName.IsEmpty())
				{
					intValues[i] = 0;
					break;
				}

				intValues[i] = fieldCoreInfo->GetParser()->GetIndex(funcName);

				switch (CSymbol(fieldCoreInfo->GetParser(), intValues[i]).GetType())
				{
				case CSymbol::Type::Prototype:
				case CSymbol::Type::Instance:
				case CSymbol::Type::VarInstance:
				case CSymbol::Type::Func:
				case CSymbol::Type::ExternalFunc:
					break;

				default:
					intValues[i] = 0;
					break;
				}

				break;
			}

			case CFieldCoreInfo::Type::Float:
				floatValues[i] = arc.ReadFloat(Z i);
				break;

			case CFieldCoreInfo::Type::String:
				stringValues[i] = arc.ReadString(Z i).ToChar();
				break;

			default:
				ASSERT(false);
			}
		}
	}

	CFieldValue::CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo, const byte * data) :
		CFieldValue(fieldCoreInfo)
	{
		if (intValues)
			CopyMemory(intValues.get(), data, sizeof(int) * fieldCoreInfo->GetCount());

		if (floatValues)
			CopyMemory(floatValues.get(), data, sizeof(float) * fieldCoreInfo->GetCount());

		if (stringValues)
			for (int i = 0; i < fieldCoreInfo->GetCount(); i++)
				stringValues[i] = (reinterpret_cast<const zSTRING*>(data) + i)->ToChar();
	}

	CFieldValue::CFieldValue(const CFieldValue & right) :
		CFieldValue(right.fieldCoreInfo)
	{
		CopyValuesFrom(right);
	}

	CFieldValue& CFieldValue::operator=(const CFieldValue & right)
	{
		intValues.reset();
		floatValues.reset();
		stringValues.reset();

		fieldCoreInfo = right.fieldCoreInfo;

		AllocateValues();
		CopyValuesFrom(right);
	}

	std::shared_ptr<const CFieldCoreInfo> CFieldValue::GetCoreInfo() const
	{
		return fieldCoreInfo;
	}

	void CFieldValue::CopyValuesTo(byte * data) const
	{
		if (intValues)
			CopyMemory(data, intValues.get(), sizeof(int) * fieldCoreInfo->GetCount());

		if (floatValues)
			CopyMemory(data, floatValues.get(), sizeof(float) * fieldCoreInfo->GetCount());

		if (stringValues)
			for (int i = 0; i < fieldCoreInfo->GetCount(); i++)
				* (reinterpret_cast<zSTRING*>(data) + i) = stringValues[i].GetVector();
	}

	void CFieldValue::Archive(zCArchiver & arc) const
	{
		for (int i = 0; i < fieldCoreInfo->GetCount(); i++)
		{
			switch (fieldCoreInfo->GetType())
			{
			case CFieldCoreInfo::Type::Int:
				arc.WriteInt(Z i, intValues[i]);
				break;

			case CFieldCoreInfo::Type::Func:
			{
				if (intValues[i] <= 0 || intValues[i] >= fieldCoreInfo->GetParser()->symtab.GetNumInList())
				{
					arc.WriteString(Z i, "");
					break;
				}

				const zSTRING& funcName = fieldCoreInfo->GetParser()->GetSymbolInfo(intValues[i])->name;
				arc.WriteString(Z i, funcName);
				break;
			}

			case CFieldCoreInfo::Type::Float:
				arc.WriteFloat(Z i, floatValues[i]);
				break;

			case CFieldCoreInfo::Type::String:
				arc.WriteString(Z i, stringValues[i]);
				break;

			default:
				ASSERT(false);
			}
		}
	}
}