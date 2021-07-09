#include "UnionAfx.h"
#include "Publisher.h"

void Game_Entry()
{
	Publisher::GetInstance().Raise(GameEvent::Entry);
}

void Game_Init()
{
	Publisher::GetInstance().Raise(GameEvent::Init);
}

void Game_Exit()
{
	Publisher::GetInstance().Raise(GameEvent::Exit);
}

void Game_PreLoop()
{
	Publisher::GetInstance().Raise(GameEvent::PreLoop);
}

void Game_Loop()
{
	Publisher::GetInstance().Raise(GameEvent::Loop);
}

void Game_PostLoop()
{
	Publisher::GetInstance().Raise(GameEvent::PostLoop);
}

void Game_MenuLoop()
{
	Publisher::GetInstance().Raise(GameEvent::MenuLoop);
}

void Game_SaveBegin()
{
	Publisher::GetInstance().Raise(GameEvent::SaveBegin);
}

void Game_SaveEnd()
{
	Publisher::GetInstance().Raise(GameEvent::SaveEnd);
}

void Game_LoadBegin_NewGame()
{
	Publisher::GetInstance().Raise(GameEvent::LoadBegin_NewGame);
}

void Game_LoadEnd_NewGame()
{
	Publisher::GetInstance().Raise(GameEvent::LoadEnd_NewGame);
}

void Game_LoadBegin_SaveGame()
{
	Publisher::GetInstance().Raise(GameEvent::LoadBegin_SaveGame);
}

void Game_LoadEnd_SaveGame()
{
	Publisher::GetInstance().Raise(GameEvent::LoadEnd_SaveGame);
}

void Game_LoadBegin_ChangeLevel()
{
	Publisher::GetInstance().Raise(GameEvent::LoadBegin_ChangeLevel);
}

void Game_LoadEnd_ChangeLevel()
{
	Publisher::GetInstance().Raise(GameEvent::LoadEnd_ChangeLevel);
}

void Game_LoadBegin_Trigger()
{
	Publisher::GetInstance().Raise(GameEvent::LoadBegin_Trigger);
}

void Game_LoadEnd_Trigger()
{
	Publisher::GetInstance().Raise(GameEvent::LoadEnd_Trigger);
}

void Game_Pause()
{
	Publisher::GetInstance().Raise(GameEvent::Pause);
}

void Game_Unpause()
{
	Publisher::GetInstance().Raise(GameEvent::Unpause);
}

void Game_DefineExternals()
{
	Publisher::GetInstance().Raise(GameEvent::DefineExternals);
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