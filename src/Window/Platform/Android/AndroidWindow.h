#pragma once

#include <Window/Window.h>

struct ANativeWindow;
namespace GuGu{
    class AndroidWindow : public Window{
    public:
        virtual void ToGeneratePlatformWindow(std::shared_ptr<WindowWidget> inWindowWidget) override;

        virtual float getDpiFactor() override;

        virtual math::float2 getWindowScreenSpacePosition() override;

        void setNativeWindow(ANativeWindow* nativeWindow);

        ANativeWindow* getNativeHandle() const;

        virtual void moveWindowTo(math::float2 newPosition) override;

        virtual void reshapeWindow(math::float2 newPosition, math::float2 newSize) override;

        virtual void destroy() override;

        virtual void show() override;

        virtual void setWindowFocus() override;
    private:
        ANativeWindow* m_nativeWindow;
    };
}