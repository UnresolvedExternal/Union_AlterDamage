namespace NAMESPACE
{
	class CVobTraverser
	{
	private:
		class CVobCallback : public zCVobCallback
		{
		private:
			const std::function<void(zCVob*)>& handle;

		public:
			CVobCallback(const std::function<void(zCVob*)>& handle) :
				handle(handle)
			{

			}

			virtual void HandleVob(zCVob* vob, void* data) override
			{
				handle(vob);
			}
		};

	public:
		std::function<void(zCVob*)> handle;
		
		void TraverseVobTree(oCWorld* world = nullptr)
		{
			world = world ? world : ogame->GetGameWorld();
			CVobCallback callback(handle);
			world->TraverseVobTree(callback, nullptr, nullptr);
		}

		void TraverseVobList(oCWorld* world = nullptr)
		{
			world = world ? world : ogame->GetGameWorld();
			for (zCVob* vob : world->voblist)
				if (vob)
					handle(vob);
		}

		void TraverseItemList(oCWorld* world = nullptr)
		{
			world = world ? world : ogame->GetGameWorld();
			for (zCVob* vob : world->voblist_items)
				if (vob)
					handle(vob);
		}

		void TraverseNpcList(oCWorld* world = nullptr)
		{
			world = world ? world : ogame->GetGameWorld();
			for (zCVob* vob : world->voblist_npcs)
				if (vob)
					handle(vob);
		}
	};
}