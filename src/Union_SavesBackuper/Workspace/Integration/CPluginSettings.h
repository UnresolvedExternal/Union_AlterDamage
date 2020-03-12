namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		const string pluginName = "Union_SavesBackuper";
		int minSlot;
		int maxSlot;
		string backupFolder;
		int backupLimit;

		void Load();
	};
}