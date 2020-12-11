namespace NAMESPACE
{
	class CPointPath
	{
	public:
		virtual zVEC3 GetPosition(float time) = 0;
		virtual ~CPointPath() { }
	};

	class CCompositePointPath : public CPointPath
	{
	public:
		std::vector<std::unique_ptr<CPointPath>> paths;

		CCompositePointPath() = default;
		CCompositePointPath(const CCompositePointPath& right) = delete;
		CCompositePointPath(CCompositePointPath&& right) = default;
		CCompositePointPath& operator=(const CCompositePointPath& right) = delete;
		CCompositePointPath& operator=(CCompositePointPath&& right) = default;
		
		virtual zVEC3 GetPosition(float time) override 
		{
			zVEC3 position(0.0f);
			for (const auto& path : paths)
				position += path->GetPosition(time);
			return position;
		}
	};

	class CLinearPointPath : public CPointPath
	{
		MEMPOOL_DECL(CLinearPointPath)

	public:
		zVEC3 target;
		float time;
		float duration;
		std::function<float(float)> scale;

		CLinearPointPath() :
			target(0.0f),
			time(0.0f),
			duration(0.0f)
		{

		}

		CLinearPointPath(const CLinearPointPath& right) = delete;
		CLinearPointPath& operator=(const CLinearPointPath& right) = delete;

		void SetScale(const std::function<float(float)>& func, const zVEC2& lbCorner, const zVEC2& ruCorner)
		{
			zVEC2 delta = ruCorner - lbCorner;
			if (abs(delta[0]) < 1e-6 || abs(delta[1]) < 1e-6)
				throw std::exception("Source rectangle has zero square");

			scale = [=](float x) { return (func(lbCorner[0] + x * delta[0]) - lbCorner[1]) / delta[1]; };
		}

		virtual zVEC3 GetPosition(float time) override
		{
			if (time < this->time)
				return zVEC3(0);

			time = CoerceInRange(time, 0.0f, this->time, this->time + duration);
			time -= this->time;
			float progress = time / duration;
			if (scale)
				progress = scale(progress);

			return target * progress;
		}
	};

	MEMPOOL_DEF(CLinearPointPath)
}