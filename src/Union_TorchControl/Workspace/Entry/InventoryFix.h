namespace NAMESPACE
{
	bool CompareStrings(const char* x, const char* y, int len)
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

	int ParseInt(const char* x, int len)
	{
		int num = 0;
		while (len--)
		{
			num = num * 10 + (*x - '0');
			x += 1;
		}
		return num;
	}

	struct PackedItem
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
			const int ownLen = (colonIndex == -1) ? GetLen() : (colonIndex - startIndex);

			if (ownLen != nLen)
				return false;

			return CompareStrings(pStr + startIndex, name, nLen);
		}

		inline std::string GetName(const char* pStr) const
		{
			const int ownLen = (colonIndex == -1) ? GetLen() : (colonIndex - startIndex);
			return std::string(pStr + startIndex, pStr + startIndex + ownLen);
		}

		inline int GetLen() const
		{
			return endIndex - startIndex + 1;
		}
	};

	std::vector<PackedItem> Parse(const char* pStr, int pLen)
	{
		std::vector<PackedItem> elems;
		elems.reserve(64u);

		PackedItem elem;
		elem.startIndex = 0;
		elem.colonIndex = -1;

		for (int i = 0; i <= pLen; i++)
		{
			elem.endIndex = i - 1;

			if ((i == pLen || pStr[i] == ',') && elem.GetLen() > 0)
			{
				elems += elem;
				elem.startIndex = i + 1;
				elem.colonIndex = -1;
			}
			else if (i != pLen && pStr[i] == ':')
				elem.colonIndex = i;
		}

		return elems;
	}

	int __fastcall Hook_oCNpcInventory_GetPackedItemInfo(oCNpcInventory*, void*, zSTRING const&, int, int&, int&);
	Hook<int(__thiscall*)(oCNpcInventory*, zSTRING const&, int, int&, int&), ActiveOption<bool>> Ivk_oCNpcInventory_GetPackedItemInfo(ZENFOR(0x0066FDF0, 0x0069D560, 0x006B1D60, 0x0070F8F0), &Hook_oCNpcInventory_GetPackedItemInfo, HookMode::Patch, Settings::PackStringFix);
	int __fastcall Hook_oCNpcInventory_GetPackedItemInfo(oCNpcInventory* _this, void* vtable, zSTRING const& name, int remove, int& amount, int& equipped)
	{
		if (!_this->packAbility)
			return 0;

		amount = 0;
		equipped = false;

		for (int invNr = 0; invNr < ZENDEF(INV_MAX, INV_MAX, 1, 1); invNr++)
		{
			zSTRING& packString = ZENDEF(_this->packString[invNr], _this->packString[invNr], _this->packString, _this->packString);

			const int pLen = packString.Length();

			if (!pLen)
				continue;

			const char* pStr = packString.ToChar();
			const char* nStr = name.ToChar();
			const int nLen = name.Length();

			std::vector<PackedItem> elems = Parse(pStr, pLen);
			std::vector<int> remFlags;

			for (size_t i = 0; i < elems.size(); i++)
			{
				const PackedItem& e = elems[i];
				bool nameMatched = e.CheckName(pStr, nStr, nLen);

				if (nameMatched)
					amount += std::max(0, e.GetAmount(pStr));

				if (remove)
					remFlags += static_cast<int>(nameMatched);
			}

			if (!remFlags.empty())
			{
				std::unique_ptr<char[]> newStr = std::make_unique<char[]>(pLen + 1);
				int newStrLen = 0;

				for (size_t i = 0; i < elems.size(); i++)
				{
					const PackedItem& e = elems[i];
			
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
		}

		return amount ? 1 : 0;
	}

	int __fastcall Hook_oCNpcInventory_GetAmount(oCNpcInventory*, void*, int);
	Hook<int(__thiscall*)(oCNpcInventory*, int), ActiveOption<bool>> Ivk_oCNpcInventory_GetAmount(ZENFOR(0x0066CAC0, 0x0069A240, 0x006AEE80, 0x0070C970), &Hook_oCNpcInventory_GetAmount, HookMode::Patch, Settings::GetAmountFix);
	int __fastcall Hook_oCNpcInventory_GetAmount(oCNpcInventory * _this, void* vtable, int instance)
	{
		if (instance < 0)
			return 0;

		int amount = 0;

		for (int invNr = 0; invNr < ZENFOR(INV_MAX, INV_MAX, 1, 1); invNr++)
		{
			zCListSort<oCItem>& inventory = ZENDEF2(_this->inventory[invNr], _this->inventory);

			for (oCItem* item : inventory)
				if (item->instanz == instance)
					amount += item->amount;
		}

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

#if ENGINE <= Engine_G1A

	// WARNING: supported versions are G1, G1A
	void __fastcall Hook_oCNpcInventory_UnpackCategory(oCNpcInventory*, void*, int);
	Hook<void(__thiscall*)(oCNpcInventory*, int), ActiveOption<bool>> Ivk_oCNpcInventory_UnpackCategory(ZENFOR(0x0066FAD0, 0x0069D240, 0x00000000, 0x00000000), &Hook_oCNpcInventory_UnpackCategory, HookMode::Patch, Settings::PackStringFix);
	void __fastcall Hook_oCNpcInventory_UnpackCategory(oCNpcInventory* _this, void* vtable, int invNr)
	{
		if (!_this->packAbility || !_this->owner)
			return;

		const char* pStr = _this->packString[invNr].ToChar();
		const int pLen = _this->packString[invNr].Length();

		std::vector<PackedItem> packedItems = Parse(pStr, pLen);
		_this->packString[invNr].Clear();

		for (const PackedItem& packedItem : packedItems)
		{
			const zSTRING name = packedItem.GetName(pStr).c_str();
			bool equiped = packedItem.GetIsEquipped(pStr);

			for (int amount = packedItem.GetAmount(pStr); amount > 0; )
			{
				ZOwner<oCItem> item{ static_cast<oCItem*>(ogame->GetGameWorld()->CreateVob_novt(zVOB_TYPE_ITEM, name)) };

				if (!item)
					break;

				item->amount = item->MultiSlot() ? amount : 1;
				amount -= item->amount;

				oCItem* inserted = _this->owner->PutInInv(item.get());

				if (equiped && inserted)
				{
					equiped = false;
					_this->owner->Equip(inserted);
				}
			}
		}
	}

#endif
}
