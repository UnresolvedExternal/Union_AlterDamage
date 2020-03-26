#pragma once

#include "UnionAfx.h"

#include "Macro.h"

#include "Workspace\Events\CSubscription.h"
#include "Workspace\Utility\CBinSeqGen.h"
#include "Workspace\Utility\CThreadPool.h"
#include "Workspace\Utility\CThrottledQueue.h"
#include "Workspace\Utility\Misc.h"
#include "Workspace\Utility\TScope.h"
#include "Workspace\Utility\CMemoryStream.h"
#include "Workspace\Utility\StringComparer.h"

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define CurrentEngine Engine_G1
#include "HeaderList.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "HeaderList.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "HeaderList.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "HeaderList.h"
#endif