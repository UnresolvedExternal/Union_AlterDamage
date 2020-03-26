namespace NAMESPACE
{
	template <class T>
	class CVobCollector : public zCVobCallback
	{
	public:
		CArray<T*> vobs;

		CVobCollector() {}

		CVobCollector(zCWorld* world)
		{
			world->TraverseVobTree(*this, nullptr, &world->globalVobTree);
		}

		virtual void HandleVob(zCVob* vob, void*) override
		{
			T* e = vob->CastTo<T>();
			if (e)
				vobs.InsertEnd(e);
		}
	};
}