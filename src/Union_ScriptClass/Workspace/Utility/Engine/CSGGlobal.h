namespace NAMESPACE
{
	class CSGGlobal
	{
	private:
		static std::unordered_map<string, std::unique_ptr<CSGGlobal>, CStringHasher, CStringEqualityComparer> globals;

		const string name;

		CSGGlobal(const CSGGlobal& right) = delete;
		CSGGlobal& operator=(const CSGGlobal& right) = delete;
		zSTRING GetPath(int slotId) const;

	protected:
		CSGGlobal(const string& name);
		virtual void Archive(zCArchiver& arc) = 0;
		virtual void Unarchive(zCArchiver& arc) = 0;

	public:
		virtual void Clear() = 0;
		void Save(const TGameEvent& event);
		void Load(const TGameEvent& event);
		inline virtual ~CSGGlobal() { };

		template <class T>
		static T& Get(const string& name)
		{
			auto it = globals.find(name);
			T* pInstance = nullptr;

			if (it == globals.end())
			{
				pInstance = new T(name);
				globals[name] = std::unique_ptr<CSGGlobal>{ pInstance };
			}
			else
				pInstance = dynamic_cast<T*>(it->second.get());

			return *pInstance;
		}
	};
}