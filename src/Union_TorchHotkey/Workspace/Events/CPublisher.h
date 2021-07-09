#pragma once

#include "Workspace\Utility\Common\CMemPool.h"
#include <functional>
#include <vector>
#include <memory>

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
	std::vector<const std::function<void()>*> subs[(int)TGameEvent::DefineExternals];

	inline std::vector<const std::function<void()>*>& Subs(const TGameEvent& event)
	{
		return subs[(int)event - 1];
	}

	CPublisher();

public:
	static CPublisher& GetInstance();
	void Subscribe(const TGameEvent& event, const std::function<void()>* delegate);
	void Unsubscribe(const TGameEvent& event, const std::function<void()>* delegate);
	void Raise(const TGameEvent& event);
};