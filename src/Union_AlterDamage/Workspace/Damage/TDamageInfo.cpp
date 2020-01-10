#include "Workspace\Utility\TScope.h"
#include "Workspace\Utility\Misc.h"
#include <algorithm>

namespace NAMESPACE
{
	MEMPOOL_DEF(TDamageInfo)

	CArray<TDamageInfo*> TDamageInfo::infos;

	void TDamageInfo::HandleVobRemoved(zCVob* vob)
	{
		for (int i = infos.GetNum() - 1; i >= 0; i--)
		{
			TDamageInfo& info = *infos[i];

			if (info.vobAttacker == vob)
			{
				info.vobAttacker = nullptr;
				info.npcAttacker = nullptr;
			}

			if (info.target == vob)
				info.target = nullptr;
		}
	}

	void TDamageInfo::NotifyDamage(oCNpc* target, int damage)
	{
		if (!infos.GetNum())
			return;

		auto it = std::find_if(std::make_reverse_iterator(end(infos)), std::make_reverse_iterator(begin(infos)),
			[&](TDamageInfo* info)
			{
				return info->target == target;
			});

		if (it == std::make_reverse_iterator(begin(infos)))
			return;

		(*it)->realDamage += damage;
		(*it)->lastUpdate = ztimer->totalTimeFloat / 1000.0f;
	}

	TDamageInfo::TDamageInfo()
	{
		infos.Insert(this);
	}

	void TDamageInfo::DoDamageUnhooked(int damage)
	{
		oCNpc::oSDamageDescriptor desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.pVobAttacker = npcAttacker;
		desc.pNpcAttacker = npcAttacker;
		desc.aryDamage[oEDamageIndex::oEDamageIndex_Fall] = damage + target->protection[oEDamageIndex::oEDamageIndex_Fall];
		desc.fDamageTotal = damage;
		desc.enuModeWeapon = NPC_WEAPON_MAX;
		desc.bFinished = 1;
		desc.fDamageMultiplier = 1;
		desc.enuModeDamage = 128;
		desc.bIsDead = 1;

#if (CurrentEngine == Engine_G2) || (CurrentEngine == Engine_G2A)
		desc.bDamageDontKill = mustNotKill;
#endif

		desc.dwFieldsValid |= oCNpc::oEDescDamageFlags::oEDamageDescFlag_Attacker;
		desc.dwFieldsValid |= oCNpc::oEDescDamageFlags::oEDamageDescFlag_Damage;

		{
			auto scope = AssignTemp(TGlobals::disableOnHitHook, true);
			target->OnDamage(desc);
		}

		int health = target->GetAttribute(NPC_ATR_HITPOINTS);

		if (health <= 0)
		{
			target->ResetPos(target->GetPositionWorld());
			target->SetAttribute(NPC_ATR_HITPOINTS, health);
		}
	}

	TDamageInfo::~TDamageInfo()
	{
		infos.Remove(this);
	}
}