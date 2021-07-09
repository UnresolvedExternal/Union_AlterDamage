namespace NAMESPACE
{
	static bool CompareStrings(const char* x, const char* y, int len)
	{
		while (len)
		{
			if (*x != *y)
				return false;
			x += 1;
			y += 1;
			len -= 1;
		}
		return true;
	}

	static int ParseInt(const char* x, int len)
	{
		int num = 0;
		while (len--)
		{
			num = num * 10 + (*x - '0');
			x += 1;
		}
		return num;
	}

	struct TPackedItem
	{
		int startIndex;
		int colonIndex;
		int endIndex;

		inline int GetAmount(const char* pStr) const
		{
			if (colonIndex == -1)
				return 1;
			return ParseInt(pStr + colonIndex + 1, endIndex - colonIndex);
		}

		inline bool GetIsEquipped(const char* pStr) const
		{
			return colonIndex != -1 && pStr[endIndex] == 'E';
		}

		inline bool CheckName(const char* pStr, const char* name, int nLen) const
		{
			int ownLen = (colonIndex == -1) ? GetLen() : (colonIndex - startIndex);
			if (ownLen != nLen)
				return false;
			return CompareStrings(pStr + startIndex, name, nLen);
		}

		inline int GetLen() const
		{
			return endIndex - startIndex + 1;
		}
	};

	void Parse(const char* pStr, int pLen, Array<TPackedItem> & elems)
	{
		TPackedItem elem;
		elem.startIndex = 0;
		elem.colonIndex = -1;

		for (int i = 0; i <= pLen; i++)
		{
			elem.endIndex = i - 1;

			if ((i == pLen || pStr[i] == ',') && elem.GetLen() > 0)
			{
				elems.InsertEnd(elem);
				elem.startIndex = i + 1;
				elem.colonIndex = -1;
			}
			else if (i != pLen && pStr[i] == ':')
				elem.colonIndex = i;
		}
	}

	int __fastcall Hook_oCNpcInventory_GetPackedItemInfo(oCNpcInventory*, void*, zSTRING const&, int, int&, int&);
	CInvoke<int(__thiscall*)(oCNpcInventory*, zSTRING const&, int, int&, int&)> Ivk_oCNpcInventory_GetPackedItemInfo(ZenDef<TInstance>(0x0066FDF0, 0x0069D560, 0x006B1D60, 0x0070F8F0), &Hook_oCNpcInventory_GetPackedItemInfo, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpcInventory_GetPackedItemInfo(oCNpcInventory * _this, void* vtable, zSTRING const& name, int remove, int& amount, int& equipped)
	{
		if (!_this->packAbility)
			return 0;

		amount = 0;
		equipped = false;

#if ENGINE <= Engine_G1A
		for (int invNr = 0; invNr < INV_MAX; invNr++)
		{
			zSTRING& packString = _this->packString[invNr];
#else
		zSTRING& packString = _this->packString;
#endif

		const int pLen = packString.Length();
		if (!pLen)
#if ENGINE <= Engine_G1A
			continue;
#else
			return 0;
#endif

		const char* pStr = packString.ToChar();
		const char* nStr = name.ToChar();
		const int nLen = name.Length();

		Array<TPackedItem> elems;
		Parse(pStr, pLen, elems);

		Array<bool> remFlags;
		for (int i = 0; i < (int)elems.GetNum(); i++)
		{
			const TPackedItem& e = elems[i];
			bool nameMatched = e.CheckName(pStr, nStr, nLen);
			if (nameMatched)
				amount += MAX(0, e.GetAmount(pStr));
			if (remove)
				remFlags.InsertEnd(nameMatched);
		}

		if (remFlags.GetNum())
		{
			std::unique_ptr<char[]> newStr = std::make_unique<char[]>(pLen + 1);
			int newStrLen = 0;
			for (int i = 0; i < (int)elems.GetNum(); i++)
			{
				const TPackedItem& e = elems[i];
				if (remFlags[i])
					continue;
				if (newStrLen)
					newStr[newStrLen++] = ',';
				CopyMemory(newStr.get() + newStrLen, pStr + e.startIndex, e.GetLen());
				newStrLen += e.GetLen();
			}
			newStr[newStrLen++] = '\0';
			packString = newStr.get();
		}

#if ENGINE <= Engine_G1A
		}
#endif

		return amount ? 1 : 0;
	}

	int __fastcall Hook_oCNpcInventory_GetAmount(oCNpcInventory*, void*, int);
	CInvoke<int(__thiscall*)(oCNpcInventory*, int)> Ivk_oCNpcInventory_GetAmount(ZenDef<TInstance>(0x0066CAC0, 0x0069A240, 0x006AEE80, 0x0070C970), &Hook_oCNpcInventory_GetAmount, IvkEnabled(ENGINE));
	int __fastcall Hook_oCNpcInventory_GetAmount(oCNpcInventory * _this, void* vtable, int instance)
	{
		if (instance < 0)
			return 0;
		int amount = 0;

#if ENGINE <= Engine_G1A
		for (int invNr = 0; invNr < INV_MAX; invNr++)
		{
			const zCListSort<oCItem>& inventory = _this->inventory[invNr];
#else
			const zCListSort<oCItem>& inventory = _this->inventory;
#endif

			for (zCListSort<oCItem>* list = inventory.GetNextInList(); list; list = list->GetNextInList())
				if (list->GetData()->GetInstance() == instance)
					amount += list->GetData()->amount;
#if ENGINE <= Engine_G1A
		}
#endif

		if (!_this->packAbility)
			return amount;

		zCPar_Symbol* symbol = parser->GetSymbol(instance);
		if (!symbol)
			return amount;

		int packedAmount, equipped;
		if (_this->GetPackedItemInfo(symbol->name, true, packedAmount, equipped))
			amount += packedAmount;

		while (packedAmount)
		{
			oCItem* item = static_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, instance));
			item->amount = item->MultiSlot() ? packedAmount : 1;
			_this->Insert(item);
			item->Release();
			packedAmount -= item->amount;
		}

		return amount;
	}
}