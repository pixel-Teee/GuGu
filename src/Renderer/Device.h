#pragma once

namespace GuGu{
    class Device{
    public:
        virtual ~Device();

        virtual void onDestroy() = 0;
    };
}