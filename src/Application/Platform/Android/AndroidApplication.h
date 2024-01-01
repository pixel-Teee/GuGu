#pragma once

#include <Application/Application.h>

struct android_app;
struct AAssetManager;
namespace GuGu{

    class AndroidWindow;
    class AndroidApplication : public Application
    {
    public:
        AndroidApplication();

        void setAndroidNativeWindow(android_app* pApp);

        virtual ~AndroidApplication();

        virtual void pumpMessage() override;

        virtual void init() override;

        void setAndroidApp(android_app* app);

        void setSurfaceReady(bool value);

        bool getSurfaceReady();

        void setAssetManager(AAssetManager*);

        AAssetManager* getAssetManager();

        static std::shared_ptr<AndroidApplication> getApplication();

        std::shared_ptr<AndroidWindow> getPlatformWindow();
    private:
        android_app* m_android_app;

        bool m_surface_initialize_ready;

        std::shared_ptr<AndroidWindow> m_window;

        AAssetManager* m_assetManager;
    };
}