#include "UnionAfx.h"
#include "CGlobalEventPublisher.h"
#include "MegaHeader.h"

void Game_Entry() { 
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::ENTRY);
	
	TEngineVersion engineVersion = Union.GetEngineVersion();

#ifdef __G1
	if (engineVersion == Engine_G1)
	{
		static Gothic_I_Classic::LiftAgent agent;
	}
#endif

#ifdef __G1A
	if (engineVersion == Engine_G1A)
	{
		static Gothic_I_Addon::LiftAgent agent;
	}
#endif

#ifdef __G2
	if (engineVersion == Engine_G2)
	{
		static Gothic_II_Classic::LiftAgent agent;
	}
#endif

#ifdef __G2A
	if (engineVersion == Engine_G2A)
	{
		static Gothic_II_Addon::LiftAgent agent;
	}
#endif
}

void Game_Init() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::INIT);
}

void Game_Exit() {
}

void Game_Loop() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::LOOP);
}

void Game_SaveBegin() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::SAVE_BEGIN);
}

void Game_SaveEnd() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::SAVE_END);
}

void LoadBegin() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::LOAD_BEGIN);
}

void LoadEnd() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::LOAD_END);
}

void Game_LoadBegin_NewGame() {
 LoadBegin();
}

void Game_LoadEnd_NewGame() {
 LoadEnd();
}

void Game_LoadBegin_SaveGame() {
 LoadBegin();
}

void Game_LoadEnd_SaveGame() {
 LoadEnd();
}

void Game_LoadBegin_ChangeLevel() {
 LoadBegin();
}

void Game_LoadEnd_ChangeLevel() {
 LoadEnd();
}

void Game_LoadBegin_Trigger() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::LOAD_BEGIN_TRIGGER);
}

void Game_LoadEnd_Trigger() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::LOAD_END_TRIGGER);
}

void Game_Pause() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::PAUSE);
}

void Game_Unpause() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::UNPAUSE);
}

void Game_DefineExternals() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::DEFINE_EXTERNALS);
}

CApplication* lpApplication = CApplication::CreateRefApplication (
  Game_Entry,
  Game_Init,
  Game_Exit,
  Game_Loop,
  Game_SaveBegin,
  Game_SaveEnd,
  Game_LoadBegin_NewGame,
  Game_LoadEnd_NewGame,
  Game_LoadBegin_SaveGame,
  Game_LoadEnd_SaveGame,
  Game_LoadBegin_ChangeLevel,
  Game_LoadEnd_ChangeLevel,
  Game_LoadBegin_Trigger,
  Game_LoadEnd_Trigger,
  Game_Pause,
  Game_Unpause,
  Game_DefineExternals
  );