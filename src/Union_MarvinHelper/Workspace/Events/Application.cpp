#include "UnionAfx.h"
#include "CPublisher.h"

void Game_Entry()
{
	CPublisher::GetInstance().Raise(TGameEvent::Entry);
}

void Game_Init()
{
	CPublisher::GetInstance().Raise(TGameEvent::Init);
}

void Game_Exit()
{
	CPublisher::GetInstance().Raise(TGameEvent::Exit);
}

void Game_PreLoop()
{
	CPublisher::GetInstance().Raise(TGameEvent::PreLoop);
}

void Game_Loop()
{
	CPublisher::GetInstance().Raise(TGameEvent::Loop);
}

void Game_PostLoop()
{
	CPublisher::GetInstance().Raise(TGameEvent::PostLoop);
}
void Game_MenuLoop()
{
	CPublisher::GetInstance().Raise(TGameEvent::MenuLoop);
}


void Game_SaveBegin()
{
	CPublisher::GetInstance().Raise(TGameEvent::SaveBegin);
}

void Game_SaveEnd()
{
	CPublisher::GetInstance().Raise(TGameEvent::SaveEnd);
}

void LoadBegin()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadBegin);
}

void LoadEnd()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadEnd);
}

void Game_LoadBegin_NewGame()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadBegin_NewGame);
	LoadBegin();
}

void Game_LoadEnd_NewGame()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadEnd_NewGame);
	LoadEnd();
}

void Game_LoadBegin_SaveGame()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadBegin_SaveGame);
	LoadBegin();
}

void Game_LoadEnd_SaveGame()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadEnd_SaveGame);
	LoadEnd();
}

void Game_LoadBegin_ChangeLevel()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadBegin_ChangeLevel);
	LoadBegin();
}

void Game_LoadEnd_ChangeLevel()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadEnd_ChangeLevel);
	LoadEnd();
}

void Game_LoadBegin_Trigger()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadBegin_Trigger);
}

void Game_LoadEnd_Trigger()
{
	CPublisher::GetInstance().Raise(TGameEvent::LoadEnd_Trigger);
}

void Game_Pause()
{
	CPublisher::GetInstance().Raise(TGameEvent::Pause);
}

void Game_Unpause()
{
	CPublisher::GetInstance().Raise(TGameEvent::Unpause);
}

void Game_DefineExternals()
{
	CPublisher::GetInstance().Raise(TGameEvent::DefineExternals);
}

#define AppDefault True
CApplication* lpApplication = CApplication::CreateRefApplication
(
	Enabled(AppDefault) Game_Entry,
	Enabled(AppDefault) Game_Init,
	Enabled(AppDefault) Game_Exit,
	Enabled(AppDefault) Game_PreLoop,
	Enabled(AppDefault) Game_Loop,
	Enabled(AppDefault) Game_PostLoop,
	Enabled(AppDefault) Game_MenuLoop,
	Enabled(AppDefault) Game_SaveBegin,
	Enabled(AppDefault) Game_SaveEnd,
	Enabled(AppDefault) Game_LoadBegin_NewGame,
	Enabled(AppDefault) Game_LoadEnd_NewGame,
	Enabled(AppDefault) Game_LoadBegin_SaveGame,
	Enabled(AppDefault) Game_LoadEnd_SaveGame,
	Enabled(AppDefault) Game_LoadBegin_ChangeLevel,
	Enabled(AppDefault) Game_LoadEnd_ChangeLevel,
	Enabled(AppDefault) Game_LoadBegin_Trigger,
	Enabled(AppDefault) Game_LoadEnd_Trigger,
	Enabled(AppDefault) Game_Pause,
	Enabled(AppDefault) Game_Unpause,
	Enabled(AppDefault) Game_DefineExternals
);
/*
Functions call order on Game initialization:
  - Game_Entry           * Gothic entry point
  - Game_DefineExternals * Define external script functions
  - Game_Init            * After DAT files init

Functions call order on Change level:
  - Game_LoadBegin_Trigger     * Entry in trigger
  - Game_LoadEnd_Trigger       *
  - Game_Loop                  * Frame call window
  - Game_LoadBegin_ChangeLevel * Load begin
  - Game_SaveBegin             * Save previous level information
  - Game_SaveEnd               *
  - Game_LoadEnd_ChangeLevel   *

Functions call order on Save game:
  - Game_Pause     * Open menu
  - Game_Unpause   * Click on save
  - Game_Loop      * Frame call window
  - Game_SaveBegin * Save begin
  - Game_SaveEnd   *

Functions call order on Load game:
  - Game_Pause              * Open menu
  - Game_Unpause            * Click on load
  - Game_LoadBegin_SaveGame * Load begin
  - Game_LoadEnd_SaveGame   *
*/