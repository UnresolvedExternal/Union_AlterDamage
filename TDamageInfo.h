#include <vector>
#include <algorithm>

namespace NAMESPACE
{
	class CDotDamage;

	struct TDamageInfo
	{
		static const unsigned long DISABLE_HOOK = 0xFFAAFFAA;
		static std::vector<TDamageInfo*> infos;

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
		int realDamage;
		bool isFullMeleeDamage;
		zSTRING strVisualFX;
		std::vector<CDotDamage*> dots;

		static void HandleVobRemoved(zCVob* vob);

		TDamageInfo();
		void DoDamageUnhooked(int damage);
		~TDamageInfo();
	};
}