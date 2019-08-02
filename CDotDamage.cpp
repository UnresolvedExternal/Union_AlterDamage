#include <functional>
#include <algorithm>

namespace NAMESPACE
{
	void CDotDamage::OnLoop()
	{
		if (ogame->singleStep)
		{
			return;
		}

		if (!info->target || !info->target->globalVobTreeNode)
		{
			delete this;
			return;
		}


		if (info->target->GetAttribute(NPC_ATR_HITPOINTS) <= 0)
		{
			delete this;
			return;
		}

		if (info->mustNotKill && info->target->IsUnconscious())
		{
			delete this;
			return;
		}

		float currentTime = ztimer->totalTimeFloat / 1000.0f;
		float needDamage = min(currentTime - creationTimeInSecs, durationInSecs) / durationInSecs * damage;
		int debt = (int)(needDamage + 0.5f) - damageDone;

		if (debt)
		{
			DoDamage(debt);
			damageDone += debt;
		}

		if (damageDone >= damage)
		{
			delete this;
			return;
		}
	}

	void CDotDamage::DoDamage(int debt)
	{
		if(info->target->HasFlag(NPC_FLAG_IMMORTAL) || info->target->IsSelfPlayer() && oCNpc::godmode)
		{
			return;
		}

		int maxDamage = info->target->GetAttribute(NPC_ATR_HITPOINTS) - 1;

		if (info->mustNotKill && maxDamage > 0)
		{
			maxDamage -= 1;
		}

		int realDamage = min(maxDamage, debt);

		info->target->SetAttribute(NPC_ATR_HITPOINTS, info->target->GetAttribute(NPC_ATR_HITPOINTS) - realDamage);

		info->rawDamage[(int)damageIndex] += debt;
		info->effectiveDamage[(int)damageIndex] += debt;
		info->totalDamage += debt;
		info->realDamage += realDamage;

		if (realDamage < debt)
		{
			info->realDamage += 1;
			info->DoDamageUnhooked(1);
		}
	}

	CDotDamage::CDotDamage(std::shared_ptr<TDamageInfo> info, oEDamageIndex damageIndex, int damage, float durationInSecs):
		info(info),
		damageIndex(damageIndex),
		damage(damage),
		durationInSecs(durationInSecs),
		damageDone(0)
	{
		onLoop = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::LOOP, std::bind(&CDotDamage::OnLoop, this));
		creationTimeInSecs = ztimer->totalTimeFloat / 1000.0f;
		info->dots.push_back(this);
	}

	CDotDamage::~CDotDamage()
	{
		delete onLoop;
		auto it = std::find(info->dots.begin(), info->dots.end(), this);
		info->dots.erase(it);
	}
}