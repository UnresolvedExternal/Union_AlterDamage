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
}