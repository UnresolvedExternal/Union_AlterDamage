#pragma once

#include "UnionAfx.h"

#define Engine_G1 1
#define Engine_G1A 2
#define Engine_G2 3
#define Engine_G2A 4

#define SubEnabled(version) (Union.GetEngineVersion() == version)

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define CurrentEngine Engine_G1
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#endif