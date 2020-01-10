namespace NAMESPACE
{
	std::unique_ptr<int[]> CHitGenContainer::buffer;

	CHitGenContainer::CHitGenContainer()
	{
		if (!buffer)
			buffer.reset(new int[1000]);
	}

	CBinSeqGen& CHitGenContainer::operator[](int index)
	{
		auto where = hitGens.find(index);
		if (where != hitGens.end())
			return where->second;
		return hitGens.insert(std::make_pair(index, CBinSeqGen(TGlobals::pluginSettings.randomStability))).first->second;
	}

	void CHitGenContainer::Clear()
	{
		hitGens.clear();
	}

	void CHitGenContainer::Archive(zCArchiver& arc)
	{
		int index = 0;
		for (auto& pair : hitGens)
		{
			buffer[index++] = pair.first;
			float deviance = pair.second.GetDeviance();
			buffer[index++] = reinterpret_cast<int&>(deviance);
		}

		arc.WriteInt("HITGEN_COUNT", hitGens.size() + 2 * 1000 * 1000);
		arc.WriteRaw("HITGEN_DATA", &buffer[0], hitGens.size() * 2 * sizeof(int));
	}

	void CHitGenContainer::Unarchive(zCArchiver& arc)
	{
		Clear();

		int count = arc.ReadInt("HITGEN_COUNT");
		count -= 2 * 1000 * 1000;
		if (count < 0 || count > 10)
			return;

		arc.ReadRaw("HITGEN_DATA", &buffer[0], count * 2 * sizeof(int));

		for (int i = 0; i < count; i++)
		{
			int index = buffer[i * 2];
			float deviance = reinterpret_cast<float&>(buffer[i * 2 + 1]);
			hitGens.insert(std::make_pair(index, CBinSeqGen(TGlobals::pluginSettings.randomStability, deviance)));
		}
	}
}