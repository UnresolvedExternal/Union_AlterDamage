namespace NAMESPACE
{
	class CClassDescription
	{
		MEMPOOL_DECL(CClassDescription);

	private:
		const CSymbol classSymbol;
		std::vector<CFieldInfo> fields;

	public:
		CClassDescription(const CSymbol& classSymbol);
		CClassDescription(const CClassDescription&) = default;
		CClassDescription(CClassDescription&&) noexcept = default;
		CClassDescription& operator=(const CClassDescription&) = default;
		CClassDescription& operator=(CClassDescription&&) = default;

		bool operator==(const CClassDescription& right) const;
		bool operator!=(const CClassDescription& right) const;

		const std::vector<CFieldInfo>& GetFields() const;
		int GetBaseOffset() const;
		int GetSize() const;
		const CSymbol& GetSymbol() const;
		bool IsPersistent() const;
	};
}