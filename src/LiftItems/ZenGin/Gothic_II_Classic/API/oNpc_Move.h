// Supported with union (c) 2018 Union team

#ifndef __ONPC__MOVE_H__VER2__
#define __ONPC__MOVE_H__VER2__

namespace Gothic_II_Classic {

  struct oSDirectionInfo {
    zVEC3 checkDirection;
    unsigned char checkForChasm       : 1;
    unsigned char checkForSmallObject : 1;
    unsigned char checkIfClimbable    : 1;

    oSDirectionInfo() {}
  };

  struct oTRobustTrace {
    group {
      unsigned char stand                : 1;
      unsigned char dirChoosed           : 1;
      unsigned char exactPosition        : 1;
      unsigned char targetReached        : 1;
      unsigned char standIfTargetReached : 1;
      unsigned char waiting              : 1;
      unsigned char isObstVobSmall       : 1;
      unsigned char targetVisible        : 1;
      unsigned char useChasmChecks       : 1;
    };
    zVEC3 targetPos;
    zCVob* targetVob;
    zCVob* obstVob;
    float targetDist;
    float lastTargetDist;
    float maxTargetDist;
    float dirTurn;
    float timer;
    zVEC3 dirFirst;
    float dirLastAngle;
    zCArray<oSDirectionInfo*> lastDirections;
    int frameCtr;
    zVEC3 targetPosArray[5];
    int targetPosCounter;
    int targetPosIndex;
    float checkVisibilityTime;
    float positionUpdateTime;
    float failurePossibility;

    oTRobustTrace() {}
  };

} // namespace Gothic_II_Classic

#endif // __ONPC__MOVE_H__VER2__