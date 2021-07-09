#include "UnionAfx.h"
#include "Macro.h"

#include <regex>
#include <future>

#include "Workspace\Events\CSubscription.h"

#include "Workspace\Utility\Common\Macro.h"
#include "Workspace\Utility\Common\TScope.h"
#include "Workspace\Utility\Common\StringComparer.h"
#include "Workspace\Utility\Common\Misc.h"
#include "Workspace\Utility\Common\CMemPool.h"
#include "Workspace\Utility\Common\CThreadPool.h"
#include "Workspace\Utility\Common\CMemoryStream.h"

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
#include "Headers.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
#include "Headers.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
#include "Headers.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
#include "Headers.h"
#endif

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
#include "Sources.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
#include "Sources.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
#include "Sources.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
#include "Sources.h"
#endif