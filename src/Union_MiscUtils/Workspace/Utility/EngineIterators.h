namespace NAMESPACE
{
	template <class T>
	const T* begin(const zCArray<T>& array)
	{
		return array.GetArray();
	}

	template <class T>
	const T* end(const zCArray<T>& array)
	{
		return array.GetArray() + array.GetNum();
	}

	template <class T>
	const T* begin(const CArray<T>& array)
	{
		return &array[0];
	}

	template <class T>
	const T* end(const CArray<T>& array)
	{
		return &array[0] + array.GetNum();
	}

	class CInventoryIterator
	{
	private:
		zCListSort<oCItem>* ele;

#if CurrentEngine == Engine_G1 || CurrentEngine == Engine_G1A
		oCNpcInventory* inv;
		int invNr;
#endif

	public:
		typedef CInventoryIterator self_type;
		typedef oCItem value_type;
		typedef oCItem& reference;
		typedef oCItem* pointer;
		typedef std::forward_iterator_tag iterator_category;

		CInventoryIterator(oCNpcInventory* inv) :
			ele(nullptr)
#if CurrentEngine == Engine_G1 || CurrentEngine == Engine_G1A
			, inv(inv)
			, invNr(0)
#endif
		{
			if (!inv)
			{
				ele = nullptr;
				return;
			}

			if (inv->packAbility)
				inv->UnpackAllItems();

#if CurrentEngine == Engine_G1 || CurrentEngine == Engine_G1A
			ele = inv->inventory[invNr].GetNextInList();
			while (!ele && invNr < INV_MAX - 1)
				ele = inv->inventory[++invNr].GetNextInList();
#else
			ele = inv->inventory.GetNextInList();
#endif
		}

		CInventoryIterator(const CInventoryIterator& right) :
			ele(right.ele)
#if CurrentEngine == Engine_G1 || CurrentEngine == Engine_G1A
			, invNr(0)
			, inv(right.inv)
#endif
		{

		}

		self_type& operator++()
		{
			ele = ele->GetNextInList();

#if CurrentEngine == Engine_G1 || CurrentEngine == Engine_G1A
			while (!ele && invNr < INV_MAX - 1)
				ele = inv->inventory[++invNr].GetNextInList();
#endif

			return *this;
		}

		self_type operator++(int junk)
		{
			self_type backup = *this;
			++*this;
			return backup;
		}

		reference operator*()
		{
			return *ele->data;
		}

		pointer operator->()
		{
			return ele->data;
		}

		bool operator==(const self_type& right)
		{
			return ele == right.ele;
		}

		bool operator!=(const self_type& right)
		{
			return ele != right.ele;
		}
	};

	CInventoryIterator begin(oCNpcInventory& inv)
	{
		return CInventoryIterator(&inv);
	}

	CInventoryIterator end(oCNpcInventory& inv)
	{
		return CInventoryIterator(nullptr);
	}
}