#include <fstream>
#include <algorithm>

namespace NAMESPACE
{
	std::vector<CPopupDamageLabel*> CPopupDamageLabel::labels;
	CGlobalEventSubscription* CPopupDamageLabel::onLoop;
	CGlobalEventSubscription* CPopupDamageLabel::onLoadBegin;
	CGlobalEventSubscription* CPopupDamageLabel::onExit;
	const float CPopupDamageLabel::timeLimitInSecs = 3.0f;
	const float CPopupDamageLabel::dotAddLimitInSecs = 0.5f;

	bool CPopupDamageLabel::WorldToView(const zVEC3& worldPos, zCView* view, float& x, float& y)
	{
		zCCamera* cam = ogame->GetCamera();
		zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * worldPos;
		cam->Project(&viewPos, x, y);
		x = view->anx(x + 0.5f);
		y = view->any(y + 0.5f);
		return viewPos[2] > cam->nearClipZ;
	}

	void CPopupDamageLabel::OnLoop()
	{
		if (ogame->singleStep)
		{
			return;
		}

		for (int i = labels.size() - 1; i >= 0; i--)
		{
			labels[i]->Update(ztimer->totalTimeFloat / 1000.0f);
		}
	}

	void CPopupDamageLabel::OnLoadBegin()
	{
		labels.clear();
	}

	void CPopupDamageLabel::OnExit()
	{
		delete onLoop;
		delete onLoadBegin;
		delete onExit;

		for (size_t i = 0; i < labels.size(); i++)
		{
			delete labels[i];
		}
	}

	float CPopupDamageLabel::CalcStartOfset()
	{
		return (info->target->bbox3D.maxs[1] - info->target->bbox3D.mins[1]) * 0.55;
	}

	void CPopupDamageLabel::UpdateMaxOffsetY()
	{
		float x1, y1;
		WorldToView(anchorPos, screen, x1, y1);
		float x2, y2;
		WorldToView(anchorPos + zVEC3(0, 50, 0), screen, x2, y2);

		maxOffsetY = screen->vsizey / 4.0f / max(y1 - y2, 0.1f) * 50;
		maxOffsetY = min(maxOffsetY, 300);

		if (dotted)
		{
			maxOffsetY /= 2;
		}
	}

	void CPopupDamageLabel::Update(float currentTimeInSecs)
	{
		float lifetime = currentTimeInSecs - creationTimeInSecs;

		if (!info->target || !info->target->globalVobTreeNode)
		{
			delete this;
			return;
		}
		
		if (info->dots.size())
		{
			currentLifetimeLimit = max(currentLifetimeLimit, lifetime + dotAddLimitInSecs);
		}

		if (lifetime > currentLifetimeLimit)
		{
			delete this;
			return;
		}

		zVEC3 targetPos = info->target->GetPositionWorld();

		if (info->dots.size())
		{
			anchorPos[0] = targetPos[0];
			anchorPos[2] = targetPos[2];
		}

		anchorPos[1] = max(anchorPos[1], targetPos[1] + CalcStartOfset());

		lifetime = min(lifetime, timeLimitInSecs);
		dotted = dotted || !info->dots.empty();

		float offsetY = powf(log(1 + lifetime) / log(1 + timeLimitInSecs), 0.75f) * maxOffsetY;
		float scaling = info->isFullMeleeDamage ? 1.2f : 1.0f;
		zCOLOR color = info->isFullMeleeDamage ? zCOLOR(255, 128, 0) : zCOLOR(255, 255, 0);

		if (dotted)
		{
			color = zCOLOR(255, 128, 0);
		}

		Print(screen, anchorPos + zVEC3(0, offsetY, 0), zVEC2(scaling), color);
	}

	void CPopupDamageLabel::Print(zCView* view, const zVEC3& pos, const zVEC2& scaling, const zCOLOR& color)
	{
		float x, y;

		if (!WorldToView(pos, view, x, y))
		{
			return;
		}

		zSTRING text = info->realDamage;
		float width = view->FontSize(text) * scaling[0];
		float height = view->FontY() * scaling[1];
		x -= width / 2;
		y -= height / 2;

		x = view->nax(x);
		y = view->nay(y);
		width = view->nax(width);
		height = view->nay(height);

		zCFont* font = view->GetFont();
		float space = font->GetLetterDistance() * scaling[0];

		zrenderer->SetViewport(0, 0, 1366, 768);

		for (int i = 0; text.ToChar()[i] != '\n' && text.ToChar()[i] != '\0'; i++)
		{
			int charWidth;
			zVEC2 from, to;
			font->GetFontData(text[i], charWidth, from, to);
			float fCharWidth = charWidth * scaling[0];

#if defined(COMPILE_G1) || defined(COMPILE_G1A)
			float z = 0.9f;
#else
			float z = 11.0f;
#endif

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
		if (!onLoop)
		{
			onLoop = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::LOOP, &CPopupDamageLabel::OnLoop);
			onLoadBegin = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::LOAD_BEGIN, &CPopupDamageLabel::OnLoadBegin);
			onExit = new CGlobalEventSubscription(CGlobalEventPublisher::GlobalEvent::EXIT, &CPopupDamageLabel::OnExit);
		}

		dotted = !info->dots.empty();
		creationTimeInSecs = ztimer->totalTimeFloat / 1000.0f;
		anchorPos = info->target->GetPositionWorld();
		anchorPos[1] += CalcStartOfset();
		UpdateMaxOffsetY();
		labels.push_back(this);
	}

	CPopupDamageLabel::~CPopupDamageLabel()
	{
		auto it = std::find(labels.begin(), labels.end(), this);
		labels.erase(it);
	}
}