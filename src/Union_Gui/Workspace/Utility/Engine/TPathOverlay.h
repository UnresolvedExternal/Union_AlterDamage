namespace NAMESPACE
{
	struct TPathOverlay
	{
		zVEC3 offset;
		float startTime;
		float duration;
		std::function<float(float)> scaleProgress;

		TPathOverlay() :
			startTime(0.0f),
			duration(1.0f)
		{

		}

		TPathOverlay(const TPathOverlay& right) :
			offset(right.offset),
			startTime(right.startTime),
			duration(right.duration),
			scaleProgress(right.scaleProgress)
		{

		}

		TPathOverlay& operator=(const TPathOverlay& right)
		{
			offset = right.offset;
			duration = right.duration;
			scaleProgress = right.scaleProgress;
			return *this;
		}

		TPathOverlay(const zVEC3& offset, float startTime, float duration) :
			offset(offset),
			startTime(startTime),
			duration(duration)
		{

		}

		zVEC3 GetOffset(float time) const
		{
			if (duration < 1e-6)
				return offset;

			time = CoerceInRange(time, 0.0f, startTime, startTime + duration) - startTime;
			float progress = time / duration;
			if (scaleProgress)
				progress = scaleProgress(time / duration);

			return offset * progress;
		}

		void SetScale(const std::function<float(float)>& base, float x1, float x2, float y1, float y2)
		{
			const float dx = x2 - x1;
			const float dy = y2 - y1;

			scaleProgress = [=](float x)
			{
				return (base(x1 + x * dx) - y1) / dy;
			};
		}
	};
}