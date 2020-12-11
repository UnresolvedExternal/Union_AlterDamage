namespace NAMESPACE
{
	std::vector<int> stack;

	bool Check(zCPar_DataStack* stack)
	{
		return parser && &parser->datastack == stack;
	}

	void __fastcall Hook_zCPar_DataStack_Clear(zCPar_DataStack*, void*);
	CInvoke<void(__thiscall*)(zCPar_DataStack*)> Ivk_zCPar_DataStack_Clear(ZenDef<TInstance>(0x006FB560, 0x007359B0, 0x00745680, 0x007A5180), &Hook_zCPar_DataStack_Clear, IvkEnabled(ENGINE));
	void __fastcall Hook_zCPar_DataStack_Clear(zCPar_DataStack* _this, void* vtable)
	{
		if (Check(_this))
			return stack.clear();

		Ivk_zCPar_DataStack_Clear(_this);
	}

	int __fastcall Hook_zCPar_DataStack_IsEmpty(zCPar_DataStack*, void*);
	CInvoke<int(__thiscall*)(zCPar_DataStack*)> Ivk_zCPar_DataStack_IsEmpty(ZenDef<TInstance>(0x006FB550, 0x007359A0, 0x00745670, 0x007A5170), &Hook_zCPar_DataStack_IsEmpty, IvkEnabled(ENGINE));
	int __fastcall Hook_zCPar_DataStack_IsEmpty(zCPar_DataStack* _this, void* vtable)
	{
		if (Check(_this))
			return stack.empty();

		return Ivk_zCPar_DataStack_IsEmpty(_this);
	}

	int __fastcall Hook_zCPar_DataStack_Pop(zCPar_DataStack*, void*);
	CInvoke<int(__thiscall*)(zCPar_DataStack*)> Ivk_zCPar_DataStack_Pop(ZenDef<TInstance>(0x006FB510, 0x00735960, 0x00745630, 0x007A5070), &Hook_zCPar_DataStack_Pop, IvkEnabled(ENGINE));
	int __fastcall Hook_zCPar_DataStack_Pop(zCPar_DataStack* _this, void* vtable)
	{
		if (Check(_this))
		{
			if (stack.empty())
				return 0;

			int result = stack.back();
			stack.pop_back();
			return result;
		}

		return Ivk_zCPar_DataStack_Pop(_this);
	}

	float __fastcall Hook_zCPar_DataStack_PopFloat(zCPar_DataStack*, void*);
	CInvoke<float(__thiscall*)(zCPar_DataStack*)> Ivk_zCPar_DataStack_PopFloat(ZenDef<TInstance>(0x006FB530, 0x00735980, 0x00745650, 0x007A5090), &Hook_zCPar_DataStack_PopFloat, IvkEnabled(ENGINE));
	float __fastcall Hook_zCPar_DataStack_PopFloat(zCPar_DataStack* _this, void* vtable)
	{
		if (Check(_this))
		{
			if (stack.empty())
				return 0;

			float result = reinterpret_cast<const float&>(stack.back());
			stack.pop_back();
			return result;
		}

		return Ivk_zCPar_DataStack_PopFloat(_this);
	}

	void __fastcall Hook_zCPar_DataStack_Push(zCPar_DataStack*, void*, int);
	CInvoke<void(__thiscall*)(zCPar_DataStack*, int)> Ivk_zCPar_DataStack_Push(ZenDef<TInstance>(0x006FB4F0, 0x00735940, 0x00745540, 0x007A4F80), &Hook_zCPar_DataStack_Push, IvkEnabled(ENGINE));
	void __fastcall Hook_zCPar_DataStack_Push(zCPar_DataStack* _this, void* vtable, int a0)
	{
		if (Check(_this))
			return stack.push_back(a0);

		Ivk_zCPar_DataStack_Push(_this, a0);
	}
}