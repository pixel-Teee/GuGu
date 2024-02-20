#pragma once

#include <Core/Timer.h>

#include <sys/time.h>

namespace GuGu{
    class AndroidTimer : public Timer
    {
    public:
        AndroidTimer();

        virtual ~AndroidTimer();

        virtual float GetTotalTime() const override;
        virtual float GetDeltaTime() const override;

        virtual void Tick() override ;//calculate time

    private:
        timeval mBaseTime;
        timeval mCurrTime;
        timeval mPrevTime;

        double mDeltaTime;
    };

}