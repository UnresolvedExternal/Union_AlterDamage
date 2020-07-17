namespace NAMESPACE
{
	class CPluginSettings
	{
	private:
		bool initialized;

	public:
		int logicalKey;

		CPluginSettings();
		void LazyLoad();
	};

	CPluginSettings settings;
}