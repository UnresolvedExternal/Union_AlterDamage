#include <functional>

namespace NAMESPACE
{
	void LiftAgent::OnLoadEnd()
	{
		lift.Clear();
		ogame->GetWorld()->TraverseVobTree(lift, nullptr, &ogame->GetWorld()->globalVobTree);
	}

	void LiftAgent::OnGameLoop()
	{
		if (!ogame->game_testmode)
		{
			debugMode = TDebugMode::OFF;
			return;
		}

		timer[0u].Suspend(ogame->singleStep);

		bool debugPressed = zinput->KeyPressed(KEY_U) && timer[0u].Await(200, true);
		bool teleportPressed = zinput->KeyPressed(KEY_I) && timer[0u].Await(200, true);
		bool togglePressed = zinput->GetMouseButtonPressedRight() && timer[0u].Await(200, true);

		if (debugPressed)
		{
			debugMode = debugMode == TDebugMode::ON ? TDebugMode::OFF : TDebugMode::ON;
		}

		if (debugMode != TDebugMode::ON)
		{
			return;
		}

		if (togglePressed)
		{
			lift.ToggleLifts();
		}

		if (teleportPressed)
		{
			zCVob* vob = lift.GetNext().first;
			player->ResetPos(vob->GetPositionWorld() + zVEC3(0, 200, 0));
		}

		lift.Debug();
	}

	void LiftAgent::OnRemove(zCWorld* world, zCVob* vob)
	{
		lift.Remove(vob);
		removeIvk(world, vob);
	}

	LiftAgent::LiftAgent():
		debugMode(TDebugMode::OFF)
	{
		CGlobalEventSubscription loadSub(CGlobalEventPublisher::GlobalEvent::LOAD_END, std::bind(&LiftAgent::OnLoadEnd, this));
		CGlobalEventSubscription loopSub(CGlobalEventPublisher::GlobalEvent::LOOP, std::bind(&LiftAgent::OnGameLoop, this));
		subs.push_back(std::move(loadSub));
		subs.push_back(std::move(loopSub));

		removeIvk.SetHandler(std::bind(&LiftAgent::OnRemove, this, std::placeholders::_1, std::placeholders::_2));
	}
}