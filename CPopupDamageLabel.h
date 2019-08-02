#include "CGlobalEventSubscription.h"
#include <vector>
#include <memory>

namespace NAMESPACE
{
	class CPopupDamageLabel
	{
	private:
		static std::vector<CPopupDamageLabel*> labels;
		static CGlobalEventSubscription* onLoop;
		static CGlobalEventSubscription* onLoadBegin;
		static CGlobalEventSubscription* onExit;
		static const float timeLimitInSecs;
		static const float dotAddLimitInSecs;

		std::shared_ptr<TDamageInfo> info;
		zVEC3 anchorPos;
		float creationTimeInSecs;
		float maxOffsetY;
		bool dotted;
		float currentLifetimeLimit;

		static bool WorldToView(const zVEC3& worldPos, zCView* view, float& x, float& y);

		static void OnLoop();
		static void OnLoadBegin();
		static void OnExit();

		float CalcStartOfset();
		void UpdateMaxOffsetY();
		void Update(float totalTimeInSecs);
		void Print(zCView* view, const zVEC3& pos, const zVEC2& scaling, const zCOLOR& color);

	public:
		CPopupDamageLabel(std::shared_ptr<TDamageInfo> info);
		~CPopupDamageLabel();
	};
}