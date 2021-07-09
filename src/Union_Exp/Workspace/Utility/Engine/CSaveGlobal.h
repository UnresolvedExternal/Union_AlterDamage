#include <unordered_map>

namespace NAMESPACE
{
	CInvoke<zSTRING&(__stdcall*)(zSTRING&, int)> Ivk_oCSavegameManager_GetSlotDirName(ZENDEF(0x00436260, 0x0043A650, 0x00439180, 0x004397B0), nullptr, IVK_DISABLED);

	class CSaveGlobal
	{
	private:
		static std::unordered_map<string, std::unique_ptr<CSaveGlobal>> globals;

		string name;

		CSaveGlobal(const CSaveGlobal&) = delete;
		CSaveGlobal& operator=(const CSaveGlobal&) = delete;

		zSTRING GetPath(int slotId) const
		{
			zSTRING path;
			Ivk_oCSavegameManager_GetSlotDirName(path, slotId);

			if (path.IsEmpty())
				return path;

			path = zoptions->GetDirString(DIR_ROOT) + zoptions->GetDirString(DIR_SAVEGAMES) + path;
			path += Z name + Z".SAV";
			return path;
		}

	protected:
		CSaveGlobal(const string& name) :
			name{ name }
		{
			this->name.Upper();
		}

	public:
		virtual ~CSaveGlobal()
		{

		}

		virtual void Archive(zCArchiver& arc) = 0;
		virtual void Unarchive(zCArchiver& arc) = 0;
		virtual void Clear() = 0;

		void Save(const TGameEvent& event)
		{
			int slot = -1;

			if ((event & TGameEvent::SaveEnd) && !SaveLoadGameInfo.changeLevel)
				slot = SaveLoadGameInfo.slotID;

			std::unique_ptr<zFILE> file{ zfactory->CreateZFile(GetPath(slot)) };
			file->DirCreate();
			file->Create();

			ZOwner<zCArchiver> arc{ zarcFactory->CreateArchiverWrite(file.get(), zARC_MODE_ASCII, true, 0) };
			Archive(*arc);
		}

		void Load(const TGameEvent& event)
		{
			int slot = -1;

			if ((event & TGameEvent::LoadBegin) && !SaveLoadGameInfo.changeLevel)
				slot = SaveLoadGameInfo.slotID;

			Clear();
			
			if (ZOwner<zCArchiver> arc{ zarcFactory->CreateArchiverRead(GetPath(slot), 0) })
				Unarchive(*arc);
		}

		template <class T>
		static T& Get(const string& name)
		{
			static_assert(std::is_assignable_v<CSaveGlobal*, T*>, "Type must be inherited from CSaveGlobal");

			auto it = globals.find(name);

			if (it == globals.end())
				it = globals.insert({ name, std::make_unique(name) });
			
			return *static_cast<T*>(it->second);
		}
	};

	std::unordered_map<string, std::unique_ptr<CSaveGlobal>> CSaveGlobal::globals;
}
