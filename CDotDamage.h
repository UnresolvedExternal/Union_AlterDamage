#include "CGlobalEventSubscription.h"
#include <memory>

namespace NAMESPACE
{
	class CDotDamage
	{
	private:
		CGlobalEventSubscription* onLoop;

		std::shared_ptr<TDamageInfo> info;
		float creationTimeInSecs;
		const float durationInSecs;
		const int damage;
		oEDamageIndex damageIndex;
		int damageDone;

		void OnLoop();
		void DoDamage(int debt);

	public:
		CDotDamage(std::shared_ptr<TDamageInfo> info, oEDamageIndex damageIndex, int damage, float durationInSecs);
		~CDotDamage();
	};
}