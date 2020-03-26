namespace NAMESPACE
{
	CSubscription loadSettings(TGameEvent::Init, []()
		{
			auto& serializer = CPluginSettings::catsSerializer;
			serializer.Add("NONE", 1 << 0);
			serializer.Add("NF", 1 << 1);
			serializer.Add("FF", 1 << 2);
			serializer.Add("MUN", 1 << 3);
			serializer.Add("ARMOR", 1 << 4);
			serializer.Add("FOOD", 1 << 5);
			serializer.Add("DOCS", 1 << 6);
			serializer.Add("POTION", 1 << 7);
			serializer.Add("LIGHT", 1 << 8);
			serializer.Add("RUNE", 1 << 9);
			serializer.Add("MAGIC", 1 << 31);

			settings.Load();
			settings.Save();
		}, SubEnabled(CurrentEngine));
}