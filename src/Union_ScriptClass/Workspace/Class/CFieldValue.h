namespace NAMESPACE
{
	class CFieldValue
	{
	private:
		std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo;
		std::unique_ptr<int[]> intValues;
		std::unique_ptr<float[]> floatValues;
		std::unique_ptr<string[]> stringValues;

		void AllocateValues();
		void CopyValuesFrom(const CFieldValue& right);

	public:
		CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo);
		CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo, zCArchiver& arc);
		CFieldValue(std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo, const byte* data);
		CFieldValue(const CFieldValue& right);
		CFieldValue(CFieldValue&&) noexcept = default;
		CFieldValue& operator=(const CFieldValue& right);
		CFieldValue& operator=(CFieldValue&&) = default;

		std::shared_ptr<const CFieldCoreInfo> GetCoreInfo() const;

		void CopyValuesTo(byte* data) const;
		void Archive(zCArchiver& arc) const;
	};
}