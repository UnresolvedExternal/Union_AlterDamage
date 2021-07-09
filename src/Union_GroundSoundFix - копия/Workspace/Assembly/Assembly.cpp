#include "UnionAfx.h"

#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

#include "Macro.h"

#include "Workspace\Events\CSubscription.h"

#include "Workspace\Utility\Common\Coalesce.h"
#include "Workspace\Utility\Common\TScope.h"
#include "Workspace\Utility\Common\StringComparer.h"
#include "Workspace\Utility\Common\Misc.h"
#include "Workspace\Utility\Common\CMemPool.h"
#include "Workspace\Utility\Common\CThreadPool.h"
#include "Workspace\Utility\Common\CMemoryStream.h"
#include "Workspace\Utility\Common\VectorOperators.h"

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
#define ZENDEF(g1, g1a, g2, g2a) g1
#include "Headers.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
#define ZENDEF(g1, g1a, g2, g2a) g1a
#include "Headers.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
#define ZENDEF(g1, g1a, g2, g2a) g2
#include "Headers.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
#define ZENDEF(g1, g1a, g2, g2a) g2a
#include "Headers.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
#define ZENDEF(g1, g1a, g2, g2a) g1
#include "Sources.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
#define ZENDEF(g1, g1a, g2, g2a) g1a
#include "Sources.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
#define ZENDEF(g1, g1a, g2, g2a) g2
#include "Sources.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
#define ZENDEF(g1, g1a, g2, g2a) g2a
#include "Sources.h"
#undef ZENDEF
#undef ENGINE
#undef NAMESPACE
#endif