#pragma once

namespace GuGu{
    class SwapChain{
    public:
        virtual ~SwapChain();

        virtual void onDestroy() = 0;
    };
}