#pragma once

namespace GuGu{
    class Adapter{
    public:
        virtual ~Adapter();

        virtual void onDestroy() = 0;
    };
}