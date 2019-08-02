namespace NAMESPACE
{
	bool CDamageDealer::IsOneHanded(oCItem* weapon)
	{
		return weapon->HasFlag(ITM_FLAG_DAG) || weapon->HasFlag(ITM_FLAG_SWD) || weapon->HasFlag(ITM_FLAG_AXE);
	}

	bool CDamageDealer::IsTwoHanded(oCItem* weapon)
	{
		return weapon->HasFlag(ITM_FLAG_2HD_SWD) || weapon->HasFlag(ITM_FLAG_2HD_AXE);
	}

	bool CDamageDealer::IsMelee(oCItem* weapon)
	{
		return IsOneHanded(weapon) || IsTwoHanded(weapon);
	}

	oEDamageType CDamageDealer::IndexToDamage(const oEDamageIndex& index)
	{
		return (oEDamageType)(1 << index);
	}

	bool CDamageDealer::IsCorrectSpellData(const TDamageInfo& info)
	{
		if (info.spellId < 0 || info.spellId > SPL_MAX_SPELL)
		{
			return false;
		}

		if (info.spellLevel < 1 || info.spellLevel > MAX_SPL_LEVEL)
		{
			return false;
		}

		return true;
	}

	void CDamageDealer::SetWeaponModeByWeapon(TDamageInfo& info)
	{
		if (IsOneHanded(info.weapon))
		{
			info.enuModeWeapon = info.weapon->HasFlag(ITM_FLAG_DAG) ? NPC_WEAPON_DAG : NPC_WEAPON_1HS;
			return;
		}

		if (IsTwoHanded(info.weapon))
		{
			info.enuModeWeapon = NPC_WEAPON_2HS;
			return;
		}

		if (info.weapon->HasFlag(ITM_FLAG_BOW))
		{
			info.enuModeWeapon = NPC_WEAPON_BOW;
			return;
		}

		if (info.weapon->HasFlag(ITM_FLAG_CROSSBOW))
		{
			info.enuModeWeapon = NPC_WEAPON_CBOW;
			return;
		}

		zSTRING message = "Cannot determine used weapon: ";
		message += info.weapon->GetObjectName().ToChar();
		Message::Error(message.ToChar(), "AlterDamage error");
	}

	void CDamageDealer::SetRawDamageByWeapon(TDamageInfo& info, float multiplier)
	{
		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			info.rawDamage[i] = info.weapon->damage[i];

			if (info.weapon->damage[i] <= 0)
			{
				continue;
			}

			if (i == oEDamageIndex::oEDamageIndex_Blunt || i == oEDamageIndex::oEDamageIndex_Edge)
			{
				info.rawDamage[i] += info.npcAttacker->GetAttribute(NPC_ATR_STRENGTH);
				info.rawDamage[i] *= multiplier;
			}

#if !defined(COMPILE_G1) && !defined(COMPILE_G1A)
			if (i == oEDamageIndex::oEDamageIndex_Point)
			{
				info.rawDamage[i] += info.npcAttacker->GetAttribute(NPC_ATR_DEXTERITY);
				info.rawDamage[i] *= multiplier;
			}
#endif
		}
	}

	void CDamageDealer::InitInfo(oCNpc* target, const oCNpc::oSDamageDescriptor& desc, TDamageInfo& info)
	{
		info.vobAttacker = desc.pVobAttacker;
		info.npcAttacker = desc.pNpcAttacker;
		info.target = target;
		info.weapon = desc.pItemWeapon;
		info.spellId = desc.nSpellID;
		info.spellLevel = desc.nSpellLevel;

#if defined(COMPILE_G2) || defined(COMPILE_G2A)
		info.mustNotKill = desc.bDamageDontKill;
#endif

		info.totalDamage = desc.fDamageTotal;
		info.strVisualFX = desc.strVisualFX;

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			info.rawDamage[i] = desc.aryDamage[i];
		}
	}

	void CDamageDealer::SetWeaponMode(TDamageInfo& info)
	{
		if (info.weapon)
		{
			SetWeaponModeByWeapon(info);
			return;
		}

		if (!info.npcAttacker)
		{
			info.enuModeWeapon = NPC_WEAPON_MAX;
			return;
		}

		if (IsCorrectSpellData(info))
		{
			info.enuModeWeapon = NPC_WEAPON_MAG;
			return;
		}

		info.enuModeWeapon = NPC_WEAPON_FIST;
	}

	void CDamageDealer::SetFullRawDamage(TDamageInfo& info)
	{
		static float humanFistDamageMult = zoptions->ReadReal("ALTERDAMAGE", "HumanFistDamageMult", 0.5f);
		static float meleeRawDamageMult = zoptions->ReadReal("ALTERDAMAGE", "MeleeRawDamageMult", 1.0f);

		bool hasOwnDamage;

		switch (info.enuModeWeapon)
		{
		case NPC_WEAPON_FIST:
			hasOwnDamage = false;

			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			{
				int damage = info.npcAttacker->GetDamageByIndex((oEDamageIndex)i);
				hasOwnDamage = hasOwnDamage || (damage > 0);
				info.rawDamage[i] = damage;
			}

			if (hasOwnDamage)
			{
				break;
			}

			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			{
				if (IndexToDamage((oEDamageIndex)i) & info.npcAttacker->damagetype)
				{
					info.rawDamage[i] = info.npcAttacker->GetAttribute(NPC_ATR_STRENGTH);

					if (info.npcAttacker->IsHuman())
					{
						info.rawDamage[i] *= humanFistDamageMult;
					}
				}
			}
			break;

		case NPC_WEAPON_DAG:
		case NPC_WEAPON_1HS:
		case NPC_WEAPON_2HS:
			SetRawDamageByWeapon(info, meleeRawDamageMult);
			break;

		case NPC_WEAPON_BOW:
		case NPC_WEAPON_CBOW:
			SetRawDamageByWeapon(info, 1.0f);
			break;

		case NPC_WEAPON_MAX:
			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			{
				info.rawDamage[i] = 0;
			}
			info.rawDamage[oEDamageIndex::oEDamageIndex_Fall] = info.totalDamage;
			break;

		case NPC_WEAPON_MAG:
			break;

		default:
			Message::Error("Unexpected weapon mode (internal error)", "AlterDamage error");
			break;
		}
	}

	void CDamageDealer::HandleIfMiss(TDamageInfo& info)
	{
		static float meleeMissMult = zoptions->ReadReal("ALTERDAMAGE", "MeleeMissMult", 0.5f);

		if (info.enuModeWeapon != NPC_WEAPON_DAG && info.enuModeWeapon != NPC_WEAPON_1HS && info.enuModeWeapon != NPC_WEAPON_2HS)
		{
			info.isFullMeleeDamage = false;
			return;
		}

#if defined(COMPILE_G2) || defined(COMPILE_G2A)
		info.isFullMeleeDamage = (info.enuModeWeapon == NPC_WEAPON_2HS) ?
			karma->RollSuccess(info.npcAttacker->GetHitChance(NPC_HITCHANCE_2H) / 100.0f) :
			karma->RollSuccess(info.npcAttacker->GetHitChance(NPC_HITCHANCE_1H) / 100.0f);
#else
		info.isFullMeleeDamage = (info.enuModeWeapon == NPC_WEAPON_2HS) ?
			karma->RollSuccess(info.npcAttacker->GetTalentValue(NPC_HITCHANCE_2H) / 100.0f) :
			karma->RollSuccess(info.npcAttacker->GetTalentValue(NPC_HITCHANCE_1H) / 100.0f);
#endif

		if (info.isFullMeleeDamage)
		{
			return;
		}

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			if (i == oEDamageIndex::oEDamageIndex_Blunt || i == oEDamageIndex::oEDamageIndex_Edge || i == oEDamageIndex::oEDamageIndex_Point)
			{
				info.rawDamage[i] *= meleeMissMult;
			}
		}
	}

	void CDamageDealer::SetEffectiveDamage(TDamageInfo& info)
	{
		static float armorSoftCap = zoptions->ReadReal("ALTERDAMAGE", "ArmorSoftCap", 0.65f);
		static float beyondSoftCapArmorScaling = zoptions->ReadReal("ALTERDAMAGE", "BeyondSoftCapArmorScaling", 0.3f);

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			float damage;

			if (info.target->GetProtectionByIndex((oEDamageIndex)i) <= info.rawDamage[i] * armorSoftCap)
			{
				damage = info.rawDamage[i] - info.target->GetProtectionByIndex((oEDamageIndex)i);
			}
			else
			{
				damage = info.rawDamage[i] * (1 - armorSoftCap);
				damage -= beyondSoftCapArmorScaling * (info.target->GetProtectionByIndex((oEDamageIndex)i) - armorSoftCap * info.rawDamage[i]);
			}

			info.effectiveDamage[i] = max(0, damage);
		}
	}

	void CDamageDealer::SetTotalDamage(TDamageInfo& info)
	{
		static float twoHandedMult = zoptions->ReadReal("ALTERDAMAGE", "TwoHandedMult", 1.35f);

		info.totalDamage = 0;

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			info.totalDamage += info.effectiveDamage[i];
		}

		if (info.enuModeWeapon == NPC_WEAPON_2HS)
		{
			info.totalDamage *= twoHandedMult;
		}
	}

	void CDamageDealer::SetRealDamage(TDamageInfo& info)
	{
		if (info.target->HasFlag(NPC_FLAG_IMMORTAL) || info.target->IsSelfPlayer() && oCNpc::godmode)
		{
			info.realDamage = 0;
			return;
		}

		int maxDamage = info.target->GetAttribute(NPC_ATR_HITPOINTS);

#if defined(COMPILE_G1) || defined(COMPILE_G2)
		info.mustNotKill = 
			info.npcAttacker &&
			info.enuModeWeapon != NPC_WEAPON_BOW && 
			info.enuModeWeapon != NPC_WEAPON_CBOW &&
			!(info.enuModeWeapon == NPC_WEAPON_MAG && info.effectiveDamage[oEDamageIndex::oEDamageIndex_Fire] > 1e-4) &&
			!info.target->IsHostile(info.npcAttacker) &&
			!info.target->IsMonster();
#endif

		if (info.mustNotKill && maxDamage > 0)
		{
			maxDamage -= 1;
		}

		info.realDamage = min(maxDamage, (int)(info.totalDamage + 0.5f));
	}

	void CDamageDealer::ApplyDamage(const TDamageInfo& info)
	{
		info.target->SetAttribute(NPC_ATR_HITPOINTS, info.target->GetAttribute(NPC_ATR_HITPOINTS) - info.realDamage);
	}

	void CDamageDealer::AddDots(std::shared_ptr<TDamageInfo> info)
	{
		static float fireDotDamageMult = zoptions->ReadReal("ALTERDAMAGE", "FireDotDamageMult", 0.5f);
		static float fireDotDuration = zoptions->ReadReal("ALTERDAMAGE", "FireDotDuration", 5.0f);

#if defined(COMPILE_G2) || defined(COMPILE_G2A)
		if (!((string)info->strVisualFX.ToChar()).CompareI("VOB_MAGICBURN"))
		{
			return;
		}
#else
		if (info->enuModeWeapon != NPC_WEAPON_MAG)
		{
			return;
		}

		static zCArray<int> fireSpells;

		if (fireSpells.IsEmpty())
		{
			fireSpells.InsertEnd(SPL_FIREBALL);
			fireSpells.InsertEnd(SPL_FIREBOLT);
			fireSpells.InsertEnd(SPL_FIRERAIN);
			fireSpells.InsertEnd(SPL_FIRESTORM);
		}

		if (fireSpells.Search(info->spellId) < 0)
		{
			return;
		}
#endif

		float dotDamage = info->effectiveDamage[(int)oEDamageIndex::oEDamageIndex_Magic];
		dotDamage *= fireDotDamageMult;

		if (dotDamage >= 0.5f)
		{
			new CDotDamage(info, oEDamageIndex::oEDamageIndex_Magic, (int)(dotDamage + 0.5f), fireDotDuration);
		}

		dotDamage = info->effectiveDamage[(int)oEDamageIndex::oEDamageIndex_Fire];
		dotDamage *= fireDotDamageMult;

		if (dotDamage >= 0.5f)
		{
			new CDotDamage(info, oEDamageIndex::oEDamageIndex_Fire, (int)(dotDamage + 0.5f), fireDotDuration);
		}
	}

	void CDamageDealer::UpdateDescriptor(const TDamageInfo& info, oCNpc::oSDamageDescriptor& desc)
	{
		// dont care
	}

	CDamageDealer::CDamageDealer(const std::shared_ptr<CKarma>& karma):
		karma(karma)
	{
	}

	std::shared_ptr<TDamageInfo> CDamageDealer::DoDamage(oCNpc* target, oCNpc::oSDamageDescriptor& desc)
	{
		std::shared_ptr<TDamageInfo> info = std::make_shared<TDamageInfo>();

		InitInfo(target, desc, *info);
		SetWeaponMode(*info);
		SetFullRawDamage(*info);
		HandleIfMiss(*info);
		SetEffectiveDamage(*info);
		SetTotalDamage(*info);
		SetRealDamage(*info);
		ApplyDamage(*info);
		AddDots(info);
		UpdateDescriptor(*info, desc);

		return info;
	}
}