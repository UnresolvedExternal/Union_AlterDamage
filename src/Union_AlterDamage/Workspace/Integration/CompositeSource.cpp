#include "CompositeHeader.h"

#ifdef __G1
#define NAMESPACE Gothic_I_Classic
#define CurrentEngine Engine_G1
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\Damage\CHitGenContainer.cpp"
#include "Workspace\Damage\TDamageInfo.cpp"
#include "Workspace\Damage\CDotDamage.cpp"
#include "Workspace\Damage\CDamageDealer.cpp"
#include "Workspace\Damage\CPopupDamageLabel.cpp"
#include "Workspace\Damage\Hooks.cpp"
#endif

#ifdef __G1A
#define NAMESPACE Gothic_I_Addon
#define CurrentEngine Engine_G1A
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\Damage\CHitGenContainer.cpp"
#include "Workspace\Damage\TDamageInfo.cpp"
#include "Workspace\Damage\CDotDamage.cpp"
#include "Workspace\Damage\CDamageDealer.cpp"
#include "Workspace\Damage\CPopupDamageLabel.cpp"
#include "Workspace\Damage\Hooks.cpp"
#endif

#ifdef __G2
#define NAMESPACE Gothic_II_Classic
#define CurrentEngine Engine_G2
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\Damage\CHitGenContainer.cpp"
#include "Workspace\Damage\TDamageInfo.cpp"
#include "Workspace\Damage\CDotDamage.cpp"
#include "Workspace\Damage\CDamageDealer.cpp"
#include "Workspace\Damage\CPopupDamageLabel.cpp"
#include "Workspace\Damage\Hooks.cpp"
#endif

#ifdef __G2A
#define NAMESPACE Gothic_II_Addon
#define CurrentEngine Engine_G2A
#include "CPluginSettings.cpp"
#include "TGlobals.cpp"
#include "Workspace\Damage\CHitGenContainer.cpp"
#include "Workspace\Damage\TDamageInfo.cpp"
#include "Workspace\Damage\CDotDamage.cpp"
#include "Workspace\Damage\CDamageDealer.cpp"
#include "Workspace\Damage\CPopupDamageLabel.cpp"
#include "Workspace\Damage\Hooks.cpp"
#endif