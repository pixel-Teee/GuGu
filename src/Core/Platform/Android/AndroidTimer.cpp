#include <pch.h>

#include "AndroidTimer.h"

namespace GuGu{
    AndroidTimer::~AndroidTimer() {

    }
    float AndroidTimer::GetTotalTime() const {
        //struct timeval time;
        //gettimeofday(&time, NULL);

        return (mCurrTime.tv_sec - mBaseTime.tv_sec) + (mCurrTime.tv_usec - mBaseTime.tv_usec) / 1000000.0;
    }
    float AndroidTimer::GetDeltaTime() const {
        return mDeltaTime;
    }
    void AndroidTimer::Tick() {
        struct timeval time;
        gettimeofday(&time, NULL);

        mCurrTime = time;

        mDeltaTime = (mCurrTime.tv_sec - mPrevTime.tv_sec) + (mCurrTime.tv_usec - mPrevTime.tv_usec) / 1000000.0;

        mPrevTime = mCurrTime;
    }
    AndroidTimer::AndroidTimer() {
        struct timeval time;
        gettimeofday(&time, NULL);

        mBaseTime = time;
        mPrevTime = time;
        mCurrTime = time;
    }
    std::shared_ptr<Timer> CreateTimerFactory()
    {
        return std::make_shared<AndroidTimer>();
    }
}