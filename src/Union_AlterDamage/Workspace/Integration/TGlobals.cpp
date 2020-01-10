#include "Workspace\Events\CSubscription.h"
#include <unordered_map>

namespace NAMESPACE
{
	CPluginSettings TGlobals::pluginSettings;
	bool TGlobals::disableOnHitHook = false;
	std::unordered_map<oCNpc*, CHitGenContainer> TGlobals::hitGens;
}