#pragma once

#include <Application/Application.h>

struct android_app;
struct AAssetManager;
namespace GuGu{
    class AndroidWindow;
    class WindowWidget;
    class AndroidApplication : public Application
    {
    public:
        AndroidApplication();

        virtual ~AndroidApplication();

        virtual void init(std::shared_ptr<WindowWidget> inWindowWidget) override;

        virtual void pumpMessage() override;
        //virtual void resize(int32_t width, int32_t height) override;
        virtual std::shared_ptr<Window> getWindow(uint32_t index) override;

        static std::shared_ptr<AndroidApplication> getApplication();

        std::shared_ptr<AndroidWindow> getPlatformWindow();

        void setAndroidApp(android_app* app);

        void setSurfaceReady(bool value);

        bool getSurfaceReady() const;

        void setNeedToRecreateSwapChain(bool value);

        bool getNeedToRecreateSwapChainFlag() const;

        void setAssetManager(AAssetManager*);

        AAssetManager* getAssetManager() const;

        void setAndroidNativeWindow(android_app* pApp);

        virtual void makeWindow(std::shared_ptr<WindowWidget> inWindowWidget) override;
    private:
        android_app* m_android_app;

        AAssetManager* m_assetManager;

        bool m_surfaceInitializeReady;

        bool m_needToRecreateSwapChain;

        std::shared_ptr<AndroidWindow> m_window;
    };
}