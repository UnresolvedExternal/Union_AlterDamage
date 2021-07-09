namespace NAMESPACE
{
	std::unordered_map<string, std::unique_ptr<CSGGlobal>, CStringHasher, CStringEqualityComparer> CSGGlobal::globals;

	CInvoke<zSTRING&(__stdcall*)(zSTRING&, int)> Ivk_oCSavegameManager_GetSlotDirName(ZenDef<TInstance>(0x00436260, 0x0043A650, 0x00439180, 0x004397B0), nullptr, IVK_DISABLED);

	zSTRING CSGGlobal::GetPath(int slotId) const
	{
		zSTRING path;
		Ivk_oCSavegameManager_GetSlotDirName(path, slotId);

		if (path.IsEmpty())
			return path;

		path = zoptions->GetDirString(DIR_ROOT) + zoptions->GetDirString(DIR_SAVEGAMES) + path;
		path += Z name + Z".SAV";
		return path;
	}

	CSGGlobal::CSGGlobal(const string& name) :
		name(name)
	{

	}

	void CSGGlobal::Save(const TGameEvent& event)
	{
		int slotId = -3;

		switch (event)
		{
		case TGameEvent::SaveBegin:
			slotId = -1;
			break;

		case TGameEvent::SaveEnd:
			slotId = SaveLoadGameInfo.changeLevel ? -1 : SaveLoadGameInfo.slotID;
			break;

		default:
			ASSERT(false);
		}

		zSTRING path = GetPath(slotId);
		if (path.IsEmpty())
			return;

		zCArchiver* arc = CreateArchiverWrite(path);
		Archive(*arc);
		arc->Close();
		arc->Release();
	}

	void CSGGlobal::Load(const TGameEvent& event)
	{
		int slotId = -3;

		switch (event)
		{
		case TGameEvent::LoadBegin:
		case TGameEvent::LoadBegin_NewGame:
		case TGameEvent::LoadBegin_SaveGame:
		case TGameEvent::LoadBegin_ChangeLevel:
		case TGameEvent::LoadEnd_NewGame:
			slotId = SaveLoadGameInfo.changeLevel ? -1 : SaveLoadGameInfo.slotID;
			break;

		case TGameEvent::LoadEnd:
		case TGameEvent::LoadEnd_SaveGame:
		case TGameEvent::LoadEnd_ChangeLevel:
			slotId = -1;
			break;

		default:
			ASSERT(false);
		}

		Clear();
		zSTRING path = GetPath(slotId);
		
		if (path.IsEmpty())
			return;

		zCArchiver* arc = zarcFactory->CreateArchiverRead(path, 0);
		
		if (!arc)
			return;

		Unarchive(*arc);
		arc->Close();
		arc->Release();
	}
}
