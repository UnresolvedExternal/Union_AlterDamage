// Supported with union (c) 2018 Union team

#ifndef __ONPC__DAMAGE_H__VER0__
#define __ONPC__DAMAGE_H__VER0__

namespace Gothic_I_Classic {

  typedef enum oEFlagsDamageDescriptor {
    oEDamageDescFlag_Attacker        = 1,
    oEDamageDescFlag_Npc             = 2,
    oEDamageDescFlag_Inflictor       = 4,
    oEDamageDescFlag_Weapon          = 8,
    oEDamageDescFlag_VisualFX        = 16,
    oEDamageDescFlag_SpellID         = 32,
    oEDamageDescFlag_SpellLevel      = 64,
    oEDamageDescFlag_DamageType      = 128,
    oEDamageDescFlag_WeaponType      = 256,
    oEDamageDescFlag_Damage          = 512,
    oEDamageDescFlag_HitLocation     = 1024,
    oEDamageDescFlag_FlyDirection    = 2048,
    oEDamageDescFlag_OverlayActivate = 4096,
    oEDamageDescFlag_OverlayInterval = 8192,
    oEDamageDescFlag_OverlayDuration = 16384,
    oEDamageDescFlag_OverlayDamage   = 32768,
    oEDamageDescFlag_ForceDWORD      = 0xffffffff
  } oEDescDamageFlags, oEFlagsDamageDesc;

  typedef struct oSDamageDescriptor {
  public:
    unsigned long dwFieldsValid;
    zCVob* pVobAttacker;
    oCNpc* pNpcAttacker;
    zCVob* pVobHit;
    oCVisualFX* pFXHit;
    oCItem* pItemWeapon;
    unsigned long nSpellID;
    unsigned long nSpellLevel;
    unsigned long enuModeDamage;
    unsigned long enuModeWeapon;
    unsigned long aryDamage[oEDamageIndex_MAX];
    float fDamageTotal;
    float fDamageMultiplier;
    zVEC3 vecLocationHit;
    zVEC3 vecDirectionFly;
    zSTRING strVisualFX;
    float fTimeDuration;
    float fTimeInterval;
    float fDamagePerInterval;
    group {
      unsigned long bOnce          : 1;
      unsigned long bFinished      : 1;
      unsigned long bIsDead        : 1;
      unsigned long bIsUnconscious : 1;
      unsigned long lReserved      : 28;
    };
    float fAzimuth;
    float fElevation;
    float fTimeCurrent;
    float fDamageReal;
    float fDamageEffective;
    unsigned long aryDamageEffective[oEDamageIndex_MAX];
    zCVob* pVobParticleFX;
    zCParticleFX* pParticleFX;
    oCVisualFX* pVisualFX;

    oSDamageDescriptor() {}
  } oSDescDamage;

} // namespace Gothic_I_Classic

#endif // __ONPC__DAMAGE_H__VER0__