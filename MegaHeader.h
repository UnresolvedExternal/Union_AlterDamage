#pragma once
#include "UnionAfx.h"

#ifdef __G1
#define COMPILE_G1
#include "Common.h"
#include "TDamageInfo.h"
#include "CDamageDealer.h"
#include "CPopupDamageLabel.h"
#include "CDotDamage.h"
#undef COMPILE_G1
#endif

#ifdef __G1A
#define COMPILE_G1A
#include "Common.h"
#include "TDamageInfo.h"
#include "CDamageDealer.h"
#include "CPopupDamageLabel.h"
#include "CDotDamage.h"
#undef COMPILE_G1A
#endif

#ifdef __G2
#define COMPILE_G2
#include "Common.h"
#include "TDamageInfo.h"
#include "CDamageDealer.h"
#include "CPopupDamageLabel.h"
#include "CDotDamage.h"
#undef COMPILE_G2
#endif

#ifdef __G2A
#define COMPILE_G2A
#include "Common.h"
#include "TDamageInfo.h"
#include "CDamageDealer.h"
#include "CPopupDamageLabel.h"
#include "CDotDamage.h"
#undef COMPILE_G2A
#endif