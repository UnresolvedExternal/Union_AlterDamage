#include <algorithm>

namespace NAMESPACE
{
	MEMPOOL_DEF(CDotDamage)

	void CDotDamage::OnLoop()
	{
		timer.Attach();
		timer.Suspend(0, ogame->singleStep);
		sendMessages = timer(0, 200);

		if (ogame->singleStep)
			return;
		if (!info->target || !info->target->globalVobTreeNode)
			return delete this;
		if (info->target->GetAttribute(NPC_ATR_HITPOINTS) <= 0)
			return delete this;
		if (info->mustNotKill && info->target->IsUnconscious())
			return delete this;
		if (sendMessages)
			info->AssessFightSound();

		float currentTime = ztimer->totalTimeFloat / 1000.0f;
		info->lastUpdate = currentTime;
		float needDamage = min(currentTime - creationTimeInSecs, durationInSecs) / durationInSecs * damage;
		int debt = (int)(needDamage + 0.5f) - damageDone;

		if (debt)
		{
			DoDamage(debt);
			damageDone += debt;
		}

		if (damageDone >= damage)
			return delete this;
	}

	void CDotDamage::DoDamage(int debt)
	{
		if (info->target->HasFlag(NPC_FLAG_IMMORTAL) || info->target->IsSelfPlayer() && oCNpc::godmode)
			return;
		
		int maxDamage = info->target->GetAttribute(NPC_ATR_HITPOINTS);
		if (info->mustNotKill && maxDamage > 0)
			maxDamage -= 1;

		int realDamage = min(maxDamage, debt);
		info->target->SetAttribute(NPC_ATR_HITPOINTS, info->target->GetAttribute(NPC_ATR_HITPOINTS) - realDamage);

		info->rawDamage[(int)damageIndex] += debt;
		info->effectiveDamage[(int)damageIndex] += debt;
		info->totalDamage += debt;
		info->realDamage += realDamage;
		assessDamageDebt += realDamage;

		if (sendMessages)
		{
			info->AssessDamage(assessDamageDebt);
			assessDamageDebt = 0;
		}

		if (info->target->GetAttribute(NPC_ATR_HITPOINTS) <= 1)
		{
			if (info->mustNotKill)
				info->DropUnconscious();
			else
				info->Kill();
			return delete this;
		}
	}

	CDotDamage::CDotDamage(std::shared_ptr<TDamageInfo> info, oEDamageIndex damageIndex, int damage, float durationInSecs) :
		info(info),
		damageIndex(damageIndex),
		damage(damage),
		durationInSecs(durationInSecs),
		damageDone(0),
		assessDamageDebt(0)
	{
		onLoop.Reset(TGameEvent::Loop, std::bind(&CDotDamage::OnLoop, this));
		creationTimeInSecs = ztimer->totalTimeFloat / 1000.0f;
		info->dots.Insert(this);
	}

	CDotDamage::~CDotDamage()
	{
		info->dots.Remove(this);
	}
}