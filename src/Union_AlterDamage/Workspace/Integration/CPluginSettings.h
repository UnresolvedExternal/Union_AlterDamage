namespace NAMESPACE
{
	class CPluginSettings
	{
	public:
		const string pluginName = "Union_AlterDamage";
		bool enablePopupDamage;
		bool requiredAttrScaling;
		float humanFistDamageMult;
		float meleeRawDamageMult;
		float meleeMissMult;
		float armorSoftCap;
		float beyondSoftCapArmorScaling;
		float twoHandedMult;
		float comboChanceAdd;
		float comboDamageAdd;
		float randomStability;
		float fireDotDamageMult;
		float fireDotDuration;
		string minDamageVarName;

		void Load();
	};
}