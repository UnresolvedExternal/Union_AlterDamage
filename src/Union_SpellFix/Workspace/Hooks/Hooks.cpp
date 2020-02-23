#include "Workspace\Events\CSubscription.h"
#include "Workspace\Utility\TScope.h"

namespace NAMESPACE
{
	void RemoveChildEffects(zCVob* vob)
	{
		CCollector<zCEffect> effectsCollector;
		ogame->GetWorld()->TraverseVobTree(effectsCollector, nullptr, ogame->GetWorld()->globalVobTree.Search(vob));
		CArray<zCEffect*>& effects = effectsCollector.vobs;
		for (size_t i = 0; i < effects.GetNum(); i++)
			if (!effects[i]->GetObjectName().HasWordI(Z"TRANSFORM"))
				effects[i]->RemoveVobFromWorld();

		oCNpc* npc = vob->CastTo<oCNpc>();
		if (!npc)
			return;

		zCArray<zCEventMessage*>& messages = npc->GetEM(false)->messageList;
		for (int i = 0; i < messages.GetNum(); i++)
		{
			oCMsgDamage* message = messages[i]->CastTo<oCMsgDamage>();
			oCVisualFX* fx = message->descDamage.pVisualFX;
			if (!message || message->subType != oCMsgDamage::EV_DAMAGE_PER_FRAME || !fx)
				continue;
			npc->OnDamage_Effects_End(message->descDamage);
		}
		npc->GetEM(false)->KillMessages();
	}

	void SwitchNpcAgro(oCNpc* from, oCNpc* to)
	{
		zCArray<zCVob*> activeVobs = ogame->GetWorld()->activeVobList;
		for (int i = 0; i < activeVobs.GetNum(); i++)
		{
			oCNpc* npc = activeVobs[i]->CastTo<oCNpc>();
			if (npc)
			{
				if (npc->enemy == from)
					npc->SetEnemy(to);
				if (npc->focus_vob == from)
					npc->SetFocusVob(to);
			}
		}
	}

	// bugfix: switch active NPCs agro after transformation
	// bugfix: remove visual effects after transformation
	int __fastcall Hook_oCSpell_CastSpecificSpell(oCSpell*, void*);
	CInvoke<int(__thiscall*)(oCSpell*)> Ivk_oCSpell_CastSpecificSpell(ZenDef<TInstance>(0x0047EC70, 0x004896B0, 0x00485340, 0x00486960), &Hook_oCSpell_CastSpecificSpell, IvkEnabled(CurrentEngine));
	int __fastcall Hook_oCSpell_CastSpecificSpell(oCSpell* _this, void* vtable)
	{
		int result = Ivk_oCSpell_CastSpecificSpell(_this);
		if (_this->spellCasterNpc && _this->saveNpc && _this->saveNpc->sleepingMode == zVOB_SLEEPING)
		{
			SwitchNpcAgro(_this->saveNpc, _this->spellCasterNpc);
			RemoveChildEffects(_this->saveNpc);
		}
		return result;
	}

	// bugfix: remove visual effects after reverting transformation
	int __fastcall Hook_oCSpell_DeleteCaster(oCSpell*, void*);
	CInvoke<int(__thiscall*)(oCSpell*)> Ivk_oCSpell_DeleteCaster(ZenDef<TInstance>(0x0047FB50, 0x0048A750, 0x00485D00, 0x00487320), &Hook_oCSpell_DeleteCaster, IvkEnabled(CurrentEngine));
	int __fastcall Hook_oCSpell_DeleteCaster(oCSpell* _this, void* vtable)
	{
		_this->spellCasterNpc->Disable();
		RemoveChildEffects(_this->spellCasterNpc);
		return Ivk_oCSpell_DeleteCaster(_this);
	}

	// bugfix: suppress activation of spell's visual for transformed caster
	void __fastcall Hook_oCSpell_Setup(oCSpell*, void*, zCVob*, zCVob*, int);
	CInvoke<void(__thiscall*)(oCSpell*, zCVob*, zCVob*, int)> Ivk_oCSpell_Setup(ZenDef<TInstance>(0x0047C5B0, 0x00486D40, 0x004832D0, 0x00484930), &Hook_oCSpell_Setup, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCSpell_Setup(oCSpell * _this, void* vtable, zCVob * caster, zCVob * a2, int a3)
	{
		if (caster && caster->sleepingMode == zTVobSleepingMode::zVOB_SLEEPING)
			return;
		Ivk_oCSpell_Setup(_this, caster, a2, a3);
	}

	// bugfix: spell collision processed for collided vob
	int __fastcall Hook_oCVisualFX_ProcessCollision(oCVisualFX*, void*, zSVisualFXColl&);
	CInvoke<int(__thiscall*)(oCVisualFX*, zSVisualFXColl&)> Ivk_oCVisualFX_ProcessCollision(ZenDef<TInstance>(0, 0, 0x00493CF0, 0x004958D0), &Hook_oCVisualFX_ProcessCollision, (CurrentEngine >= Engine_G2) ? IVK_AUTO : IVK_DISABLED);
	int __fastcall Hook_oCVisualFX_ProcessCollision(oCVisualFX* _this, void* vtable, zSVisualFXColl& a1)
	{
		auto scope = AssignTemp(_this->target, a1.foundVob);
		return Ivk_oCVisualFX_ProcessCollision(_this, a1);
	}

	// bugfix: preclude crash when use transform in water
	zCTree<zCVob>* __fastcall Hook_zCWorld_AddVob(zCWorld*, void*, zCVob*);
	CInvoke<zCTree<zCVob>* (__thiscall*)(zCWorld*, zCVob*)> Ivk_zCWorld_AddVob(ZenDef<TInstance>(0x005F6340, 0x00617320, 0x0061D080, 0x00624810), &Hook_zCWorld_AddVob, IvkEnabled(CurrentEngine));
	zCTree<zCVob>* __fastcall Hook_zCWorld_AddVob(zCWorld* _this, void* vtable, zCVob* a1)
	{
		return Ivk_zCWorld_AddVob(_this ? _this : ogame->GetWorld(), a1);
	}
}