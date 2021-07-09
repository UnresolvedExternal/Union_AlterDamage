namespace NAMESPACE
{
	CFieldInfo::CFieldInfo(std::shared_ptr<CFieldCoreInfo> fieldCoreInfo, int offset) :
		fieldCoreInfo(fieldCoreInfo),
		offset(offset)
	{
		ASSERT(fieldCoreInfo.get());
		ASSERT(offset >= 0);
	}

	std::shared_ptr<const CFieldCoreInfo> CFieldInfo::GetCoreInfo() const
	{
		return fieldCoreInfo;
	}

	CFieldValue CFieldInfo::GetValue(const byte * data) const
	{
		return CFieldValue(fieldCoreInfo, data + offset);
	}

	void CFieldInfo::SetValue(byte * data, const CFieldValue & value) const
	{
		value.CopyValuesTo(data + offset);
	}

	void CFieldInfo::Initialize(byte * data) const
	{
		if (fieldCoreInfo->GetType() != CFieldCoreInfo::Type::String)
			return;

		data += offset;
		for (int i = 0; i < fieldCoreInfo->GetCount(); i++, data += sizeof(zSTRING))
			::new (data) zSTRING();
	}

	void CFieldInfo::Deinitialize(byte * data) const
	{
		if (fieldCoreInfo->GetType() != CFieldCoreInfo::Type::String)
			return;

		data += offset;
		for (int i = 0; i < fieldCoreInfo->GetCount(); i++, data += sizeof(zSTRING))
			reinterpret_cast<zSTRING*>(data)->~zSTRING();
	}
}