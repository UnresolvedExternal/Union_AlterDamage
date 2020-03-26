#include "Workspace\Events\CPublisher.h"

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

void Game_Loop()
{
	CPublisher::GetInstance().Raise(TGameEvent::Loop);
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

#define EnableEventDefault true
CApplication* lpApplication = CApplication::CreateRefApplication
(
	Enabled(EnableEventDefault) Game_Entry,
	Enabled(EnableEventDefault) Game_Init,
	Enabled(EnableEventDefault) Game_Exit,
	Enabled(EnableEventDefault) Game_Loop,
	Enabled(EnableEventDefault) Game_SaveBegin,
	Enabled(EnableEventDefault) Game_SaveEnd,
	Enabled(EnableEventDefault) Game_LoadBegin_NewGame,
	Enabled(EnableEventDefault) Game_LoadEnd_NewGame,
	Enabled(EnableEventDefault) Game_LoadBegin_SaveGame,
	Enabled(EnableEventDefault) Game_LoadEnd_SaveGame,
	Enabled(EnableEventDefault) Game_LoadBegin_ChangeLevel,
	Enabled(EnableEventDefault) Game_LoadEnd_ChangeLevel,
	Enabled(EnableEventDefault) Game_LoadBegin_Trigger,
	Enabled(EnableEventDefault) Game_LoadEnd_Trigger,
	Enabled(EnableEventDefault) Game_Pause,
	Enabled(EnableEventDefault) Game_Unpause,
	Enabled(EnableEventDefault) Game_DefineExternals
);