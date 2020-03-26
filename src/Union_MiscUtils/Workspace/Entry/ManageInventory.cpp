namespace NAMESPACE
{
	CSubscription loadInventory(TGameEvent::LoadEnd, []()
		{
			static CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");
			invMon.Load();
		}, SubEnabled(CurrentEngine));

	CSubscription saveInventory(TGameEvent::SaveBegin, []()
		{
			static CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");
			invMon.Save();
		}, SubEnabled(CurrentEngine));
	
	void __fastcall Hook_oCNpcInventory_Close(oCNpcInventory*, void*);
	CInvoke<void(__thiscall*)(oCNpcInventory*)> Ivk_oCNpcInventory_Close(ZenDef<TInstance>(0x0066C1E0, 0x00699960, 0x006AE810, 0x0070C2F0), &Hook_oCNpcInventory_Close, IvkEnabled(CurrentEngine));
	void __fastcall Hook_oCNpcInventory_Close(oCNpcInventory* _this, void* vtable)
	{
		if (player && &player->inventory2 == _this && _this->IsOpen() && _this->GetEnableHandleEvent())
		{
			static CSGInventoryMonitor& invMon = CSGGlobal::Get<CSGInventoryMonitor>("Inventory");
			invMon.Update(*_this);
		}

		Ivk_oCNpcInventory_Close(_this);
	}
}