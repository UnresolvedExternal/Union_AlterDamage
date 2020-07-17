namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		bool allowChangeSpeed;
		bool allowSkipNoise;
		bool restrictAcrobatic;
		Array<zSTRING> reapplyOverlays;

		CPluginSettings& operator=(const CPluginSettings& other) = delete;
		void Load();
	};

	extern CPluginSettings settings;
}