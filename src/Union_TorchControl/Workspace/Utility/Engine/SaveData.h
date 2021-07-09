#include <unordered_map>

namespace NAMESPACE
{
	namespace Helpers
	{
		CInvoke<zSTRING&(__stdcall*)(zSTRING&, int)> Ivk_oCSavegameManager_GetSlotDirName(ZENDEF(0x00436260, 0x0043A650, 0x00439180, 0x004397B0), nullptr, IVK_DISABLED);
	}

	class SaveData
	{
	private:
		static std::unordered_map<string, SaveData*> instances;
		static Sub<void> cleaner;

		string name;

		SaveData(const SaveData&) = delete;
		SaveData& operator=(const SaveData&) = delete;

		zSTRING GetPath(int slotId) const
		{
			zSTRING path;
			Helpers::Ivk_oCSavegameManager_GetSlotDirName(path, slotId);

			if (path.IsEmpty())
				return path;

			path = zoptions->GetDirString(DIR_ROOT) + zoptions->GetDirString(DIR_SAVEGAMES) + path;
			path += Z name + Z".SAV";
			return path;
		}

		static void OnExit()
		{
			for (auto& pair : instances)
				delete pair.second;

			instances.clear();
		}

	protected:
		SaveData(const string& name) :
			name{ name }
		{
			this->name.Upper();
		}

	public:
		virtual ~SaveData()
		{

		}

		virtual void Archive(zCArchiver& arc) = 0;
		virtual void Unarchive(zCArchiver& arc) = 0;
		virtual void Clear() = 0;

		void Save(const GameEvent& event)
		{
			int slot = -1;

			if ((event & GameEvent::SaveEnd) && !SaveLoadGameInfo.changeLevel)
				slot = SaveLoadGameInfo.slotID;

			std::unique_ptr<zFILE> file{ zfactory->CreateZFile(GetPath(slot)) };
			file->DirCreate();
			file->Create();

			ZOwner<zCArchiver> arc{ zarcFactory->CreateArchiverWrite(file.get(), zARC_MODE_ASCII, true, 0) };
			arc->Close();
			Archive(*arc);
		}

		void Load(const GameEvent& event)
		{
			int slot = -1;

			if ((event & GameEvent::LoadBegin) && !SaveLoadGameInfo.changeLevel)
				slot = SaveLoadGameInfo.slotID;

			Clear();
			
			if (ZOwner<zCArchiver> arc{ zarcFactory->CreateArchiverRead(GetPath(slot), 0) })
			{
				Unarchive(*arc);
				arc->Close();
			}
		}

		template <class T>
		static T& Get(const string& name)
		{
			static_assert(std::is_assignable_v<SaveData*&, T*>, "Type must be inherited from SaveData");

			if (instances.empty())
				cleaner = { GameEvent::Exit, &SaveData::OnExit };

			auto it = instances.find(name);

			if (it == instances.end())
			{
				auto pair = instances.insert({ name, new T(name) });
				return static_cast<T&>(*pair.first->second);
			}
			
			return static_cast<T&>(*it->second);
		}
	};

	std::unordered_map<string, SaveData*> SaveData::instances;
	Sub<void> SaveData::cleaner;
}
