namespace NAMESPACE
{
	void CPluginSettings::Load()
	{
		enablePopupDamage = zoptions->ReadBool(pluginName.GetVector(), Z"EnablePopupDamage", true);
		switchAttribute = zoptions->ReadBool(pluginName.GetVector(), Z"SwitchAttribute", true);
		humanFistDamageMult = zoptions->ReadReal(pluginName.GetVector(), Z"HumanFistDamageMult", 0.5f);
		meleeRawDamageMult = zoptions->ReadReal(pluginName.GetVector(), Z"MeleeRawDamageMult", ZenDef(2.0f, 2.0f, 1.0f, 1.0f));
		meleeMissMult = zoptions->ReadReal(pluginName.GetVector(), Z"MeleeMissMult", 0.5f);
		armorSoftCap = zoptions->ReadReal(pluginName.GetVector(), Z"ArmorSoftCap", 0.6f);
		beyondSoftCapArmorScaling = zoptions->ReadReal(pluginName.GetVector(), Z"BeyondSoftCapArmorScaling", 0.3f);
		twoHandedMult = zoptions->ReadReal(pluginName.GetVector(), Z"TwoHandedMult", 1.35f);
		randomStability = zoptions->ReadReal(pluginName.GetVector(), Z"RandomStability", 0.5f);
		fireDotDamageMult = zoptions->ReadReal(pluginName.GetVector(), Z"FireDotDamageMult", 0.5f);
		fireDotDuration = zoptions->ReadReal(pluginName.GetVector(), Z"FireDotDuration", 5.0f);
		minDamageVarName = (string)zoptions->ReadString(pluginName.GetVector(), Z"MinDamageVarName", "#");
	}
}