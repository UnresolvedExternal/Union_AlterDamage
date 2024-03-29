namespace NAMESPACE
{
	int GetControlMessageCount(zCEventManager* em)
	{
		int count = 0;

		for (zCEventMessage* message : em->messageList)
		{
			if (message->IsDeleted())
				continue;

			if (oCMsgWeapon* weaponMsg = message->CastTo<oCMsgWeapon>())
			{
				if (weaponMsg->subType == oCMsgWeapon::EV_DRAWWEAPON || weaponMsg->subType == oCMsgWeapon::EV_REMOVEWEAPON)
					count += 1;

				continue;
			}

			if (oCMsgManipulate* manipMsg = message->CastTo<oCMsgManipulate>())
			{
				if (manipMsg->subType == oCMsgManipulate::EV_USEITEMTOSTATE)
					count += 1;

				continue;
			}
		}

		return count;
	}

	enum class TMessageMode
	{
		Unknown,
		Enabled,
		Disabled
	} messageMode;

	bool inHandleEvent;

	int __fastcall Hook_oCGame_HandleEvent(oCGame*, void*, int);
	Hook<int(__thiscall*)(oCGame*, int), ActiveOption<int>> Ivk_oCGame_HandleEvent(ZENFOR(0x0065EEE0, 0x0068A300, 0x0069E980, 0x006FC170), &Hook_oCGame_HandleEvent, HookMode::Patch, Settings::MessageLimit);
	int __fastcall Hook_oCGame_HandleEvent(oCGame* _this, void* vtable, int a0)
	{
		messageMode = TMessageMode::Unknown;
		auto scope = AssignTemp(inHandleEvent, true);
		return Ivk_oCGame_HandleEvent(_this, a0);
	}

	void __fastcall Hook_zCEventManager_OnMessage(zCEventManager*, void*, zCEventMessage*, zCVob*);
	Hook<void(__thiscall*)(zCEventManager*, zCEventMessage*, zCVob*), ActiveOption<int>> Ivk_zCEventManager_OnMessage(ZENFOR(0x006DD090, 0x00715250, 0x00726940, 0x00786380), &Hook_zCEventManager_OnMessage, HookMode::Patch, Settings::MessageLimit);
	void __fastcall Hook_zCEventManager_OnMessage(zCEventManager* _this, void* vtable, zCEventMessage* message, zCVob* sourceVob)
	{
		if (inHandleEvent && messageMode == TMessageMode::Disabled)
		{
			message->Release();
			return;
		}

		if (inHandleEvent && messageMode == TMessageMode::Enabled)
			return Ivk_zCEventManager_OnMessage(_this, message, sourceVob);

		if (inHandleEvent && messageMode == TMessageMode::Unknown && _this->hostVob == player && GetControlMessageCount(_this) >= Settings::MessageLimit)
		{
			messageMode = TMessageMode::Disabled;
			message->Release();
			return;
		}

		messageMode = TMessageMode::Enabled;
		Ivk_zCEventManager_OnMessage(_this, message, sourceVob);
	}
}
