namespace NAMESPACE
{
	class CFieldInfo
	{
	private:
		std::shared_ptr<const CFieldCoreInfo> fieldCoreInfo;
		int offset;

	public:
		CFieldInfo(std::shared_ptr<CFieldCoreInfo> fieldCoreInfo, int offset);
		CFieldInfo(const CFieldInfo&) = default;
		CFieldInfo(CFieldInfo&&) noexcept = default;
		CFieldInfo& operator=(const CFieldInfo&) = default;
		CFieldInfo& operator=(CFieldInfo&&) = default;

		std::shared_ptr<const CFieldCoreInfo> GetCoreInfo() const;

		CFieldValue GetValue(const byte* data) const;
		void SetValue(byte* data, const CFieldValue& value) const;
		void Initialize(byte* data) const;
		void Deinitialize(byte* data) const;
	};
}