#include <condition_variable>
#include <fstream>

namespace NAMESPACE
{
	void CBackupManager::IncrementSlots()
	{
		if (nextSlot >= 0)
		{
			nextSlot = nextSlot + 1 - TGlobals::pluginSettings.minSlot;
			nextSlot = nextSlot % (TGlobals::pluginSettings.maxSlot - TGlobals::pluginSettings.minSlot + 1);
			nextSlot += TGlobals::pluginSettings.minSlot;
			nextSlotId += 1;
		}
		nextBackupId += 1;
	}

	bool CBackupManager::HandleError(std::error_code& error)
	{
		if (error)
		{
			cmd << "Error: " << error.message().c_str() << endl;
			return true;
		}
		return false;
	}

	string CBackupManager::GenerateSavegameName(int sourceSlot, oCSavegameInfo* info)
	{
		if (sourceSlot == 0)
			return string::Combine("%i_%t", nextSlotId, "quicksave");
		return string::Combine("%i_%z", nextSlotId, info->m_Name);
	}

	string CBackupManager::GetSavegameFolder(int slot)
	{
		return zoptions->GetDirString(zTOptionPaths::DIR_ROOT) + zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES) +
			ogame->savegameManager->GetSlotDirName(slot);
	}

	fs::path CBackupManager::GetBackupFolder()
	{
		fs::path path = TGlobals::pluginSettings.backupFolder.GetVector();
		path += zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES).ToChar();

		if (path.is_relative())
		{
			fs::path root = zoptions->GetDirString(zTOptionPaths::DIR_ROOT).ToChar();
			return root += path;
		}

		return path;
	}

	bool CBackupManager::ClearFolder(const fs::path& path, bool keepSaveInfo)
	{
		std::error_code error;
		fs::create_directories(path, error);
		if (HandleError(error))
			return false;

		auto iterator = fs::directory_iterator(path, error);
		if (HandleError(error))
			return false;

		for (const auto& e : iterator)
		{
			if (keepSaveInfo && e.path().has_filename() && string(e.path().filename().string().c_str()).CompareI("SAVEINFO.SAV"))
				continue;
			fs::remove_all(e.path(), error);
			if (HandleError(error))
				return false;
		}
		return true;
	}

	bool CBackupManager::CopyFolderContent(const fs::path& source, const fs::path& dest, bool copySaveInfo)
	{
		std::error_code error;
		auto iterator = fs::directory_iterator(source, error);
		if (HandleError(error))
			return false;

		for (const auto& e : iterator)
		{
			if (!copySaveInfo && e.path().has_filename() && string(e.path().filename().string().c_str()).CompareI("SAVEINFO.SAV"))
				continue;
			fs::copy(e.path(), dest, error);
			if (HandleError(error))
				return false;
		}
	}

	void CBackupManager::CopySaveToSlot(int sourceSlot)
	{
		if (TGlobals::pluginSettings.minSlot < 0)
			return;

		int destSlot = nextSlot;
		string sourceFolder = GetSavegameFolder(sourceSlot);
		string destFolder = GetSavegameFolder(destSlot);

		std::function<void()> copy = [=]()
		{
			if (ClearFolder(destFolder.GetVector(), true))
				CopyFolderContent(sourceFolder.GetVector(), destFolder.GetVector(), false);
			accessors[sourceSlot]->unlock_shared();
			accessors[destSlot]->unlock();
		};
		copyQueue.Enqueue(copy);
	}

	void CBackupManager::CopySaveBackup(int sourceSlot)
	{
		fs::path source = GetSavegameFolder(sourceSlot).GetVector();
		fs::path path = GetBackupFolder();
		string name(nextBackupId);
		path /= name.GetVector();
		int limit = TGlobals::pluginSettings.backupLimit;

		std::function<void()> copy = [=]()
		{
			std::error_code error;
			if (limit > 0)
			{
				while (backups.size() >= (uint)limit)
				{
					fs::remove_all(backups.front(), error);
					if (HandleError(error))
						return;
					backups.pop();
				}
				backups.push(path);
			}
			
			if (fs::exists(path))
				return;
			fs::copy(source, path, error);
			HandleError(error);
		};
		copyQueue.Enqueue(copy);
	}

	CBackupManager::CBackupManager(oCSavegameManager* saveMan) :
		copyQueue(1),
		nextSlot(-1),
		nextSlotId(-1)
	{
		if (TGlobals::pluginSettings.minSlot >= 0)
		{
			if (TGlobals::pluginSettings.maxSlot >= saveMan->infoList.GetNum())
			{
				Message::Error(string::Combine("MaxSlot(%i) exceeds the limit of %i", TGlobals::pluginSettings.maxSlot, saveMan->infoList.GetNum()),
					TGlobals::pluginSettings.pluginName);
				TGlobals::pluginSettings.maxSlot = saveMan->infoList.GetNum();
				TGlobals::pluginSettings.minSlot = min(TGlobals::pluginSettings.minSlot, TGlobals::pluginSettings.maxSlot);
			}

			for (int i = TGlobals::pluginSettings.minSlot; i <= TGlobals::pluginSettings.maxSlot; i++)
			{
				string name = saveMan->GetSavegame(i)->m_Name;
				int number = (int)name.ToInt32();
				number = max(number, 0);
				if (number > nextSlotId)
				{
					nextSlotId = number;
					nextSlot = i;
				}
			}

			if (nextSlotId <= 0)
			{
				nextSlotId = 1;
				nextSlot = TGlobals::pluginSettings.minSlot;
			}
		}

		nextBackupId = 0;
		std::error_code error;
		fs::create_directories(GetBackupFolder(), error);
		if (HandleError(error))
			Message::Error(A"Unable to initalize backup folder:\n" + GetBackupFolder().string().c_str(), TGlobals::pluginSettings.pluginName);
		else
		{
			auto iterator = fs::directory_iterator(GetBackupFolder(), error);
			if (!HandleError(error))
			{
				std::vector<std::pair<fs::path, int>> paths;
				for (const auto& e : iterator)
				{
					string name = e.path().filename().string().c_str();
					int number = (int)name.ToInt32();
					nextBackupId = max(nextBackupId, number);
					paths.push_back(std::make_pair(e.path(), number));
				}
				std::sort(paths.begin(), paths.end(), [](const std::pair<fs::path, int> & x, const std::pair<fs::path, int> & y)
					{ return x.second < y.second; });
				for (const auto& e : paths)
					backups.push(e.first);
			}
		}

		if (nextSlotId == 1)
			nextBackupId += 1;
		else
			IncrementSlots();

		accessors.resize(saveMan->infoList.GetNum());
		for (int i = 0; i < saveMan->infoList.GetNum(); i++)
			accessors[i] = std::make_unique<std::shared_timed_mutex>();
	}

	void CBackupManager::OnSaveWritten(int slot, oCSavegameInfo* info)
	{
		if (nextSlot >= 0 && slot != nextSlot)
		{
			oCSavegameInfo* destInfo = ogame->savegameManager->GetSavegame(nextSlot);

			accessors[slot]->lock_shared();
			accessors[nextSlot]->lock();
			
			// update thumb
			destInfo->CleanResources();
			destInfo->SetSlot(slot);
			destInfo->UpdateThumbPic(nullptr);
			destInfo->SetSlot(nextSlot);

			// copy info
			string infoPath = GetSavegameFolder(slot) + "SAVEINFO.SAV";
			zCArchiver* arc = zarcFactory->CreateArchiverRead(infoPath.GetVector(), 0);
			arc->ReadObject(destInfo);
			arc->Release();
			
			destInfo->m_Name = GenerateSavegameName(slot, info);

			// save info
			infoPath = GetSavegameFolder(nextSlot) + "SAVEINFO.SAV";
			zFILE* file = zfactory->CreateZFile(infoPath.GetVector());
			file->DirCreate();
			file->Create();
			delete file;
			arc = zarcFactory->CreateArchiverWrite(infoPath.GetVector(), zTArchiveMode::zARC_MODE_ASCII, 0, 0);
			arc->WriteObject(destInfo);
			arc->Release();

			CopySaveToSlot(slot);
		}

		CopySaveBackup(slot);
		IncrementSlots();
	}

	std::shared_lock<std::shared_timed_mutex> CBackupManager::LockReadSlot(int slot)
	{
		return std::move(std::shared_lock<std::shared_timed_mutex> { *accessors[slot] });
	}

	std::unique_lock<std::shared_timed_mutex> CBackupManager::LockWriteSlot(int slot)
	{
		return std::move(std::unique_lock<std::shared_timed_mutex> { *accessors[slot] });
	}

	std::vector<std::unique_lock<std::shared_timed_mutex>> CBackupManager::LockAll()
	{
		std::vector<std::unique_lock<std::shared_timed_mutex>> locks;
		for (const auto& e : accessors)
			locks.push_back(std::move(std::unique_lock<std::shared_timed_mutex> { *e }));
		return std::move(locks);
	}
}