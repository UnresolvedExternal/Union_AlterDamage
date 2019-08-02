#include "CKarma.h"
#include "CGlobalEventSubscription.h"
#include <unordered_map>
#include <memory>

namespace NAMESPACE
{
	std::unordered_map<oCNpc*, std::unique_ptr<CDamageDealer>> damageDealers;
	std::shared_ptr<CKarma> notNpcKarma = std::make_shared<CKarma>();
	CDamageDealer notNpcDamageDealer(notNpcKarma);

	void __fastcall Hook_oCNpc_OnDamage_Hit(oCNpc*, void*, oCNpc::oSDamageDescriptor&);
	CInvoke<void(__thiscall*)(oCNpc*, oCNpc::oSDamageDescriptor&)> Ivk_oCNpc_OnDamage_Hit(nullptr, nullptr, IVK_DISABLED);
	void __fastcall Hook_oCNpc_OnDamage_Hit(oCNpc* _this, void* vtable, oCNpc::oSDamageDescriptor& desc)
	{
		if (desc.aryDamage[0] == TDamageInfo::DISABLE_HOOK)
		{
			desc.aryDamage[0] = 0;
			return Ivk_oCNpc_OnDamage_Hit(_this, desc);
		}

		static bool enablePopupDamage = zoptions->ReadBool("ALTERDAMAGE", "EnablePopupDamage", true);

		CDamageDealer* current;

		if (desc.pNpcAttacker)
		{
			auto it = damageDealers.find(desc.pNpcAttacker);
			
			if (it == damageDealers.end())
			{
				current = new CDamageDealer(std::make_shared<CKarma>());
				damageDealers.insert(std::make_pair(desc.pNpcAttacker, std::unique_ptr<CDamageDealer>(current)));
			}
			else
			{
				current = it->second.get();
			}
		}
		else
		{
			current = &notNpcDamageDealer;
		}

		std::shared_ptr<TDamageInfo> info = current->DoDamage(_this, desc);

		if (enablePopupDamage && info->npcAttacker && info->npcAttacker->IsSelfPlayer() && info->realDamage)
		{
			new CPopupDamageLabel(info); 
		}
	}

	void __fastcall Hook_zCWorld_VobRemovedFromWorld(zCWorld*, void*, zCVob*);
	CInvoke<void(__thiscall*)(zCWorld*, zCVob*)> Ivk_zCWorld_VobRemovedFromWorld(nullptr, nullptr, IVK_DISABLED);
	void __fastcall Hook_zCWorld_VobRemovedFromWorld(zCWorld* _this, void* vtable, zCVob* vob)
	{
		Ivk_zCWorld_VobRemovedFromWorld(_this, vob);

		TDamageInfo::HandleVobRemoved(vob);

		if (vob->type != zVOB_TYPE_NSC)
		{
			return;
		}
		
		auto it = damageDealers.find((oCNpc*)vob);

		if (it == damageDealers.end())
		{
			return;
		}

		damageDealers.erase(it);
	}

	CGlobalEventSubscription* onLoadBegin;
	CGlobalEventSubscription* onExit;

	void OnLoadBegin()
	{
		damageDealers.clear();
	}

	void OnExit()
	{
		delete onLoadBegin;
		delete onExit;
	}

	void Attach()
	{
		onLoadBegin = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::LOAD_BEGIN, &OnLoadBegin);
		onExit = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::EXIT, &OnExit);

		Ivk_oCNpc_OnDamage_Hit.Attach(ADDRESS_OCNPC_ONDAMAGE_HIT, &Hook_oCNpc_OnDamage_Hit);
		Ivk_zCWorld_VobRemovedFromWorld.Attach(ADDRESS_ZCWORLD_VOBREMOVEDFROMWORLD, &Hook_zCWorld_VobRemovedFromWorld);
	}
}