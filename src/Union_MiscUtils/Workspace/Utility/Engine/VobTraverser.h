namespace NAMESPACE
{
	class VobTraverser
	{
	public:
		std::function<void(zCVob*)> handle;
		
		void TraverseVobTree(zCTree<zCVob>* tree = nullptr)
		{
			if (!tree)
				tree = &ogame->world->globalVobTree;

			struct TElement
			{
				zCTree<zCVob>* tree;
				zCVob* vob;
				bool traversed;

				TElement(zCTree<zCVob>* tree) : tree{ tree }, vob{ tree->GetData() }, traversed{ } { }
			};

			std::vector<TElement> stack{ tree };
			stack.reserve(64u);

			while (!stack.empty())
			{
				if (stack.back().traversed)
				{
					if (stack.back().vob)
						handle(stack.back().vob);

					stack.pop_back();
					continue;
				}

				stack.back().traversed = true;
				const size_t size = stack.size();

				for (zCTree<zCVob>* child = stack.back().tree->GetFirstChild(); child; child = child->GetNextChild())
					stack.push_back(child);

				std::reverse(stack.begin() + size, stack.end());
			}
		}

		void TraverseVobList(zCListSort<zCVob>* list = nullptr)
		{
			list = list ? list : ogame->GetGameWorld()->voblist;
			std::for_each(begin(list), end(list), handle);
		}

		void TraverseItemList(zCListSort<oCItem>* list = nullptr)
		{
			list = list ? list : ogame->GetGameWorld()->voblist_items;
			std::for_each(begin(list), end(list), handle);
		}

		void TraverseNpcList(zCListSort<oCNpc>* list = nullptr)
		{
			list = list ? list : ogame->GetGameWorld()->voblist_npcs;
			std::for_each(begin(list), end(list), handle);
		}
	};
}