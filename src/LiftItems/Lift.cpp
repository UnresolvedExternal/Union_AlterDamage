#include <algorithm>

namespace NAMESPACE
{
	zVEC3 Lift::GetFocus(zCVob* vob)
	{
		return vob->bbox3D.GetCenter() + (vob->bbox3D.GetCenter() - vob->bbox3D.GetCenterFloor()) * 0.5f;
	}

	bool Lift::TryMoveUp(zCVob* vob)
	{
		if (vob->groundPoly ||
			ogame->GetWorld()->TraceRayFirstHit(GetFocus(vob), zVEC3(0, -500, 0), vob,
				zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
		{
			return false;
		}

		if (!ogame->GetWorld()->TraceRayNearestHit(vob->GetPositionWorld(), zVEC3(0, 200, 0), vob,
			zTRACERAY_POLY_2SIDED | zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
		{
			return false;
		}

		zVEC3 shift = ogame->GetWorld()->traceRayReport.foundIntersection - vob->GetPositionWorld();
		vob->Move(shift[0], shift[1], shift[2]);
		return true;
	}

	bool Lift::TryMoveDown(zCVob* vob)
	{
		if (!ogame->GetWorld()->TraceRayNearestHit(vob->bbox3D.GetCenterFloor() + (vob->bbox3D.maxs[VY] - vob->bbox3D.mins[VY]), zVEC3(0, -200, 0), vob,
			zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
		{
			return false;
		}

		const zVEC3& floorPos = vob->bbox3D.GetCenterFloor();
		const zVEC3& newFloorPos = ogame->GetWorld()->traceRayReport.foundIntersection;

		if (newFloorPos[VY] > floorPos[VY] - 10.0f)
			return false;

		vob->trafoObjToWorld.SetTranslation(newFloorPos + zVEC3(0, (vob->bbox3D.maxs[VY] - vob->bbox3D.mins[VY]) / 2.0f, 0));
		return true;
	}

	bool Lift::TryLiftVob(zCVob* vob)
	{
		return TryMoveUp(vob) || TryMoveDown(vob);
	}

	void Lift::HandleVob(zCVob * vob, void* wtf)
	{
		if (vob->GetVobType() != zTVobType::zVOB_TYPE_ITEM)
		{
			return;
		}

		itemsTraversed += 1;

		zVEC3 oldPos = vob->GetPositionWorld();

		if (TryLiftVob(vob))
		{
			oldPositions.push_back(std::make_pair(vob, oldPos));
		}
	}

	void Lift::ToggleLifts()
	{
		for (auto it = oldPositions.begin(); it != oldPositions.end(); it++)
		{
			zVEC3 pos = it->first->GetPositionWorld();
			zVEC3 shift = it->second - pos;
			it->first->Move(shift[0], shift[1], shift[2]);
			it->second = pos;
		}
	}

	void Lift::Debug()
	{
		screen->PrintCX(1000, string::Combine("Items checked: %i", itemsTraversed).GetVector());
		screen->PrintCX(1200, string::Combine("Items affected: %i", oldPositions.size()).GetVector());

		for (auto it = oldPositions.begin(); it != oldPositions.end(); it++)
		{
			zlineCache->Line3D(GetFocus(player), GetFocus(it->first), zCOLOR(255, 0, 0), true);
		}

		if (!player->GetFocusVob())
		{
			return;
		}
	}

	void Lift::Clear()
	{
		oldPositions.clear();
		nextIndex = 0;
	}

	void Lift::Remove(zCVob* vob)
	{
		auto it = std::find_if(oldPositions.begin(), oldPositions.end(), [vob](std::pair<zCVob*, zVEC3> pair) { return pair.first == vob; });

		if (it == oldPositions.end())
		{
			return;
		}

		int index = it - oldPositions.begin();

		if (nextIndex > index)
		{
			nextIndex -= 1;
		}

		oldPositions.erase(it);
	}

	std::pair<zCVob*, zVEC3> Lift::GetNext()
	{
		if (oldPositions.empty())
		{
			return std::make_pair(player, player->GetPositionWorld());
		}

		auto result = oldPositions[nextIndex];
		nextIndex = (nextIndex + 1) % oldPositions.size();
		return result;
	}
}