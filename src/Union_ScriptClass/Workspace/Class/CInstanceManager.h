namespace NAMESPACE
{
	class CInstanceManager : public CSGGlobal
	{
	private:
		struct TInstanceInfo
		{
			std::unique_ptr<CInstance> instance;
			std::vector<int> persistentReferences;
			std::vector<int> automaticReferences;
		};

		std::vector<CSubscription> subs;
		std::unordered_map<int, std::shared_ptr<CClassDescription>> classes; // all classes
		std::vector<CSymbol> automaticConstInstances; // instance somename(?!_p)(C_SOMECLASS_P)
		std::vector<CSymbol> persistentVariables; // var C_SOMECLASS_P global; instance somename_p(C_SOMECLASS_P)
		std::vector<std::unique_ptr<CInstance>> temporaryInstances; // not C_SOMECLASS_P instances
		std::unordered_map<int, TInstanceInfo> persistentInstances; // offset -> instance
		int needToSaveCounter;
		int allocationCounter;
		bool initialized;

		static bool IsPersistentConstInstance(const CSymbol& symbol);
		void Initialize();
		void HandleReference(const CSymbol& reference, bool persistent);

		void OnLoadBegin();
		void OnSaveBegin();
		void OnLoop();

	public:
		CInstanceManager();
		virtual void Clear() override;
		virtual void Archive(zCArchiver& arc) override;
		virtual void Unarchive(zCArchiver& arc) override;
		void CreateInstance(const CSymbol& var, const CSymbol& ctor, bool incrementCounter = true);
		void ClearUnused();
	};
}