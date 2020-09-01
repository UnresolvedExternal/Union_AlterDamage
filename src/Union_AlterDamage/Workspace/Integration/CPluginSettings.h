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
		float attackRunChanceAdd;
		float attackRunMult;
		float randomStability;
		float mageMaxManaToDamage;
		float fireDotDamageMult;
		float fireDotDuration;
		string minDamageVarName;

		void Load();
	};
}