#include <vector>
#include <functional>

namespace NAMESPACE
{
	class CInvoke_zCWorld_VobRemovedFromWorld
	{
	private:
		enum State { ATTACHED, DETACHED, MOVEDOUT };

		static std::vector<CInvoke_zCWorld_VobRemovedFromWorld*>* hooks;
		static CInvoke<void(__thiscall*)(zCWorld*, zCVob*)>* ivk;
		static int attachedCount;

		State state;
		int index;
		std::function<void(zCWorld*, zCVob*)> handler;

		static void __fastcall Hook(zCWorld* _this, void* vtable, zCVob* vob);
		static void IncreaseAttachedCount(int delta);

		void Attach();
		void Detach();

	public:
		CInvoke_zCWorld_VobRemovedFromWorld();
		CInvoke_zCWorld_VobRemovedFromWorld(const std::function<void(zCWorld*, zCVob*)>& handler);
		CInvoke_zCWorld_VobRemovedFromWorld(CInvoke_zCWorld_VobRemovedFromWorld&& source);

		void SetHandler(const std::function<void(zCWorld*, zCVob*)>& handler);
		void operator ()(zCWorld* _this, zCVob* vob);
		
		~CInvoke_zCWorld_VobRemovedFromWorld();
	};
}