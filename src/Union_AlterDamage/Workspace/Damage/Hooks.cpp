#include "Workspace\Utility\CBinSeqGen.h"
#include "Workspace\Utility\Misc.h"
#include <vector>

namespace NAMESPACE
{
	CSubscription loadSettings(TGameEvent::Init, []()
		{
			TGlobals::pluginSettings.Load();
		}, SubEnabled(CurrentEngine));

	CSubscription clearHitGens(TGameEvent::LoadBegin, []()
		{
			TGlobals::hitGens.clear();
		}, SubEnabled(CurrentEngine));

	void __fastcall Hook_oCNpc_Archive(oCNpc*, void*, zCArchiver&);
	CInvoke<void(__thiscall*)(oCNpc*, zCArchiver&)> Ivk_oCNpc_Archive(ZenDef<TInstance>(0x006A2680, 0x006D4EE0, 0x006E79D0, 0x00746470), &Hook_oCNpc_Archive, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCNpc_Archive(oCNpc* _this, void* vtable, zCArchiver& arc)
	{
		Ivk_oCNpc_Archive(_this, arc);
		TGlobals::hitGens[_this].Archive(arc);
	}

	void __fastcall Hook_oCNpc_Unarchive(oCNpc*, void*, zCArchiver&);
	CInvoke<void(__thiscall*)(oCNpc*, zCArchiver&)> Ivk_oCNpc_Unarchive(ZenDef<TInstance>(0x006A31E0, 0x006D5AD0, 0x006E8790, 0x00747230), &Hook_oCNpc_Unarchive, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCNpc_Unarchive(oCNpc* _this, void* vtable, zCArchiver& arc)
	{
		Ivk_oCNpc_Unarchive(_this, arc);
		TGlobals::hitGens[_this].Clear();
		TGlobals::hitGens[_this].Unarchive(arc);
	}

	void __fastcall Hook_zCWorld_VobRemovedFromWorld(zCWorld*, void*, zCVob*);
	CInvoke<void(__thiscall*)(zCWorld*, zCVob*)> Ivk_zCWorld_VobRemovedFromWorld(ZenDef<TInstance>(0x005F64C0, 0x006174A0, 0x0061D1E0, 0x00624970), &Hook_zCWorld_VobRemovedFromWorld, IvkEnabled(CurrentEngine));
	void __fastcall Hook_zCWorld_VobRemovedFromWorld(zCWorld* _this, void* vtable, zCVob* vob)
	{
		Ivk_zCWorld_VobRemovedFromWorld(_this, vob);
		TDamageInfo::HandleVobRemoved(vob);
	}

	void __fastcall Hook_oCNpc_OnDamage_Hit(oCNpc*, void*, oCNpc::oSDamageDescriptor&);
	CInvoke<void(__thiscall*)(oCNpc*, oCNpc::oSDamageDescriptor&)> Ivk_oCNpc_OnDamage_Hit(ZenDef<TInstance>(0x00731410, 0x007700A0, 0x0077D390, 0x00666610), &Hook_oCNpc_OnDamage_Hit, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCNpc_OnDamage_Hit(oCNpc* _this, void* vtable, oCNpc::oSDamageDescriptor& desc)
	{
		if (TGlobals::disableOnHitHook)
			return Ivk_oCNpc_OnDamage_Hit(_this, desc);

		if (desc.dwFieldsValid & oCNpc::oEDescDamageFlags::oEDamageDescFlag_OverlayActivate)
			return;

		std::shared_ptr<TDamageInfo> info;

		if (desc.pNpcAttacker)
		{
			CHitGenContainer& hitGen = TGlobals::hitGens[desc.pNpcAttacker];
			info = CDamageDealer(&hitGen).DoDamage(_this, desc);
		}
		else
		{
			CHitGenContainer hitGen;
			info = CDamageDealer(&hitGen).DoDamage(_this, desc);
		}

		if (TGlobals::pluginSettings.enablePopupDamage && info->npcAttacker && info->npcAttacker->IsSelfPlayer())
			new CPopupDamageLabel(info);
	}
}