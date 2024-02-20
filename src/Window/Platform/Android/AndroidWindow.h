#pragma once

#include <Window/Window.h>

struct ANativeWindow;
namespace GuGu{
    class AndroidWindow : public Window{
    public:
        virtual void ToGeneratePlatformWindow() override;

        void setNativeWindow(ANativeWindow* nativeWindow);

        ANativeWindow* getNativeHandle() const;
    private:
        ANativeWindow* m_nativeWindow;
    };
}