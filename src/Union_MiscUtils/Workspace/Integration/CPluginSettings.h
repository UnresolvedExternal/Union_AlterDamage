#include "Workspace\Utility\Misc.h"

namespace NAMESPACE
{
	struct TItemMarkSettings
	{
		string name;
		int prio;
		string texName;
		int texPos[4];
		int opacity;

		TItemMarkSettings(const string& name);
		string GetTexPosString();
		void SetTexPosString(const string& texPosStr);
		void Save();
		void Load();
	};

	class CPluginSettings
	{
	public:
		static CFlagsSerializer<int> catsSerializer;
		static const char* const pluginName;

		TItemMarkSettings unreadDoc;
		TItemMarkSettings newItem;
		TItemMarkSettings addItem;

		bool removeKeys;

		bool saveTorches;
		bool exchangeTorchOnTeleport;

		bool printTorchName;
		int nameToDescFlags;
		bool appendAmountInfo;

		CPluginSettings();
		CPluginSettings& operator=(const CPluginSettings& other) = delete;
		void Save();
		void Load();
	};

	extern CPluginSettings settings;
	extern CPluginSettings saveSettings;
}