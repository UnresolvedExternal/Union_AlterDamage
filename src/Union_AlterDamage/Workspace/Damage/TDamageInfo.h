#include <vector>
#include <algorithm>
#include "Workspace\Memory\CMemPool.h"

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

		static void HandleVobRemoved(zCVob* vob);
		static void NotifyDamage(oCNpc* target, int damage);

		TDamageInfo();
		void DoDamageUnhooked(int damage);
		~TDamageInfo();
	};
}