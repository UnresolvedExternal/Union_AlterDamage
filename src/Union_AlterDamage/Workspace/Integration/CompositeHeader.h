#pragma once

#include "UnionAfx.h"

#define Engine_G1 1
#define Engine_G1A 2
#define Engine_G2 3
#define Engine_G2A 4

#define SubEnabled(version) ((Union.GetEngineVersion() == version) ? true : false)

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define CurrentEngine Engine_G1
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\Damage\CHitGenContainer.h"
#include "Workspace\Damage\TDamageInfo.h"
#include "Workspace\Damage\CDotDamage.h"
#include "Workspace\Damage\CDamageDealer.h"
#include "Workspace\Damage\CPopupDamageLabel.h"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\Damage\CHitGenContainer.h"
#include "Workspace\Damage\TDamageInfo.h"
#include "Workspace\Damage\CDotDamage.h"
#include "Workspace\Damage\CDamageDealer.h"
#include "Workspace\Damage\CPopupDamageLabel.h"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\Damage\CHitGenContainer.h"
#include "Workspace\Damage\TDamageInfo.h"
#include "Workspace\Damage\CDotDamage.h"
#include "Workspace\Damage\CDamageDealer.h"
#include "Workspace\Damage\CPopupDamageLabel.h"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "Workspace\Logging\Serializing.h"
#include "Workspace\Logging\CLogger.h"
#include "CPluginSettings.h"
#include "TGlobals.h"
#include "Workspace\Damage\CHitGenContainer.h"
#include "Workspace\Damage\TDamageInfo.h"
#include "Workspace\Damage\CDotDamage.h"
#include "Workspace\Damage\CDamageDealer.h"
#include "Workspace\Damage\CPopupDamageLabel.h"
#endif