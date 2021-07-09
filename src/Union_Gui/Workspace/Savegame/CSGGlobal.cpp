namespace NAMESPACE
{
	std::unordered_map<string, std::unique_ptr<CSGGlobal>, CStringHasher, CStringEqualityComparer> CSGGlobal::globals;

	zSTRING CSGGlobal::GetPath() const
	{
		zSTRING path = zoptions->GetDirString(DIR_ROOT);
		path += zoptions->GetDirString(DIR_SAVEGAMES);
		path += "current\\";
		path += name.GetVector();
		path += ".SAV";
		return path;
	}

	CSGGlobal::CSGGlobal(const string& name) :
		name(name)
	{
	}

	void CSGGlobal::Save()
	{
		zCArchiver* arc = CreateArchiverWrite(GetPath());
		Archive(*arc);
		arc->Close();
		arc->Release();
	}

	void CSGGlobal::Load()
	{
		Clear();

		zCArchiver* arc = zarcFactory->CreateArchiverRead(GetPath(), 0);
		if (!arc)
			return;

		Unarchive(*arc);
		arc->Close();
		arc->Release();
	}
}