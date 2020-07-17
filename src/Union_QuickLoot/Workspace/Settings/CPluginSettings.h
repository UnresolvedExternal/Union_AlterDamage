namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		static CFlagsSerializer<int> catsSerializer;

		bool LootItems;
		bool LootNpcs;
		bool LootContainers;
		bool DropArmor;
		int NameToDescFlags;
		bool AppendAmountInfo;
		string XChar;

		CPluginSettings();
		CPluginSettings& operator=(const CPluginSettings& other) = delete;
		void Load();
	};

	extern CPluginSettings settings;
}