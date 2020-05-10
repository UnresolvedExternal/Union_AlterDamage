namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		CPluginSettings();
		CPluginSettings& operator=(const CPluginSettings& other) = delete;
		void Load();
	};

	extern CPluginSettings settings;
}