namespace NAMESPACE
{
	zCArchiver* CreateArchiverWrite(zSTRING path);
	
	template <class T>
	void CollectVobs(oCWorld* world, Array<T*>& vobs)
	{
		for (zCVob* vob : world->voblist)
		{
			T* e = vob->CastTo<T>();
			if (e)
				vobs.InsertEnd(e);
		}
	}

	template <>
	void CollectVobs(oCWorld* world, Array<oCItem*>& vobs)
	{
		for (oCItem* item : world->voblist_items)
			vobs.InsertEnd(item);
	}

	template <>
	void CollectVobs(oCWorld* world, Array<oCNpc*>& vobs)
	{
		for (oCNpc* npc : world->voblist_npcs)
			vobs.InsertEnd(npc);
	}

	template <class T>
	void CollectVobs(Array<T*>& vobs)
	{
		CollectVobs(ogame->GetGameWorld(), vobs);
	}

	void Print(zCView* view, const zVEC3& worldPos, const zSTRING& text);
	bool IsLogicalPressed(unsigned short logicalId);
}