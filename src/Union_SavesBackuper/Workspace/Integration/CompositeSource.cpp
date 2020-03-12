#include "CompositeHeader.h"

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define CurrentEngine Engine_G1
#include "Addresses_G1.h"
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\BackupManager\CBackupManager.cpp"
#include "Workspace\Hooks\Hooks.cpp"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "Addresses_G1A.h"
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\BackupManager\CBackupManager.cpp"
#include "Workspace\Hooks\Hooks.cpp"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "Addresses_G2.h"
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\BackupManager\CBackupManager.cpp"
#include "Workspace\Hooks\Hooks.cpp"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "Addresses_G2A.h"
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\BackupManager\CBackupManager.cpp"
#include "Workspace\Hooks\Hooks.cpp"
#endif