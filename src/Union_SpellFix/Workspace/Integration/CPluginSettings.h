namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		const string pluginName = "Union_SpellFix";
		bool collideTargetFix;
		void Load();
	};

	extern CPluginSettings settings;
}