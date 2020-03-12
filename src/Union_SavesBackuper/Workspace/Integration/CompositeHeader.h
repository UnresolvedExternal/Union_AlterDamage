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
#include "Addresses_G1.h"
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\BackupManager\CBackupManager.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "Addresses_G1A.h"
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\BackupManager\CBackupManager.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "Addresses_G2.h"
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\BackupManager\CBackupManager.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "Addresses_G2A.h"
#include "Workspace\Utility\CCollector.h"
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\BackupManager\CBackupManager.h"
#endif