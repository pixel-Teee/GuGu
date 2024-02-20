#include <pch.h>

#include "WindowsTimer.h"

#include <windows.h>

namespace GuGu {
	WindowsTimer::WindowsTimer()
	{
		int64_t countsPerSec = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		mSecondsPerCount = 1.0 / (double)countsPerSec;
		int64_t currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mBaseTime = currTime;
		mPrevTime = currTime;
		mCurrTime = currTime;
	}
	WindowsTimer::~WindowsTimer()
	{
	}
	float WindowsTimer::GetTotalTime() const
	{
		return (mCurrTime - mBaseTime) * mSecondsPerCount;
	}
	float WindowsTimer::GetDeltaTime() const
	{
		return mDeltaTime;
	}
	void WindowsTimer::Tick()
	{
		int64_t currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime = currTime;

		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		mPrevTime = mCurrTime;
	}
	std::shared_ptr<Timer> CreateTimerFactory()
	{
		std::shared_ptr<Timer> timer = std::make_shared<WindowsTimer>();
		return timer;
	}
}