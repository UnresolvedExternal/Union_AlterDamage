// Supported with union (c) 2018 Union team

#ifndef __ONPC__FIGHT_H__VER2__
#define __ONPC__FIGHT_H__VER2__

namespace Gothic_II_Classic {
  const int FA_MAX_SITUATIONS = 19;
  const int FA_MAX_ENTRY      = 6;

  enum oEBloodMode {
    oEBloodMode_None,
    oEBloodMode_Particles,
    oEBloodMode_Decals,
    oEBloodMode_Trails,
    oEBloodMode_Amplification
  };

  typedef enum oEFightAction {
    NPC_FIGHTMOVE_NOTINITIALISED,
    NPC_FIGHTMOVE_RUN,
    NPC_FIGHTMOVE_RUNBACK,
    NPC_FIGHTMOVE_JUMPBACK,
    NPC_FIGHTMOVE_TURN,
    NPC_FIGHTMOVE_STRAFE,
    NPC_FIGHTMOVE_ATTACK,
    NPC_FIGHTMOVE_SIDEATTACK,
    NPC_FIGHTMOVE_FRONTATTACK,
    NPC_FIGHTMOVE_TRIPLEATTACK,
    NPC_FIGHTMOVE_WHIRLATTACK,
    NPC_FIGHTMOVE_MASTERATTACK,
    NPC_FIGHTMOVE_PREHIT,
    NPC_FIGHTMOVE_COMBOZONE,
    NPC_FIGHTMOVE_POSTHIT,
    NPC_FIGHTMOVE_TURNTOHIT,
    NPC_FIGHTMOVE_STORMPREHIT,
    NPC_FIGHTMOVE_PARADE,
    NPC_FIGHTMOVE_STANDUP,
    NPC_FIGHTMOVE_WAIT,
    NPC_FIGHTMOVE_ONGROUND,
    NPC_FIGHTMOVE_STUMBLE,
    NPC_FIGHTMOVE_SKIP,
    NPC_FIGHTMOVE_WAIT_LONG,
    NPC_FIGHTMOVE_WAIT_EXT,
    NPC_FIGHTMOVE_MAX
  } TFAction;

  typedef struct oSActionBlock {
  public:
    int ownaction[FA_MAX_ENTRY];
    int max;

    oSActionBlock() {}
  } oSActionBlock;

  typedef struct oSFightAI {
  public:
    oSActionBlock myactions[FA_MAX_SITUATIONS];

    oSFightAI() {}
  } oCNpc::oSFightAI;

} // namespace Gothic_II_Classic

#endif // __ONPC__FIGHT_H__VER2__