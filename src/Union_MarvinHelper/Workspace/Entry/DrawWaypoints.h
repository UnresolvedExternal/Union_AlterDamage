namespace NAMESPACE
{
	void Draw(zCWaypoint* wp, zCCamera* cam)
	{
		const zVEC3& center = wp->GetPositionWorld();
		std::vector<zVEC3> points = { { -15, 0, 0 }, { 15, 0, 0 }, { 0, 0, -15 }, { 0, 0, 15 } };

		for (zVEC3& point : points)
			point += center;

		zlineCache->Line3D(points[0], points[1], GFX_RED, true);
		zlineCache->Line3D(points[2], points[3], GFX_RED, true);

		if (cam->connectedVob->GetPositionWorld().Distance(center) >= 1000)
			return;

		const zVEC3 viewCenter = WorldToView(center, screen);

		if (viewCenter[VZ] < cam->nearClipZ)
			return;

		screen->Print(static_cast<int>(viewCenter[VX] + 0.5f), static_cast<int>(viewCenter[VY] + 0.5f), wp->GetName());
	}

	void __fastcall Hook_zCWayNet_Draw(zCWayNet*, void*, zCCamera*);
	ModulePatchCallInvoker<void(__thiscall*)(zCWayNet*, zCCamera*)> Ivk_zCWayNet_Draw(ZENFOR(0x00703C60, 0x0073E460, 0x0074DD70, 0x007AD870), &Hook_zCWayNet_Draw);
	void __fastcall Hook_zCWayNet_Draw(zCWayNet* _this, void* vtable, zCCamera* a0)
	{
		Ivk_zCWayNet_Draw(_this, a0);

		std::unordered_set<zCWaypoint*> waypoints;

		for (zCWay* way : _this->waylist)
			if (way)
			{
				waypoints.insert(way->left);
				waypoints.insert(way->right);
			}

		for (zCWaypoint* wp : _this->wplist)
			if (wp && waypoints.find(wp) == waypoints.end())
				Draw(wp, a0);
	}

	void __fastcall Hook_oCGame_ShowFreePoints(oCGame*, void*);
	ModulePatchCallInvoker<void(__thiscall*)(oCGame*)> Ivk_oCGame_ShowFreePoints(ZENFOR(0x00640700, 0x006677B0, 0x0066E510, 0x006CB290), &Hook_oCGame_ShowFreePoints);
	void __fastcall Hook_oCGame_ShowFreePoints(oCGame* ogame, void* vtable)
	{
		if (!ogame->showFreePoints || !ogame->GetCameraVob())
			return;

		constexpr float radius = 3000.0f;

		zTBBox3D bbox;
		bbox.mins = bbox.maxs = ogame->GetCameraVob()->GetPositionWorld();
		bbox.mins -= zVEC3{ radius };
		bbox.maxs += zVEC3{ radius };

		zCArray<zCVob*> vobs;
		ogame->world->bspTree.bspRoot->CollectVobsInBBox3D(vobs, bbox);

		for (zCVob* vob : vobs)
			if (zCVobSpot* fp = dynamic_cast<zCVobSpot*>(vob))
			{
				constexpr float size = 50.0f;

				std::vector<zVEC3> vertexes;
				vertexes.reserve(8u);

				for (int z = 1; z >= -1; z -= 2)
					for (int x = -1, sign = 1; x <= 1; x += 2, sign *= -1)
						for (int y = -1 * sign; abs(y) <= 1; y += 2 * sign)
							vertexes.emplace_back(x * size, y * 2 * size, z * size);

				for (zVEC3& v : vertexes)
					v = fp->trafoObjToWorld * v;

				const zCOLOR color = fp->IsAvailable(ogame->GetSelfPlayerVob()) ? GFX_YELLOW : GFX_RED;

				for (int i = 0; i < 4; i++)
				{
					zlineCache->Line3D(vertexes[i], vertexes[(i + 1) % 4], GFX_GREEN, true);
					zlineCache->Line3D(vertexes[i + 4], vertexes[(i == 3) ? 4 : (i + 5)], color, true);
					zlineCache->Line3D(vertexes[i], vertexes[i + 4], color, true);
				}

				const zVEC3 screenPos = WorldToView(fp->GetPositionWorld(), screen);

				if (screenPos[VZ] > 0 && screenPos[VZ] < 1000)
					screen->Print(static_cast<int>(screenPos[VX] + 0.5f), static_cast<int>(screenPos[VY] + 0.5f), fp->objectName);
			}
	}
}