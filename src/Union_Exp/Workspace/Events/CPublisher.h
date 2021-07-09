#pragma once

#include "Delegate.h"
#include <functional>
#include <vector>
#include <unordered_set>

enum TGameEvent : uint32_t
{
	NoEvent = 0u,

	Entry = 1u << 0,
	Init = 1u << 1,
	Exit = 1u << 2,
	PreLoop = 1u << 3,
	Loop = 1u << 4,
	PostLoop = 1u << 5,
	MenuLoop = 1u << 6,
	SaveBegin = 1u << 7,
	SaveEnd = 1u << 8,
	LoadBegin_NewGame = 1u << 9,
	LoadEnd_NewGame = 1u << 10,
	LoadBegin_SaveGame = 1u << 11,
	LoadEnd_SaveGame = 1u << 12,
	LoadBegin_ChangeLevel = 1u << 13,
	LoadEnd_ChangeLevel = 1u << 14,
	LoadBegin_Trigger = 1u << 15,
	LoadEnd_Trigger = 1u << 16,
	Pause = 1u << 17,
	Unpause = 1u << 18,
	DefineExternals = 1u << 19,
	Detach = 1u << 20,

	Execute = 1u << 21,
	All = (1u << 22) - 1,
	LoadBegin = LoadBegin_NewGame | LoadBegin_SaveGame | LoadBegin_ChangeLevel,
	LoadEnd = LoadEnd_NewGame | LoadEnd_SaveGame | LoadEnd_ChangeLevel
};

constexpr uint32_t maxGameEvents = 21;

#define TNUM(x) static_cast<uint32_t>(x)
#define TEVE(x) static_cast<TGameEvent>(x);

inline TGameEvent operator&(const TGameEvent& x, const TGameEvent& y) { return TEVE(TNUM(x) & TNUM(y)); }
inline TGameEvent operator~(const TGameEvent& x) { return TEVE(~TNUM(x)); }
inline TGameEvent operator|(const TGameEvent& x, const TGameEvent& y) { return TEVE(TNUM(x) | TNUM(y)); }

#undef TNUM
#undef TEVE

class CPublisher
{
private:
	std::vector<const std::function<void()>*> subs[maxGameEvents];
	std::unordered_set<const std::function<void()>*> subsHashed[maxGameEvents];

	CPublisher() = default;

public:
	static CPublisher& GetInstance();
	void Subscribe(const TGameEvent& events, const CDelegate<void()>& delegate);
	void Unsubscribe(const TGameEvent& events, const CDelegate<void()>& delegate);
	void Raise(const TGameEvent& events);
};