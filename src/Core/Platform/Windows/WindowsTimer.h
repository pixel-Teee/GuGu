#include <Core/Timer.h>

namespace GuGu {
	class WindowsTimer : public Timer
	{
	public:
		WindowsTimer();

		virtual ~WindowsTimer();

		virtual float GetTotalTime() const override;

		virtual float GetDeltaTime() const override;

		virtual void Tick() override;//calculate time

	private:
		double mSecondsPerCount;
		double mDeltaTime;

		int64_t mBaseTime;
		int64_t mCurrTime;
		int64_t mPrevTime;
	};
}