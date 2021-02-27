namespace NAMESPACE
{
	class CFpsCounter
	{
	public:
		class CBenchmark
		{
		private:
			CFpsCounter& fpsCounter;
			const double requiredTime;

		public:
			std::function<bool(double)> handleFrameTime;

			CBenchmark(CFpsCounter& fpsCounter, double requiredTime) :
				fpsCounter(fpsCounter),
				requiredTime(requiredTime)
			{
				fpsCounter.Add(this);
			}

			double GetRequiredTime() const
			{
				return requiredTime;
			}

			void ProcessFrameTimes() const
			{
				if (!handleFrameTime)
					return;

				for (size_t i = 0; i < fpsCounter.frameCount; i++)
					if (handleFrameTime(fpsCounter[fpsCounter.frameCount - i - 1]))
						return;
			}

			~CBenchmark()
			{
				fpsCounter.Remove(this);
			}
		};

	private:
		std::vector<CSubscription> subs;

		std::vector<const CBenchmark*> benchmarks;
		std::vector<double> frameTimes;
		std::chrono::time_point<std::chrono::steady_clock> lastFrame;
		bool lastFrameValid;
		size_t nextFrameIndex, frameCount;
		double requiredTime;
		double currentTime;

		void OnLoop()
		{
			if (!lastFrameValid)
			{
				lastFrameValid = true;
				lastFrame = std::chrono::high_resolution_clock::now();
				return;
			}

			if (frameTimes.empty())
				return;

			auto newLastFrame = std::chrono::high_resolution_clock::now();
			double frameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(newLastFrame - lastFrame).count() / 1000000000.0;
			lastFrame = newLastFrame;

			if (frameCount == frameTimes.size())
				currentTime -= frameTimes[nextFrameIndex];

			currentTime += frameTime;
			frameTimes[nextFrameIndex] = frameTime;
			nextFrameIndex = (nextFrameIndex + 1) % frameTimes.size();
			frameCount = std::min(frameCount + 1, frameTimes.size());

			if (frameCount == frameTimes.size() && currentTime < requiredTime)
				ReserveBuffer();

			cmd << endl;
		}

		void OnLoadBegin()
		{
			lastFrameValid = false;
		}

		void OnSaveBegin()
		{
			lastFrameValid = false;
		}

		void Add(const CBenchmark* benchmark)
		{
			benchmarks.push_back(benchmark);
			
			if (benchmark->GetRequiredTime() > requiredTime)
			{
				requiredTime = benchmark->GetRequiredTime();
				ReserveBuffer();
			}
		}

		void Remove(const CBenchmark* benchmark)
		{
			benchmarks.erase(std::find(benchmarks.begin(), benchmarks.end(), benchmark));

			if (benchmark->GetRequiredTime() >= requiredTime || benchmarks.empty())
			{
				requiredTime = 0;

				for (const CBenchmark* benchmark : benchmarks)
					requiredTime = std::max(requiredTime, benchmark->GetRequiredTime());

				ReserveBuffer();
			}
		}

		double operator[](size_t frameNumber) const
		{
			return frameTimes[(nextFrameIndex + frameTimes.size() - frameCount + frameNumber) % frameTimes.size()];
		}

		void ReserveBuffer()
		{
			if (requiredTime <= 0)
			{
				frameTimes = { };
				nextFrameIndex = 0;
				frameCount = 0;
				currentTime = 0;
				return;
			}

			size_t newSize = requiredTime * 150;

			if (frameCount == frameTimes.size() && currentTime < requiredTime)
				newSize = std::max(newSize, frameCount * 2);

			if (newSize <= frameTimes.size())
				return;

			std::vector<double> newFrameTimes(newSize);

			for (size_t i = 0; i < frameCount; i++)
				newFrameTimes[i] = (*this)[i];

			frameTimes = std::move(newFrameTimes);
			nextFrameIndex = frameCount % frameTimes.size();
		}

	public:
		CFpsCounter() :
			lastFrameValid(false),
			currentTime(0),
			requiredTime(0),
			nextFrameIndex(0),
			frameCount(0)
		{
			ADDSUB(Loop);
			ADDSUB(LoadBegin);
			ADDSUB(SaveBegin);
		}
	};
}