namespace NAMESPACE
{
	void CPluginSettings::Load()
	{
		minSlot = zoptions->ReadInt(pluginName.GetVector(), "MinSlot", ZenDef(8, 8, 10, 10));
		maxSlot = zoptions->ReadInt(pluginName.GetVector(), "MaxSlot", ZenDef(15, 15, 20, 20));
		backupFolder = zoptions->ReadString(pluginName.GetVector(), "BackupFolder", "\\Backups\\").ToChar();
		backupLimit = zoptions->ReadInt(pluginName.GetVector(), "BackupLimit", 100);

		if (minSlot > maxSlot || minSlot < 0 || maxSlot < 0)
			minSlot = maxSlot = -1;
	}
}