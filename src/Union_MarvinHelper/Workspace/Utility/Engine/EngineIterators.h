namespace NAMESPACE
{
	// zCArray<T>

	template <class T>
	T* begin(zCArray<T>& array)
	{
		return array.GetArray();
	}

	template <class T>
	T* end(zCArray<T>& array)
	{
		return array.GetArray() + array.GetNum();
	}

	template <class T>
	const T* cbegin(const zCArray<T>& array)
	{
		return array.GetArray();
	}

	template <class T>
	const T* cend(const zCArray<T>& array)
	{
		return array.GetArray() + array.GetNum();
	}

	// zCList / zCListSort

	template <class T, class TList>
	struct TListSortIterator
	{
	private:
		TList* ele;

	public:
		typedef TListSortIterator self_type;
		typedef T* value_type;
		typedef T*& reference;
		typedef T** pointer;
		typedef std::forward_iterator_tag iterator_category;

		TListSortIterator(TList& list) :
			ele(list.next)
		{

		}

		TListSortIterator(std::nullptr_t) :
			ele(nullptr)
		{

		}

		TListSortIterator(const TListSortIterator<T, TList>& right) :
			ele(right.ele)
		{

		}

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

		reference operator*()
		{
			return ele->data;
		}

		pointer operator->()
		{
			return &ele->data;
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

	template <class T>
	TListSortIterator<T, zCList<T>> begin(zCList<T>& list)
	{
		return TListSortIterator<T, zCList<T>>(list);
	}

	template <class T>
	TListSortIterator<T, zCList<T>> end(zCList<T>& list)
	{
		return TListSortIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	TListSortIterator<T, zCList<T>> begin(zCList<T>* list)
	{
		if (list)
			return TListSortIterator<T, zCList<T>>(*list);
		return TListSortIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	TListSortIterator<T, zCList<T>> end(zCList<T>* list)
	{
		return TListSortIterator<T, zCList<T>>(nullptr);
	}

	template <class T>
	TListSortIterator<T, zCListSort<T>> begin(zCListSort<T>& list)
	{
		return TListSortIterator<T, zCListSort<T>>(list);
	}

	template <class T>
	TListSortIterator<T, zCListSort<T>> end(zCListSort<T>& list)
	{
		return TListSortIterator<T, zCListSort<T>>(nullptr);
	}

	template <class T>
	TListSortIterator<T, zCListSort<T>> begin(zCListSort<T>* list)
	{
		if (list)
			return TListSortIterator<T, zCListSort<T>>(*list);
		return TListSortIterator<T, zCListSort<T>>(nullptr);
	}

	template <class T>
	TListSortIterator<T, zCListSort<T>> end(zCListSort<T>* list)
	{
		return TListSortIterator<T, zCListSort<T>>(nullptr);
	}

	// oCNpcInventory

	struct TInventoryIterator
	{
	private:
		zCListSort<oCItem>* ele;

#if ENGINE <= Engine_G1A
		oCNpcInventory* inv;
		int invNr;
#endif

	public:
		typedef TInventoryIterator self_type;
		typedef oCItem value_type;
		typedef oCItem& reference;
		typedef oCItem* pointer;
		typedef std::forward_iterator_tag iterator_category;

		TInventoryIterator(oCNpcInventory* inv) :
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

		TInventoryIterator(const TInventoryIterator& right) :
			ele(right.ele)
#if ENGINE <= Engine_G1A
			, invNr(right.invNr)
			, inv(right.inv)
#endif
		{

		}

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

	TInventoryIterator begin(oCNpcInventory& inv)
	{
		return TInventoryIterator(&inv);
	}

	TInventoryIterator end(oCNpcInventory& inv)
	{
		return TInventoryIterator(nullptr);
	}

	template <class T>
	struct TTreeIterator
	{
	private:
		zCTree<T>* node;

	public:
		typedef TTreeIterator self_type;
		typedef zCVob* value_type;
		typedef zCVob*& reference;
		typedef zCVob** pointer;
		typedef std::input_iterator_tag iterator_category;

		TTreeIterator(zCTree<T>* tree) :
			node(tree)
		{

		}

		TTreeIterator(const TTreeIterator&) = default;
		TTreeIterator(TTreeIterator&&) = default;
		TTreeIterator& operator=(const TTreeIterator&) = default;
		TTreeIterator& operator=(TTreeIterator&&) = default;

		self_type& operator++()
		{
			if (node->GetFirstChild())
			{
				node = node->GetFirstChild();
				return *this;
			}

			if (node->GetNextChild())
			{
				node = node->GetNextChild();
				return *this;
			}

			while (node = node->GetParent())
				if (node->GetNextChild())
				{
					node = node->GetNextChild();
					return *this;
				}

			return *this;
		}

		self_type operator++(int junk)
		{
			self_type backup = *this;
			++*this;
			return backup;
		}

		value_type operator*()
		{
			return node->GetData();
		}

		bool operator==(const self_type& right)
		{
			return node == right.node;
		}

		bool operator!=(const self_type& right)
		{
			return node != right.node;
		}

		zCTree<T>* GetNode()
		{
			return node;
		}
	};

	template <class T>
	TTreeIterator<T> begin(zCTree<T>& root)
	{
		return TTreeIterator<T>(&root);
	}

	template <class T>
	TTreeIterator<T> end(zCTree<T>& root)
	{
		return TTreeIterator<T>(nullptr);
	}

	template <class T>
	TTreeIterator<T> begin(zCTree<T>* root)
	{
		return TTreeIterator<T>(root);
	}

	template <class T>
	TTreeIterator<T> end(zCTree<T>* root)
	{
		return TTreeIterator<T>(nullptr);
	}
}