#include "Workspace\Utility\CThreadPool.h"
#include <shared_mutex>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <queue>

namespace fs = std::experimental::filesystem;

namespace NAMESPACE
{
	class CBackupManager
	{
	private:
		CThreadPool copyQueue;
		std::vector<std::unique_ptr<std::shared_timed_mutex>> accessors;
		int nextSlot;
		int nextSlotId;
		int nextBackupId;
		std::queue<fs::path> backups;

		void IncrementSlots();
		bool HandleError(std::error_code& error);
		string GenerateSavegameName(int sourceSlot, oCSavegameInfo* info);
		string GetSavegameFolder(int slot);
		fs::path GetBackupFolder();
		bool ClearFolder(const fs::path& path, bool keepSaveInfo);
		bool CopyFolderContent(const fs::path& source, const fs::path& dest, bool copySaveInfo);
		void CopySaveToSlot(int sourceSlot);
		void CopySaveBackup(int sourceSlot);

	public:
		CBackupManager(oCSavegameManager* saveMan);
		void OnSaveWritten(int slot, oCSavegameInfo* info);
		std::shared_lock<std::shared_timed_mutex> LockReadSlot(int slot);
		std::unique_lock<std::shared_timed_mutex> LockWriteSlot(int slot);
		std::vector<std::unique_lock<std::shared_timed_mutex>> LockAll();
		bool CanSaveInSlot(int slot);
	};
}