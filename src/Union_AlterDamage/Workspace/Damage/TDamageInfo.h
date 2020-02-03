#include <algorithm>
#include "Workspace\Memory\CMemPool.h"
#include "Workspace\Utility\CThrottledQueue.h"

namespace NAMESPACE
{
	class CDotDamage;

	struct TDamageInfo
	{
		MEMPOOL_DECL(TDamageInfo)

	public:
		static CArray<TDamageInfo*> infos;

		unsigned long enuModeWeapon;
		zCVob* vobAttacker;
		oCNpc* npcAttacker;
		oCNpc* target;
		oCItem* weapon;
		unsigned long spellId;
		unsigned long spellCat;
		unsigned long spellLevel;
		bool mustNotKill;
		float rawDamage[oEDamageIndex_MAX];
		float effectiveDamage[oEDamageIndex_MAX];
		float totalDamage;
		float lastUpdate;
		int realDamage;
		int scalingAttr;
		oEDamageIndex primaryDamage;
		bool IsCrit;
		zSTRING strVisualFX;
		CArray<CDotDamage*> dots;
		CThrottledQueue<oCNpc*> fightSoundsQueue;

		static void HandleFightSounds();
		static void HandleVobRemoved(zCVob* vob);
		static void NotifyDamage(oCNpc* target, int damage);

		TDamageInfo();
		void AssessDamage(int damage);
		void AssessFightSound();
		void Kill();
		void DropUnconscious();
		~TDamageInfo();
	};
}