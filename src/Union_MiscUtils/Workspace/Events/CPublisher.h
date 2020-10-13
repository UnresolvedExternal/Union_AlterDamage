#pragma once

#include "Delegate.h"
#include <functional>
#include <vector>

enum class TGameEvent
{
	NoEvent = 0,
	Entry,
	Init,
	DetachDll,
	Loop,
	SaveBegin,
	SaveEnd,
	LoadBegin,
	LoadEnd,
	LoadBegin_NewGame,
	LoadEnd_NewGame,
	LoadBegin_SaveGame,
	LoadEnd_SaveGame,
	LoadBegin_ChangeLevel,
	LoadEnd_ChangeLevel,
	LoadBegin_Trigger,
	LoadEnd_Trigger,
	Pause,
	Unpause,
	Exit,
	DefineExternals
};

class CPublisher
{
private:
	bool dllDetached;
	int nesting;
	CDelegateContainer<void()> subs[(int)TGameEvent::DefineExternals];

	CPublisher();

public:
	static CPublisher& GetInstance();

	inline CDelegateContainer<void()>& Subs(const TGameEvent& event)
	{
		return subs[(int)event - 1];
	}

	inline bool IsDllDetached() const
	{
		return dllDetached;
	}

	void Raise(const TGameEvent& event);
};