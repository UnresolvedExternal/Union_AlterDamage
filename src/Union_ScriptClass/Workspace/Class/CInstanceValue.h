namespace NAMESPACE
{
	class CInstanceValue
	{
	private:
		std::shared_ptr<const CClassDescription> classDesc;
		std::vector<CFieldValue> values;

	public:
		CInstanceValue(std::shared_ptr<const CClassDescription> classDesc);
		CInstanceValue(const CInstanceValue&) = default;
		CInstanceValue(CInstanceValue&&) noexcept = default;
		CInstanceValue& operator=(const CInstanceValue&) = default;
		CInstanceValue& operator=(CInstanceValue&&) = default;

		std::vector<CFieldValue>& GetValues();
		const std::vector<CFieldValue>& GetValues() const;

		void Archive(zCArchiver& arc) const;
		void Unarchive(zCArchiver& arc);
	};
}