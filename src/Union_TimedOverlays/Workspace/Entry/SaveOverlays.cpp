namespace NAMESPACE
{
	CSubscription saveOverlays(TGameEvent::SaveBegin, []()
		{
			CSGOverlaySet& overlays = CSGGlobal::Get<CSGOverlaySet>(A"OVERLAYS");
			overlays.Save();
		}, CHECK_THIS_ENGINE);

	CSubscription loadOverlays(TGameEvent::LoadEnd, []()
		{
			CSGOverlaySet& overlays = CSGGlobal::Get<CSGOverlaySet>(A"OVERLAYS");
			overlays.Load();
			overlays.Apply();
		}, CHECK_THIS_ENGINE);

	CSubscription updateOverlays(TGameEvent::Loop, []()
		{
			CSGOverlaySet& overlays = CSGGlobal::Get<CSGOverlaySet>(A"OVERLAYS");
			overlays.Update();
		}, CHECK_THIS_ENGINE);
}