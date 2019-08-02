#include "UnionAfx.h"
#include "CGlobalEventPublisher.h"

#ifdef __G1
namespace Gothic_I_Classic { extern void Attach(); }
#endif
#ifdef __G1A
namespace Gothic_I_Addon { extern void Attach(); }
#endif
#ifdef __G2
namespace Gothic_II_Classic { extern void Attach(); }
#endif
#ifdef __G2A
namespace Gothic_II_Addon { extern void Attach(); }
#endif

void Game_Entry() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::ENTRY);
}

void Game_Init() {
	CGlobalEventPublisher::Raise(CGlobalEventPublisher::INIT);
	
	switch (Union.GetEngineVersion())
	{
#ifdef __G1
	case TEngineVersion::Engine_G1: Gothic_I_Classic::Attach(); break;
#endif
#ifdef __G1A
	case TEngineVersion::Engine_G1A: Gothic_I_Addon::Attach(); break;
#endif
#ifdef __G2
	case TEngineVersion::Engine_G2: Gothic_II_Classic::Attach(); break;
#endif
#ifdef __G2A
	case TEngineVersion::Engine_G2A: Gothic_II_Addon::Attach(); break;
#endif
	}
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