#include <memory>
#include "Workspace\Memory\CMemPool.h"
#include "Workspace\Events\CSubscription.h"

namespace NAMESPACE
{
	class CDotDamage
	{
		MEMPOOL_DECL(CDotDamage)

	private:
		CSubscription onLoop;

		std::shared_ptr<TDamageInfo> info;
		CTimer timer;
		bool sendMessages;
		float creationTimeInSecs;
		const float durationInSecs;
		const int damage;
		oEDamageIndex damageIndex;
		int damageDone;
		int assessDamageDebt;

		void OnLoop();
		void DoDamage(int debt);

	public:
		CDotDamage(std::shared_ptr<TDamageInfo> info, oEDamageIndex damageIndex, int damage, float durationInSecs);
		~CDotDamage();
	};
}