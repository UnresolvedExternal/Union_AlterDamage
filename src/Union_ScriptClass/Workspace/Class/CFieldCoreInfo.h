namespace NAMESPACE
{
	class CFieldCoreInfo
	{
		MEMPOOL_DECL(CFieldCoreInfo);

	public:
		enum class Type
		{
			Int,
			Float,
			Func,
			String
		};

		enum class TCompareResult
		{
			Identical,
			Distinct,
			Incompatible
		};

	private:
		zCParser* parser;
		string name;
		Type type;
		int count;

	public:
		CFieldCoreInfo(const CSymbol& classVar);
		CFieldCoreInfo(zCParser* parser, zCArchiver& arc);
		CFieldCoreInfo(const CFieldCoreInfo&) = default;
		CFieldCoreInfo(CFieldCoreInfo&&) noexcept = default;
		CFieldCoreInfo& operator=(const CFieldCoreInfo&) = default;
		CFieldCoreInfo& operator=(CFieldCoreInfo&&) = default;

		TCompareResult Compare(const CFieldCoreInfo& right) const;

		zCParser* GetParser() const;
		const string& GetName() const;
		Type GetType() const;
		int GetCount() const;

		void Archive(zCArchiver& arc) const;
	};
}