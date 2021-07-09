namespace NAMESPACE
{
	[[nodiscard]] zCArchiver* CreateArchiverWrite(const zSTRING& path)
	{
		static bool saveGameToAnisValid = false;
		static bool saveGameToAnsi;

		if (!saveGameToAnisValid)
		{
			saveGameToAnisValid = true;
			COption& unionOpt = Union.GetUnionOption();
			unionOpt.Read(saveGameToAnsi, "GAME", "SaveGameToANSI");
		}

		zTArchiveMode mode = saveGameToAnsi ? zARC_MODE_ASCII : zARC_MODE_BINARY_SAFE;

		zFILE* file = zfactory->CreateZFile(path);
		file->DirCreate();
		file->Create();
		delete file;

		return zarcFactory->CreateArchiverWrite(path, mode, 0, 0);
	}

	void Print(zCView* view, const zVEC3& worldPos, const zSTRING& text)
	{
		if (!view)
			return;
		if (!text.Length())
			return;

		zCCamera* cam = COA2(ogame, GetCamera());
		if (!cam)
			return;

		zVEC3 pos = cam->GetTransform(zCAM_TRAFO_VIEW) * worldPos;
		if (pos[2] < cam->nearClipZ)
			return;

		int x, y;
		cam->Project(&pos, x, y);
		x = view->anx(x);
		y = view->any(y);

		float width = view->FontSize(const_cast<zSTRING&>(text));
		float height = view->FontY();
		float vx = x - width / 2.0f;
		float vy = y - height / 2.0f;
		vx = CoerceInRange<float>(vx, width, 0, 8191);
		vy = CoerceInRange<float>(vy, height, 0, 8191);

		view->Print(vx + 0.5f, vy + 0.5f, text);
	}

	bool IsLogicalPressed(unsigned short logicalId)
	{
		zCArray<unsigned short> keys;
		if (!zinput->GetBinding(logicalId, keys))
			return false;

		for (const auto& key : keys)
			if (zinput->KeyPressed(key))
				return true;

		return false;
	}

	zVEC3 WorldToView(const zVEC3& worldPos, zCView* view)
	{
		zCCamera* cam = ogame->GetCamera();
		zVEC3 result = cam->trafoView * worldPos;
		
		int x, y;
		cam->Project(&result, x, y);
		
		if (view)
		{
			x = view->anx(x);
			y = view->any(y);
		}

		result[0] = (float)x;
		result[1] = (float)y;
		return result;
	}

	zVEC3 WorldToViewText(const zVEC3& worldPos, zCView* view, const zSTRING& text, bool coerce)
	{
		zVEC3 viewPos = WorldToView(worldPos, view);
		float width = view->FontSize(const_cast<zSTRING&>(text));
		float height = view->FontY();
		viewPos[0] -= width / 2;
		viewPos[1] -= height / 2;

		if (coerce)
		{
			viewPos[0] = CoerceInRange(viewPos[0], width, 0.0f, 8191.0f);
			viewPos[1] = CoerceInRange(viewPos[1], height, 0.0f, 8191.0f);
		}

		return viewPos;
	}
}