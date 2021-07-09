namespace NAMESPACE
{
	template <class T> T* begin(zCArray<T>& array) { return array.GetArray(); }
	template <class T> T* end(zCArray<T>& array) { return array.GetArray() + array.GetNum(); }
	template <class T> const T* cbegin(const zCArray<T>& array) { return array.GetArray(); }
	template <class T> const T* cend(const zCArray<T>& array) {	return array.GetArray() + array.GetNum(); }

	template <class T, class TList>
	struct ListIterator
	{
	private:
		TList* ele;

	public:
		typedef ListIterator self_type;
		typedef T* value_type;
		typedef T*& reference;
		typedef T** pointer;
		typedef std::forward_iterator_tag iterator_category;

		ListIterator(TList& list) :
			ele{ list.next }
		{

		}

		ListIterator(std::nullptr_t) :
			ele{ }
		{

		}

		ListIterator(const ListIterator<T, TList>& right) = default;
		ListIterator& operator=(const ListIterator<T, TList>& right) = default;

		self_type& operator++()
		{
			ele = ele->next;
			return *this;
		}

		self_type operator++(int junk)
		{
			self_type backup = *this;
			++*this;
			return backup;
		}

		reference operator*() {	return ele->data; }
		pointer operator->() { return &ele->data; }
		bool operator==(const self_type& right)	{ return ele == right.ele; }
		bool operator!=(const self_type& right) { return ele != right.ele; }
	};

	template <class T>
	ListIterator<T, zCList<T>> begin(zCList<T>& list)
	{
		return ListIterator<T, zCList<T>>(list);
	}

	template <class T>
	ListIterator<T, zCList<T>> end(zCList<T>& list)
	{
		return ListIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	ListIterator<T, zCList<T>> begin(zCList<T>* list)
	{
		if (list)
			return ListIterator<T, zCList<T>>(*list);
		return ListIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	ListIterator<T, zCList<T>> end(zCList<T>* list)
	{
		return ListIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	ListIterator<T, zCListSort<T>> begin(zCListSort<T>& list)
	{
		return ListIterator<T, zCListSort<T>>(list);
	}

	template <class T>
	ListIterator<T, zCListSort<T>> end(zCListSort<T>& list)
	{
		return ListIterator<T, zCListSort<T>>(nullptr);
	}

	template <class T>
	ListIterator<T, zCListSort<T>> begin(zCListSort<T>* list)
	{
		if (list)
			return ListIterator<T, zCListSort<T>>(*list);
		return ListIterator<T, zCListSort<T>>(nullptr);
	}

	template <class T>
	ListIterator<T, zCListSort<T>> end(zCListSort<T>* list)
	{
		return ListIterator<T, zCListSort<T>>(nullptr);
	}

	struct InventoryIterator
	{
	private:
		zCListSort<oCItem>* ele;

#if ENGINE <= Engine_G1A
		oCNpcInventory* inv;
		int invNr;
#endif

	public:
		typedef InventoryIterator self_type;
		typedef oCItem* value_type;
		typedef oCItem*& reference;
		typedef oCItem** pointer;
		typedef std::forward_iterator_tag iterator_category;

		InventoryIterator(oCNpcInventory* inv) :
			ele(nullptr)
#if ENGINE <= Engine_G1A
			, inv(inv)
			, invNr(0)
#endif
		{
			if (!inv)
				return;

			if (inv->packAbility)
				inv->UnpackAllItems();

#if ENGINE <= Engine_G1A
			ele = inv->inventory[invNr].GetNextInList();

			while (!ele && invNr < INV_MAX - 1)
				ele = inv->inventory[++invNr].next;
#else
			ele = inv->inventory.GetNextInList();
#endif
		}

		InventoryIterator(const InventoryIterator&) = default;
		InventoryIterator& operator=(const InventoryIterator&) = default;

		self_type& operator++()
		{
			ele = ele->next;

#if ENGINE <= Engine_G1A
			while (!ele && invNr < INV_MAX - 1)
				ele = inv->inventory[++invNr].next;
#endif

			return *this;
		}

		self_type operator++(int junk)
		{
			self_type backup = *this;
			++*this;
			return backup;
		}

		reference operator*() {	return ele->data; }
		pointer operator->() { return &ele->data; }
		bool operator==(const self_type& right)	const { return ele == right.ele; }
		bool operator!=(const self_type& right) const { return ele != right.ele; }
	};

	InventoryIterator begin(oCNpcInventory& inv)
	{
		return InventoryIterator(&inv);
	}

	InventoryIterator end(oCNpcInventory& inv)
	{
		return InventoryIterator(nullptr);
	}
}