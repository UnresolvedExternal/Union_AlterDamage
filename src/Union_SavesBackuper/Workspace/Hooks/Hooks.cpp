namespace NAMESPACE
{
	CBackupManager* bManager = nullptr;

	void __fastcall Hook_oCSavegameManager_Reinit(oCSavegameManager*, void*);
	CInvoke<void(__thiscall*)(oCSavegameManager*)> Ivk_oCSavegameManager_Reinit(__oCSavegameManager_Reinit_0x00439070, &Hook_oCSavegameManager_Reinit, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCSavegameManager_Reinit(oCSavegameManager* _this, void* vtable)
	{
		if (bManager)
		{
			auto lock = bManager->LockAll();
			return Ivk_oCSavegameManager_Reinit(_this);
		}
		Ivk_oCSavegameManager_Reinit(_this);
	}

	void __fastcall Hook_oCSavegameManager_SetAndWriteSavegame(oCSavegameManager*, void*, int, oCSavegameInfo*);
	CInvoke<void(__thiscall*)(oCSavegameManager*, int, oCSavegameInfo*)> Ivk_oCSavegameManager_SetAndWriteSavegame(__oCSavegameManager_SetAndWriteSavegame_0x004390E0, &Hook_oCSavegameManager_SetAndWriteSavegame, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCSavegameManager_SetAndWriteSavegame(oCSavegameManager* _this, void* vtable, int a1, oCSavegameInfo* a2)
	{
		if (!bManager)
			bManager = new CBackupManager(_this);

		auto lock = bManager->LockWriteSlot(a1);
		Ivk_oCSavegameManager_SetAndWriteSavegame(_this, a1, a2);
		lock.unlock();
		bManager->OnSaveWritten(a1, a2);
	}

	void __fastcall Hook_oCGame_LoadSavegame(oCGame*, void*, int, int);
	CInvoke<void(__thiscall*)(oCGame*, int, int)> Ivk_oCGame_LoadSavegame(nullptr, nullptr, IVK_DISABLED);
	void __fastcall Hook_oCGame_LoadSavegame(oCGame* _this, void* vtable, int a1, int a2)
	{
		if (bManager)
		{
			auto lock = bManager->LockReadSlot(a1);
			return Ivk_oCGame_LoadSavegame(_this, a1, a2);
		}
		Ivk_oCGame_LoadSavegame(_this, a1, a2);
	}

	template <class T>
	void AttachProtected(CInvoke<T>& ivk, const TInstance& address, const TInstance& hook)
	{
		bool wasProtected = false;
		THookInfo* info = CCallBack::FindHook((uint)(void32)address);
		if (info && info->ulProtected)
		{
			wasProtected = true;
			CCallBack::Unprotect(*info);
		}
		ivk.Attach(address, hook);
		if (wasProtected)
			ivk.Protect();
	}

	CSubscription attachProtected(TGameEvent::Entry, [&]()
		{
			AttachProtected(Ivk_oCGame_LoadSavegame, __oCGame_LoadSavegame_0x006C67D0, &Hook_oCGame_LoadSavegame);
		}, SubEnabled(CurrentEngine));

	CSubscription deleteManager(TGameEvent::Exit, []()
		{
			if (bManager)
			{
				delete bManager;
				bManager = nullptr;
			}
		}, SubEnabled(CurrentEngine));

	CSubscription loadSettings(TGameEvent::Init, []()
		{
			TGlobals::pluginSettings.Load();
		}, SubEnabled(CurrentEngine));
}