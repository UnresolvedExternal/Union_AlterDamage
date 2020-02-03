#include "Workspace\Utility\TScope.h"
#include "Workspace\Utility\Misc.h"
#include <algorithm>

namespace NAMESPACE
{
	MEMPOOL_DEF(TDamageInfo)

	CArray<TDamageInfo*> TDamageInfo::infos;

	void TDamageInfo::HandleFightSounds()
	{
		if (!infos.GetNum())
			return;

		static int index = 0;
		if (index >= (int)infos.GetNum())
			index = 0;

		int startIndex = index;
		int needHandle = 1;
		
		while (true)
		{
			while (needHandle && infos[index]->fightSoundsQueue.Size())
			{
				oCNpc* npc = infos[index]->fightSoundsQueue.Dequeu(ztimer->totalTimeFloat / 1000.0f);
				if (npc == infos[index]->target || npc == infos[index]->npcAttacker || !npc->globalVobTreeNode || !infos[index]->target || !infos[index]->npcAttacker)
					continue;
				npc->AssessFightSound_S(infos[index]->npcAttacker, infos[index]->target->GetPositionWorld(), infos[index]->target);
				needHandle -= 1;
			}

			index = (index + 1) % infos.GetNum();
			if (index == startIndex || !needHandle)
				break;
		}
	}

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

	TDamageInfo::TDamageInfo() :
		fightSoundsQueue(5.0f)
	{
		infos.Insert(this);
	}

	void TDamageInfo::AssessDamage(int damage)
	{
		target->AssessDamage_S(npcAttacker, damage);
	}

	void TDamageInfo::AssessFightSound()
	{
		zCArray<zCVob*>& vobs = ogame->GetWorld()->activeVobList;
		for (int i = 0; i < vobs.GetNum(); i++)
			if (vobs[i]->GetVobType() == zVOB_TYPE_NSC)
				fightSoundsQueue.Enqueu(vobs[i]->CastTo<oCNpc>(), ztimer->totalTimeFloat / 1000.0f);
	}

	void TDamageInfo::Kill()
	{
		target->DropUnconscious(0, npcAttacker);
		target->SetAttribute(NPC_ATR_HITPOINTS, 0);
		target->GetModel()->StopAnisLayerRange(0, zMDL_MAX_ANIS_PARALLEL);
		target->DoDie(npcAttacker);
		
		oCSVMManager man;

		if (target->voice < 0 || target->voice >= man.svm_max)
			return;
		int ou = man.sv_module[target->voice].GetOU("DEAD");

		if (ou != -1)
		{
			zCCSBlock* block = ogame->GetCutsceneManager()->LibGet(ou);
			zCSoundSystem::zTSound3DParams params;
			zsound->GetSound3DProps(0, params);
			zsound->PlaySound3D(block->GetRoleName(), target, 0, &params);
		}
	}

	void TDamageInfo::DropUnconscious()
	{
		target->DropUnconscious(0, npcAttacker);
	}

	TDamageInfo::~TDamageInfo()
	{
		infos.Remove(this);
	}
}