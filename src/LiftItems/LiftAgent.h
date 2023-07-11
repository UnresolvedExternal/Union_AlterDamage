#include "CGlobalEventSubscription.h"
#include <vector>

namespace NAMESPACE
{
	class LiftAgent
	{
	private:
		enum TDebugMode { ON, OFF };

		std::vector<CGlobalEventSubscription> subs;
		CInvoke_zCWorld_VobRemovedFromWorld removeIvk;
		Lift lift;
		TDebugMode debugMode;
		Timer timer;

		void OnLoadEnd();
		void OnGameLoop();
		void OnRemove(zCWorld*, zCVob* vob);

	public:
		LiftAgent();
	};
}