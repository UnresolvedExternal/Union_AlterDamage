namespace NAMESPACE
{
	std::vector<TDamageInfo*> TDamageInfo::infos;

	void TDamageInfo::HandleVobRemoved(zCVob* vob)
	{
		for (int i = infos.size() - 1; i >= 0; i--)
		{
			TDamageInfo& info = *infos[i];

			if (info.vobAttacker == vob)
			{
				info.vobAttacker = nullptr;
				info.npcAttacker = nullptr;
			}

			if (info.target == vob)
			{
				info.target = nullptr;
			}
		}
	}

	TDamageInfo::TDamageInfo()
	{
		infos.push_back(this);
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
		desc.aryDamage[0] = TDamageInfo::DISABLE_HOOK;

#if defined(COMPILE_G2) || defined(COMPILE_G2A)
		desc.bDamageDontKill = mustNotKill;
#endif

		desc.dwFieldsValid |= oCNpc::oEDescDamageFlags::oEDamageDescFlag_Attacker;
		desc.dwFieldsValid |= oCNpc::oEDescDamageFlags::oEDamageDescFlag_Damage;
		//desc.dwFieldsValid |= oCNpc::oEDescDamageFlags::oEDamageDescFlag_DamageType;

		target->OnDamage(desc);
		int health = target->GetAttribute(NPC_ATR_HITPOINTS);

		if (health <= 0)
		{
			target->ResetPos(target->GetPositionWorld());
			target->SetAttribute(NPC_ATR_HITPOINTS, health);
		}
	}

	TDamageInfo::~TDamageInfo()
	{
		auto it = std::find(infos.begin(), infos.end(), this);
		infos.erase(it);
	}
}