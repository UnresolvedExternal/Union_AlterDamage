namespace NAMESPACE
{
	CInvoke<zSTRING*(__cdecl*)(zSTRING*, int)> Ivk_oCAniCtrl_Human_GetWeaponString(ZENFOR(0x00626100, 0x0064AF70, 0x006523E0, 0x006AEC60), nullptr, IVK_DISABLED);

	int __fastcall Hook_oCNpc_EV_RemoveWeapon(oCNpc*, void*, oCMsgWeapon*);
	Hook<int(__thiscall*)(oCNpc*, oCMsgWeapon*), ActiveOption<bool>> Ivk_oCNpc_EV_RemoveWeapon(ZENFOR(0x006A93C0, 0x006DC1B0, 0x006EEDB0, 0x0074DB20), &Hook_oCNpc_EV_RemoveWeapon, HookMode::Patch, Settings::WaterWeaponFix);
	int __fastcall Hook_oCNpc_EV_RemoveWeapon(oCNpc* _this, void* vtable, oCMsgWeapon* msg)
	{
		int result = Ivk_oCNpc_EV_RemoveWeapon(_this, msg);

		if (!result && !msg->initDone)
		{
			zCModel* model = _this->GetModel();
			if (!model)
				return result;

			oCAniCtrl_Human* anictrl = _this->GetAnictrl();
			if (!anictrl || anictrl->walkmode != ANI_WALKMODE_WATER)
				return result;

			int wmode = _this->GetWeaponMode();
			zSTRING wStr;
			Ivk_oCAniCtrl_Human_GetWeaponString(&wStr, wmode);
			wStr = "T_" + wStr + "MOVE_2_MOVE";
			msg->ani = model->GetAniIDFromAniName(wStr);

			if (msg->ani != -1)
			{
				model->StartAni(msg->ani, 0);
				msg->duringRun = true;
				msg->initDone = true;
				return false;
			}

			return true;
		}

		return result;
	}
}
