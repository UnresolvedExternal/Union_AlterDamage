#include <fstream>
#include <algorithm>
#include "Workspace\Utility\Misc.h"

namespace NAMESPACE
{
	MEMPOOL_DEF(CPopupDamageLabel)

	CArray<CPopupDamageLabel*> CPopupDamageLabel::labels;
	const float CPopupDamageLabel::timeLimitInSecs = 3.0f;
	const float CPopupDamageLabel::prolongationInSecs = 1.5f;
	const float CPopupDamageLabel::rightMoveTime = 0.3f;
	const float CPopupDamageLabel::rightMoveDist = 75.0f;
	const float CPopupDamageLabel::upMoveDist = 100.0f;

	bool CPopupDamageLabel::WorldToView(const zVEC3& worldPos, zCView* view, zVEC2& pos)
	{
		zCCamera* cam = ogame->GetCamera();
		zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * worldPos;
		
		int x, y;
		cam->Project(&viewPos, x, y);
		pos[0] = view->anx(x + 0.5f);
		pos[1] = view->any(y + 0.5f);

		return viewPos[2] > cam->nearClipZ;
	}
	
	float CPopupDamageLabel::Scale(float p, float max)
	{
		return sqrt(p / max);
	}

	void CPopupDamageLabel::CalcOffset(float lifetime)
	{
		const zMAT4& viewTrafo = ogame->GetCamera()->GetTransform(zTCamTrafoType::zCAM_TRAFO_WORLDVIEW_INV);
		offset = zVEC3(viewTrafo[0][0], viewTrafo[1][0], viewTrafo[2][0]) * Scale(min(lifetime, rightMoveTime), rightMoveTime) * rightMoveDist;
		offset[1] += Scale(min(lifetime, timeLimitInSecs), timeLimitInSecs) * upMoveDist;
	}

	void CPopupDamageLabel::Update(float currentTimeInSecs)
	{
		float lifetime = currentTimeInSecs - creationTimeInSecs;

		if (!info->target || !info->target->globalVobTreeNode)
		{
			delete this;
			return;
		}

		if (lastAnchorUpdate < info->lastUpdate)
		{
			lastAnchorUpdate = ztimer->totalTimeFloat / 1000.0f;
			currentLifetimeLimit = max(currentLifetimeLimit, lifetime + prolongationInSecs);
			anchorPos[0] = info->target->GetPositionWorld()[0];
			anchorPos[2] = info->target->GetPositionWorld()[2];
		}

		if (lifetime > currentLifetimeLimit)
		{
			delete this;
			return;
		}

		lifetime = min(lifetime, timeLimitInSecs);
		dotted = dotted || info->dots.GetNum();

		CalcOffset(lifetime);
		float scaling = info->IsCrit ? 1.4f : 1.1f;

		if (lifetime < 0.3f)
			scaling *= Scale(lifetime, 0.3f) * 1.5f;
		else if (lifetime < 0.4f)
			scaling *= 1.5f - (1 - Scale(0.4f - lifetime, 0.4f)) * 0.5f;

		zCOLOR color = info->IsCrit ? zCOLOR(255, 128, 0) : zCOLOR(255, 255, 0);
		if (dotted)
			color = zCOLOR(255, 128, 0);

		Print(screen, anchorPos + offset, zVEC2(scaling), color);
	}

	void CPopupDamageLabel::CalcViewport(zSTRING& text, zCView* view, zVEC2 viewPos, const zVEC2& scaling)
	{
		float width = view->FontSize(text) * scaling[0];
		float height = view->FontY() * scaling[1];
		viewPos[0] = CoerceInRange(viewPos[0] - width / 2, width, 0, 8191);
		viewPos[1] = CoerceInRange(viewPos[1] - height / 2, height, 0, 8191);

		vpPos.left = view->nax(viewPos[0]) + vpOffset[0];
		vpPos.top = view->nay(viewPos[1]) + vpOffset[1];
		width = view->nax(width);
		height = view->nay(height);
		vpPos.right = vpPos.left + width;
		vpPos.bottom = vpPos.top + height;

		for (int i = 0; labels[i] != this; i++)
			if (Intersects(vpPos, labels[i]->vpPos))
			{
				int deltaY = (labels[i]->vpPos.bottom - vpPos.top) / 3.0f;
				vpPos.top += deltaY;
				vpPos.bottom += deltaY;
				vpOffset[1] += deltaY;
			}
	}

	void CPopupDamageLabel::Print(zCView* view, const zVEC3& pos, const zVEC2& scaling, const zCOLOR& color)
	{
		zVEC2 viewPos;
		if (!WorldToView(pos, view, viewPos))
			return;

		zSTRING text = info->realDamage;
		CalcViewport(text, view, viewPos, scaling);

		zCFont* font = view->GetFont();
		float space = font->GetLetterDistance() * scaling[0];

		float x = vpPos.left;
		float y = vpPos.top;
		float height = vpPos.bottom - vpPos.top;

		for (size_t i = 0; text.ToChar()[i] != '\n' && text.ToChar()[i] != '\0'; i++)
		{
			int charWidth;
			zVEC2 from, to;
			font->GetFontData(text[i], charWidth, from, to);
			float fCharWidth = charWidth * scaling[0];
			float z = ZenDef(0.9f, 0.9f, 1.1f, 1.1f);

			zrenderer->DrawTile(
				font->GetFontTexture(),
				zVEC2(x, y),
				zVEC2(x + fCharWidth, y + height),
				z,
				from,
				to,
				color
			);

			x += fCharWidth + space;
		}
	}

	CPopupDamageLabel::CPopupDamageLabel(std::shared_ptr<TDamageInfo> info) :
		info(info),
		currentLifetimeLimit(timeLimitInSecs)
	{
		labels.Insert(this);

		onLoop.Reset(TGameEvent::Loop, [&]() 
			{
				Update(ztimer->totalTimeFloat / 1000.0f);
			});

		onLoadBegin.Reset(TGameEvent::LoadBegin, [&]()
			{
				delete this;
			});

		onDetachDll.Reset(TGameEvent::DetachDll, [&]()
			{
				delete this;
			});

		dotted = info->dots.GetNum();
		creationTimeInSecs = ztimer->totalTimeFloat / 1000.0f;
		lastAnchorUpdate = creationTimeInSecs;
		anchorPos = info->target->GetPositionWorld();
		anchorPos[1] += (info->target->bbox3D.maxs[1] - info->target->bbox3D.mins[1]) * 0.6f;
		vpOffset = zVEC2(0, 0);
		CalcOffset(0);
	}

	CPopupDamageLabel::~CPopupDamageLabel()
	{
		labels.Remove(this);
	}
}