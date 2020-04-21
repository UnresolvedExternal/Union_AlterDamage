namespace NAMESPACE
{
	template <class T>
	class CVobCollector
	{
	public:
		Array<T*> vobs;

		CVobCollector(oCWorld* world)
		{
			for (zCVob* vob : world->voblist)
			{
				T* e = vob->CastTo<T>();
				if (e)
					vobs.InsertEnd(e);
			}
		}
	};

	template <>
	class CVobCollector<oCItem>
	{
	public:
		Array<oCItem*> vobs;

		CVobCollector(oCWorld* world)
		{
			for (oCItem* vob : world->voblist_items)
				vobs.InsertEnd(vob);
		}
	};


	template <>
	class CVobCollector<oCNpc>
	{
	public:
		Array<oCNpc*> vobs;

		CVobCollector(oCWorld* world)
		{
			for (oCNpc* vob : world->voblist_npcs)
				vobs.InsertEnd(vob);
		}
	};
}