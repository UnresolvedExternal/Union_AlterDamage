namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		const string pluginName = "Union_AlterDamage";
		bool enablePopupDamage;
		bool switchAttribute;
		float humanFistDamageMult;
		float meleeRawDamageMult;
		float meleeMissMult;
		float armorSoftCap;
		float beyondSoftCapArmorScaling;
		float twoHandedMult;
		float randomStability;
		float fireDotDamageMult;
		float fireDotDuration;
		string minDamageVarName;

		void Load();
	};
}