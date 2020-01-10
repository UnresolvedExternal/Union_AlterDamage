#include "Workspace\Memory\CMemPool.h"
#include <functional>

struct TInvGlobals
{
	static CMemPool<std::function<void()>> delegatesPool;
};