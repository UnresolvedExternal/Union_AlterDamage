#include "UnionAfx.h"
#include <vector>

namespace NAMESPACE
{
	class Lift : public zCVobCallback
	{
	private:
		std::vector<std::pair<zCVob*, zVEC3>> oldPositions;
		int nextIndex;
		int itemsTraversed;
		
		zVEC3 GetFocus(zCVob* vob);
		bool TryMoveUp(zCVob* vob);
		bool TryMoveDown(zCVob* vob);
		bool TryLiftVob(zCVob* vob);

	public:
		virtual void HandleVob(zCVob* vob, void* wtf) override;
		void ToggleLifts();
		void Debug();
		void Clear();
		void Remove(zCVob* vob);
		std::pair<zCVob*, zVEC3> GetNext();
	};
}