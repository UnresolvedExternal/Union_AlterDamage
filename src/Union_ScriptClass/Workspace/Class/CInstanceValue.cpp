namespace NAMESPACE
{
	CInstanceValue::CInstanceValue(std::shared_ptr<const CClassDescription> classDesc) :
		classDesc(classDesc)
	{
		if (!classDesc)
			return;

		values.reserve(classDesc->GetFields().size());

		for (const CFieldInfo& field : classDesc->GetFields())
			values.push_back(CFieldValue(field.GetCoreInfo()));
	}

	std::vector<CFieldValue>& CInstanceValue::GetValues()
	{
		return values;
	}

	const std::vector<CFieldValue>& CInstanceValue::GetValues() const
	{
		return values;
	}

	void CInstanceValue::Archive(zCArchiver & arc) const
	{
		arc.WriteInt("ValuesCount", values.size());

		for (const CFieldValue& value : values)
		{
			value.GetCoreInfo()->Archive(arc);
			value.Archive(arc);
		}
	}

	void CInstanceValue::Unarchive(zCArchiver & arc)
	{
		int count = arc.ReadInt("ValuesCount");

		for (int i = 0; i < count; i++)
		{
			auto fieldCoreInfo = std::make_shared<CFieldCoreInfo>(parser, arc);

			for (CFieldValue& value : values)
			{
				CFieldCoreInfo::TCompareResult compareResult = value.GetCoreInfo()->Compare(*fieldCoreInfo);

				switch (compareResult)
				{
				case CFieldCoreInfo::TCompareResult::Identical:
					value = CFieldValue(fieldCoreInfo, arc);
					goto loopEnd;

				case CFieldCoreInfo::TCompareResult::Distinct:
					break;

				case CFieldCoreInfo::TCompareResult::Incompatible:
				default:
					ASSERT(false);
					break;
				}
			}

		loopEnd:;
		}
	}
}