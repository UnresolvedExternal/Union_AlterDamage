#include <vector>
#include <memory>

namespace NAMESPACE
{
	class CPopupDamageLabel
	{
		MEMPOOL_DECL(CPopupDamageLabel)

	private:
		static CArray<CPopupDamageLabel*> labels;
		static const float timeLimitInSecs;
		static const float prolongationInSecs;
		static const float rightMoveTime;
		static const float rightMoveDist;
		static const float upMoveDist;

		CSubscription onLoop;
		CSubscription onLoadBegin;
		CSubscription onDetachDll;

		std::shared_ptr<TDamageInfo> info;
		zVEC3 anchorPos;
		zVEC3 offset;
		tagRECT vpPos;
		zVEC2 vpOffset;
		float creationTimeInSecs;
		float lastAnchorUpdate;
		float currentLifetimeLimit;
		bool dotted;

		bool WorldToView(const zVEC3& worldPos, zCView* view, zVEC2& pos);
		float Scale(float p, float max);
		void CalcOffset(float lifetime);
		void Update(float totalTimeInSecs);
		void CalcViewport(zSTRING& text, zCView* view, zVEC2 viewPos, const zVEC2& scaling);
		void Print(zCView* view, const zVEC3& pos, const zVEC2& scaling, const zCOLOR& color);

	public:
		CPopupDamageLabel(std::shared_ptr<TDamageInfo> info);
		~CPopupDamageLabel();
	};
}