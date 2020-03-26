namespace NAMESPACE
{
	class CKeyRemover : public zCVobCallback
	{
	private:
		bool needKey;
		const zSTRING* key;

		bool NeedKey(const zSTRING& keyInstance);
	public:

		bool TryRemoveKey(oCNpc* npc, const zSTRING& instance);
		virtual void HandleVob(zCVob* vob, void* data) override;
	};
}