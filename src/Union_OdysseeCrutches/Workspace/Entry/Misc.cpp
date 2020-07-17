#include <type_traits>

namespace NAMESPACE
{
	CSubscription loadSettings(TGameEvent::Init, []()
		{
			settings.Load();
		}, CHECK_THIS_ENGINE);
}