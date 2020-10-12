namespace NAMESPACE
{
	bool IsInEditItem(int index, void* adr)
	{
		if (!adr)
			return false;

		zCParser* par = zCParser::GetParser();
		if (!par)
			return false;

		zCPar_Symbol* sym = par->GetSymbol(index);
		if (!sym || (sym->type != zPAR_TYPE_INSTANCE))
			return false;

		sym = par->GetSymbol(par->GetBaseClass(index));
		if (!sym || !sym->name.Compare("C_ITEM"))
			return false;

		return true;
	}

	int __fastcall Hook_zCConsole_EditInstance(zCConsole*, void*, int, void*);
	COptionInvoke<int(__thiscall*)(zCConsole*, int, void*), bool> Ivk_zCConsole_EditInstance(ZenDef<TInstance>(0x006DC3C0, 0x007144A0, 0x00725C60, 0x007856A0), &Hook_zCConsole_EditInstance, IvkEnabled(ENGINE), Settings::ConsoleEditItemAmount);
	int __fastcall Hook_zCConsole_EditInstance(zCConsole* _this, void* vtable, int a0, void* a1)
	{
		if (IsInEditItem(a0, a1))
			_this->InsertVar("", "AMOUNT", zCON_TYPE_INT, (byte*)a1 + OffsetOf(&oCItem::amount), 1);
		return Ivk_zCConsole_EditInstance(_this, a0, a1);
	}
}