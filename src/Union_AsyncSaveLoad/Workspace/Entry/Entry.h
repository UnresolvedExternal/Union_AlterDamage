namespace NAMESPACE
{
	CInvoke<void(__cdecl*)()> Ivk__sysEvent(ZenDef<TInstance>(0x004F6AC0, 0x00509530, 0x005026F0, 0x005053E0), nullptr, IVK_DISABLED);

	std::atomic<bool> isAsync = false;
	std::atomic<oCBinkPlayer*> binkPlayer = nullptr;
	std::atomic<int> playResult = 0;

	template <class TTask>
	auto Execute(TTask task)
	{
		auto future = task.get_future();

		if (isAsync)
		{
			task();
			return future.get();
		}

		isAsync = true;
		std::thread thread(std::move(task));

		while (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
		{
			Ivk__sysEvent();

			if (binkPlayer != nullptr)
			{
				playResult = (*binkPlayer).PlayHandleEvents();
				binkPlayer = nullptr;
			}
		}

		thread.join();
		isAsync = false;
		return future.get();
	}

	void __fastcall Hook_oCGame_WriteSavegame(oCGame*, void*, int, int);
	ModulePatchCallInvoker<void(__thiscall*)(oCGame*, int, int)> Ivk_oCGame_WriteSavegame(CHECK_THIS_ENGINE ? ZenDef<TInstance>(0x0063AD80, 0x00661680, 0x006685D0, 0x006C5250) : (TInstance)0x00000000, &Hook_oCGame_WriteSavegame);
	void __fastcall Hook_oCGame_WriteSavegame(oCGame* _this, void* vtable, int a0, int a1)
	{
		return Execute(std::packaged_task<void()>([&]() { return Ivk_oCGame_WriteSavegame(_this, a0, a1); }));
	}

	void __fastcall Hook_oCGame_LoadSavegame(oCGame*, void*, int, int);
	ModulePatchCallInvoker<void(__thiscall*)(oCGame*, int, int)> Ivk_oCGame_LoadSavegame(CHECK_THIS_ENGINE ? ZenDef<TInstance>(0x0063C2A0, 0x00662D60, 0x00669BA0, 0x006C67D0) : (TInstance)0x00000000, &Hook_oCGame_LoadSavegame);
	void __fastcall Hook_oCGame_LoadSavegame(oCGame* _this, void* vtable, int a0, int a1)
	{
		return Execute(std::packaged_task<void()>([&]() { return Ivk_oCGame_LoadSavegame(_this, a0, a1); }));
	}

	void __fastcall Hook_oCGame_LoadGame(oCGame*, void*, int, zSTRING const&);
	ModulePatchCallInvoker<void(__thiscall*)(oCGame*, int, zSTRING const&)> Ivk_oCGame_LoadGame(CHECK_THIS_ENGINE ? ZenDef<TInstance>(0x0063C070, 0x00662B20, 0x00669970, 0x006C65A0) : (TInstance)0x00000000, &Hook_oCGame_LoadGame);
	void __fastcall Hook_oCGame_LoadGame(oCGame* _this, void* vtable, int a0, zSTRING const& a1)
	{
		return Execute(std::packaged_task<void()>([&]() { return Ivk_oCGame_LoadGame(_this, a0, a1); }));
	}

	int __fastcall Hook_oCBinkPlayer_PlayHandleEvents(oCBinkPlayer*, void*);
	ModulePatchCallInvoker<int(__thiscall*)(oCBinkPlayer*)> Ivk_oCBinkPlayer_PlayHandleEvents(CHECK_THIS_ENGINE ? ZenDef<TInstance>(0x004223D0, 0x00424960, 0x00422970, 0x00422CA0) : (TInstance)0x00000000, &Hook_oCBinkPlayer_PlayHandleEvents);
	int __fastcall Hook_oCBinkPlayer_PlayHandleEvents(oCBinkPlayer* _this, void* vtable)
	{
		if (binkPlayer)
			return Ivk_oCBinkPlayer_PlayHandleEvents(_this);

		if (!isAsync)
		{
			Ivk__sysEvent();
			return Ivk_oCBinkPlayer_PlayHandleEvents(_this);
		}

		binkPlayer = _this;

		while (true)
			if (!binkPlayer)
				return playResult;
	}
}