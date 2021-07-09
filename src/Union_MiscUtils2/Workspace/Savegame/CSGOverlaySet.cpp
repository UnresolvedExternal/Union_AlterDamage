namespace NAMESPACE
{
	CSGOverlaySet::CSGOverlaySet(const string& name) :
		CSGGlobal(name)
	{

	}

	void CSGOverlaySet::Clear()
	{
		overlays.Clear();
	}

	void CSGOverlaySet::Archive(zCArchiver& arc)
	{
		arc.WriteInt("SIZE", overlays.GetNum());

		for (const TOverlay& overlay : overlays)
		{
			arc.WriteString("NAME", overlay.name);
			arc.WriteFloat("TIME", overlay.time);
		}
	}

	void CSGOverlaySet::Unarchive(zCArchiver& arc)
	{
		int size = arc.ReadInt("SIZE");
		for (int i = 0; i < size; i++)
		{
			TOverlay overlay;
			overlay.name = arc.ReadString("NAME");
			overlay.time = arc.ReadFloat("TIME");
			overlays.InsertEnd(overlay);
		}
	}

	void CSGOverlaySet::Update()
	{
		Clear();
		if (!player)
			return;

		for (oCNpc::oCNpcTimedOverlay* overlay : player->timedOverlays)
			if (overlay && overlay->timer > 0)
			{
				TOverlay e;
				e.name = overlay->mdsOverlayName;
				e.time = overlay->timer;
				overlays.InsertEnd(e);
			}
	}

	void CSGOverlaySet::Apply()
	{
		for (const TOverlay& overlay : overlays)
			player->ApplyTimedOverlayMds(overlay.name, overlay.time);
	}
}