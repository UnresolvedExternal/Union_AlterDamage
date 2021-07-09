namespace NAMESPACE
{
	class CInstance
	{
	private:
		std::shared_ptr<const CClassDescription> classDesc;
		std::unique_ptr<byte[]> data;

	public:
		CInstance(std::shared_ptr<const CClassDescription> classDesc);
		CInstance(const CInstance& right);
		CInstance(CInstance&&) noexcept = default;
		CInstance& operator=(const CInstance& right);
		CInstance& operator=(CInstance&&) = default;

		bool operator==(const CInstance& right) const;
		bool operator!=(const CInstance& right) const;

		int GetSymbolOffset() const;
		CInstanceValue GetValue() const;
		void SetValue(const CInstanceValue& value);

		~CInstance();
	};
}