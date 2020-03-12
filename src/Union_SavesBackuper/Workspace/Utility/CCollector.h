namespace NAMESPACE
{
	template <class T>
	class CCollector : public zCVobCallback
	{
	public:
		CArray<T*> vobs;

		virtual void HandleVob(zCVob* vob, void*) override
		{
			T* e = vob->CastTo<T>();
			if (e)
				vobs.InsertEnd(e);
		}
	};
}