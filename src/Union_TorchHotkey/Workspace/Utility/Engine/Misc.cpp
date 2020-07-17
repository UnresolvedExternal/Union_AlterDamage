namespace NAMESPACE
{
	zCArchiver* CreateArchiverWrite(zSTRING path)
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
		x -= width / 2;
		y -= height / 2;
		x = CoerceInRange<float>(x, width, 0, 8191);
		y = CoerceInRange<float>(y, height, 0, 8191);

		view->Print(x + 0.5f, y + 0.5f, text);
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
}