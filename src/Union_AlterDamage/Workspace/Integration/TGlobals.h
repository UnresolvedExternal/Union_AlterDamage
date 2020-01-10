#include <unordered_map>

namespace NAMESPACE
{
	class CHitGenContainer;

	struct TGlobals
	{
		static CPluginSettings pluginSettings;
		static bool disableOnHitHook;
		static std::unordered_map<oCNpc*, CHitGenContainer> hitGens;
	};
}