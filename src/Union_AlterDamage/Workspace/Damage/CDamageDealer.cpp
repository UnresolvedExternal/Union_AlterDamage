#include "Workspace\Utility\Misc.h"

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

	int CDamageDealer::GetBestScalingAttribute(oCItem* weapon)
	{
		int maxCond = 0;
		int maxIndex = -1;
		for (int i = 0; i < ITM_COND_MAX; i++)
			if (weapon->cond_value[i] > maxCond)
			{
				maxIndex = i;
				maxCond = weapon->cond_value[i];
			}
		return (maxIndex != -1) ? weapon->cond_atr[maxIndex] : -1;
	}

	void CDamageDealer::GetIsMeleeAndTalent(const TDamageInfo& info, bool& isMelee, int& talent)
	{
		switch (info.enuModeWeapon)
		{
		case NPC_WEAPON_DAG:
		case NPC_WEAPON_1HS:
			talent = NPC_HITCHANCE_1H;
			isMelee = true;
			break;
		case NPC_WEAPON_2HS:
			talent = NPC_HITCHANCE_2H;
			isMelee = true;
			break;
		case NPC_WEAPON_BOW:
			talent = NPC_HITCHANCE_BOW;
			isMelee = false;
			break;
		case NPC_WEAPON_CBOW:
			talent = NPC_HITCHANCE_CROSSBOW;
			isMelee = false;
			break;
		default:
			CLogger::LogError("Not weapon mode in GetIsMeleeAndTalent()");
			return;
		}
	}

	int CDamageDealer::GetComboHit(const TDamageInfo& info)
	{
		if (info.npcAttacker && info.npcAttacker->anictrl && (info.enuModeWeapon == NPC_WEAPON_FIST || info.weapon && IsMelee(info.weapon)))
			return info.npcAttacker->anictrl->comboNr;
		return 0;
	}

	oEDamageType CDamageDealer::IndexToDamage(const oEDamageIndex& index)
	{
		return (oEDamageType)(1 << index);
	}

	bool CDamageDealer::IsCorrectSpellData(const TDamageInfo& info)
	{
		if (info.spellId < 0 || info.spellId > SPL_MAX_SPELL)
			return false;
		if (info.spellLevel < 1 || info.spellLevel > MAX_SPL_LEVEL)
			return false;
		return true;
	}

	void CDamageDealer::SetWeaponModeByWeapon(TDamageInfo & info)
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

		CLogger::LogError(Z"Cannot determine used weapon: " += info.weapon->GetObjectName().ToChar());
	}

	void CDamageDealer::SetRawDamageByWeapon(TDamageInfo& info)
	{
		oCItem* weapon = info.weapon;
		info.scalingAttr = -1;
		info.IsCrit = false;
		int bestScalingAttr = GetBestScalingAttribute(weapon);

		bool isMelee;
		int talent;
		GetIsMeleeAndTalent(info, isMelee, talent);
		float chance = GetHitChance(info.npcAttacker, talent);
		chance += GetComboHit(info) * TGlobals::pluginSettings.comboChanceAdd / 100.0f;

		if (TGlobals::attackRun)
			chance += TGlobals::pluginSettings.attackRunChanceAdd / 100.0f;

		chance = CoerceInRange(chance, 0.0f, 0.0f, 1.0f);

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			info.rawDamage[i] = weapon->damage[i];

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			if (weapon->damage[i] <= 0)
				continue;
			float& damage = info.rawDamage[i];

			if (i == oEDamageIndex::oEDamageIndex_Blunt && isMelee)
			{
				info.scalingAttr = NPC_ATR_STRENGTH;
				info.IsCrit = (*hitGen)[talent].GenNext(chance);
				float mult =  info.IsCrit ? 1.0f : TGlobals::pluginSettings.meleeMissMult;
				damage = damage * TGlobals::pluginSettings.meleeRawDamageMult * mult + info.npcAttacker->GetAttribute(info.scalingAttr) * ZenDef(1.0f, 1.0f, TGlobals::pluginSettings.meleeRawDamageMult * mult, TGlobals::pluginSettings.meleeRawDamageMult * mult);
				break;
			}

			if (i == oEDamageIndex::oEDamageIndex_Edge && isMelee)
			{
				int attrValue;
				if (TGlobals::pluginSettings.requiredAttrScaling && bestScalingAttr != -1)
				{
					info.scalingAttr = bestScalingAttr;
					attrValue = info.npcAttacker->GetAttribute(info.scalingAttr);
				}
				else
				{
					info.scalingAttr = NPC_ATR_STRENGTH;
					attrValue = info.npcAttacker->GetAttribute(info.scalingAttr);
				}
				info.IsCrit = (*hitGen)[talent].GenNext(chance);
				float mult = info.IsCrit ? 1.0f : TGlobals::pluginSettings.meleeMissMult;
				damage = damage * TGlobals::pluginSettings.meleeRawDamageMult * mult + attrValue * ZenDef(1.0f, 1.0f, TGlobals::pluginSettings.meleeRawDamageMult * mult, TGlobals::pluginSettings.meleeRawDamageMult * mult);
				break;
			}

			if (i == oEDamageIndex::oEDamageIndex_Point && !isMelee)
			{
				int attrValue;
				if (TGlobals::pluginSettings.requiredAttrScaling)
				{
					info.scalingAttr = (info.enuModeWeapon == NPC_WEAPON_BOW) ? NPC_ATR_DEXTERITY : NPC_ATR_STRENGTH;
					attrValue = info.npcAttacker->GetAttribute(info.scalingAttr);
				}
				else
				{
					info.scalingAttr = NPC_ATR_DEXTERITY;
					attrValue = info.npcAttacker->GetAttribute(info.scalingAttr);
				}
				info.scalingAttr = ZenDef(-1, -1, info.scalingAttr, info.scalingAttr);
				info.IsCrit = ZenDef(true, true, false, false) ? (*hitGen)[talent].GenNext(chance) : false;
				float mult = info.IsCrit ? 1.0f : 0.5f;
				damage = damage * ZenDef(2.0f * mult, 2.0f * mult, 1.0f, 1.0f) + attrValue * ZenDef(0.0f, 0.0f, 1.0f, 1.0f);
				break;
			}

			if (i == oEDamageIndex::oEDamageIndex_Point && isMelee)
			{
				float mult = (*hitGen)[talent].GenNext(chance) ? 1.0f : TGlobals::pluginSettings.meleeMissMult;
				info.scalingAttr = NPC_ATR_DEXTERITY;
				damage = damage * TGlobals::pluginSettings.meleeRawDamageMult * mult + info.npcAttacker->GetAttribute(info.scalingAttr) *
					ZenDef(1.0f, 1.0f, TGlobals::pluginSettings.meleeRawDamageMult * mult, TGlobals::pluginSettings.meleeRawDamageMult * mult);
				break;
			}
		}
	}

	float CDamageDealer::GetHitChance(oCNpc* npc, int talent)
	{
		float chance;
#if (CurrentEngine == Engine_G2) || (CurrentEngine == Engine_G2A)
		if (npc->IsMonster())
			return 1.0f;
		chance = npc->GetHitChance(talent) / 100.0f;
#else
		chance = npc->GetTalentValue(talent) / 100.0f;
#endif
		return CoerceInRange(chance, 0.0f, 0.0f, 1.0f);
	}

	void CDamageDealer::InitInfo(oCNpc* target, const oCNpc::oSDamageDescriptor& desc, TDamageInfo& info)
	{
		info.vobAttacker = desc.pVobAttacker;
		info.npcAttacker = desc.pNpcAttacker;
		info.target = target;
		info.weapon = desc.pItemWeapon;
		info.spellId = desc.nSpellID;
		info.spellLevel = desc.nSpellLevel;
		info.strVisualFX = desc.strVisualFX;

#if (CurrentEngine == Engine_G2) || (CurrentEngine == Engine_G2A)
		info.mustNotKill = desc.bDamageDontKill;
		if (info.strVisualFX.IsEmpty())
			if (desc.pFXHit)
				info.strVisualFX = desc.pFXHit->emFXCollDynPerc_S;
#endif

		info.totalDamage = desc.fDamageTotal;
		info.lastUpdate = ztimer->totalTimeFloat / 1000.0f;
		info.IsCrit = false;

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			info.rawDamage[i] = desc.aryDamage[i];
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

	void CDamageDealer::SetRawDamage(TDamageInfo& info)
	{
		bool hasOwnDamage;
		info.scalingAttr = -1;

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
				break;

			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			{
				if (IndexToDamage((oEDamageIndex)i) & info.npcAttacker->damagetype)
				{
					info.scalingAttr = NPC_ATR_STRENGTH;
					info.rawDamage[i] = info.npcAttacker->GetAttribute(info.scalingAttr);
					if (info.npcAttacker->IsHuman())
						info.rawDamage[i] *= TGlobals::pluginSettings.humanFistDamageMult;
				}
			}
			break;

		case NPC_WEAPON_DAG:
		case NPC_WEAPON_1HS:
		case NPC_WEAPON_2HS:
		case NPC_WEAPON_BOW:
		case NPC_WEAPON_CBOW:
			SetRawDamageByWeapon(info);
			break;

		case NPC_WEAPON_MAX:
			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
				info.rawDamage[i] = 0;
			info.rawDamage[oEDamageIndex::oEDamageIndex_Fall] = info.totalDamage;
			break;

		case NPC_WEAPON_MAG:
			if (!info.npcAttacker || info.npcAttacker->GetTalentSkill(oCNpcTalent::NPC_TAL_MAGE) <= 0)
				break;

#if CurrentEngine == Engine_G2 || CurrentEngine == Engine_G2A
			if (info.spellId >= SPL_PALLIGHT && info.spellId <= SPL_PALTELEPORTSECRET)
				break;
#endif
			
			for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
				info.rawDamage[i] += TGlobals::pluginSettings.mageMaxManaToDamage * info.npcAttacker->attribute[NPC_ATR_MANAMAX] * info.rawDamage[i];

			break;

		default:
			CLogger::LogError(Z"Unexpected weapon mode: " += info.enuModeWeapon);
			break;
		}
	}

	void CDamageDealer::SetEffectiveDamage(TDamageInfo& info)
	{
		float armorSoftCap = TGlobals::pluginSettings.armorSoftCap;
		float beyondSoftCapArmorScaling = TGlobals::pluginSettings.beyondSoftCapArmorScaling;

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
		{
			if (info.target->GetProtectionByIndex((oEDamageIndex)i) == -1)
			{
				info.effectiveDamage[i] = 0;
				continue;
			}

			float damage;

			if (info.target->GetProtectionByIndex((oEDamageIndex)i) <= info.rawDamage[i] * armorSoftCap)
				damage = info.rawDamage[i] - info.target->GetProtectionByIndex((oEDamageIndex)i);
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
		float twoHandedMult = TGlobals::pluginSettings.twoHandedMult;
		info.totalDamage = 0;

		for (int i = 0; i < (int)oEDamageIndex::oEDamageIndex_MAX; i++)
			info.totalDamage += info.effectiveDamage[i];

		if (info.enuModeWeapon == NPC_WEAPON_2HS)
			info.totalDamage *= twoHandedMult;

		if (TGlobals::attackRun)
			info.totalDamage *= TGlobals::pluginSettings.attackRunMult;

		info.totalDamage *= 1.0f + GetComboHit(info) * TGlobals::pluginSettings.comboDamageAdd;
	}

	void CDamageDealer::SetRealDamage(TDamageInfo& info)
	{
		if (info.target->HasFlag(NPC_FLAG_IMMORTAL) || info.target->IsSelfPlayer() && oCNpc::godmode)
		{
			info.realDamage = 0;
			return;
		}

		int maxDamage = info.target->GetAttribute(NPC_ATR_HITPOINTS);

#if (CurrentEngine == Engine_G1) || (CurrentEngine == Engine_G1A)
		info.mustNotKill =
			info.npcAttacker &&
			info.enuModeWeapon != NPC_WEAPON_BOW &&
			info.enuModeWeapon != NPC_WEAPON_CBOW &&
			!(info.enuModeWeapon == NPC_WEAPON_MAG && info.effectiveDamage[oEDamageIndex::oEDamageIndex_Fire] > 1e-4) &&
			!info.target->IsHostile(info.npcAttacker) &&
			!info.target->IsMonster();
#endif

		zCPar_Symbol* minDamageVar = parser->GetSymbol(TGlobals::pluginSettings.minDamageVarName.GetVector());
		int minDamage = minDamageVar ? minDamageVar->single_intdata : 0;

		info.realDamage = max(minDamage, (int)(info.totalDamage + 0.5f));
		info.realDamage = min(maxDamage, info.realDamage);
	}

	void CDamageDealer::ApplyDamage(const TDamageInfo& info)
	{
		info.target->SetAttribute(NPC_ATR_HITPOINTS, info.target->GetAttribute(NPC_ATR_HITPOINTS) - info.realDamage);
	}

	void CDamageDealer::AddDots(std::shared_ptr<TDamageInfo> info)
	{
		float fireDotDamageMult = TGlobals::pluginSettings.fireDotDamageMult;
		float fireDotDuration = TGlobals::pluginSettings.fireDotDuration;

#if (CurrentEngine == Engine_G2) || (CurrentEngine == Engine_G2A)
		if (!((string)info->strVisualFX).HasWordI("VOB_MAGICBURN"))
			return;
#else
		if (info->enuModeWeapon != NPC_WEAPON_MAG)
			return;

		static zCArray<int> fireSpells;

		if (fireSpells.IsEmpty())
		{
			fireSpells.InsertEnd(SPL_FIREBALL);
			fireSpells.InsertEnd(SPL_FIREBOLT);
			fireSpells.InsertEnd(SPL_FIRERAIN);
			fireSpells.InsertEnd(SPL_FIRESTORM);
		}

		if (fireSpells.Search(info->spellId) < 0)
			return;
#endif

		float dotDamage = info->effectiveDamage[(int)oEDamageIndex::oEDamageIndex_Magic];
		dotDamage *= fireDotDamageMult;

		if (dotDamage >= 0.5f)
			new CDotDamage(info, oEDamageIndex::oEDamageIndex_Magic, (int)(dotDamage + 0.5f), fireDotDuration);

		dotDamage = info->effectiveDamage[(int)oEDamageIndex::oEDamageIndex_Fire];
		dotDamage *= fireDotDamageMult;

		if (dotDamage >= 0.5f)
			new CDotDamage(info, oEDamageIndex::oEDamageIndex_Fire, (int)(dotDamage + 0.5f), fireDotDuration);
	}

	void CDamageDealer::UpdateDescriptor(const TDamageInfo& info, oCNpc::oSDamageDescriptor& desc)
	{
		// stagger
		desc.fDamageReal = info.realDamage ? max(info.realDamage, 2) : 0;

		// blood
		desc.fDamageEffective = info.realDamage;

		for (int i = 0; i < oEDamageIndex::oEDamageIndex_MAX; i++)
			desc.aryDamageEffective[i] = info.effectiveDamage[i];
	}

	CDamageDealer::CDamageDealer(CHitGenContainer* hitGen) :
		hitGen(hitGen)
	{
	}

	std::shared_ptr<TDamageInfo> CDamageDealer::DoDamage(oCNpc* target, oCNpc::oSDamageDescriptor & desc)
	{
		std::shared_ptr<TDamageInfo> info = std::make_shared<TDamageInfo>();

		InitInfo(target, desc, *info);
		SetWeaponMode(*info);
		SetRawDamage(*info);
		SetEffectiveDamage(*info);
		SetTotalDamage(*info);
		SetRealDamage(*info);
		ApplyDamage(*info);
		AddDots(info);
		UpdateDescriptor(*info, desc);

		return info;
	}
}