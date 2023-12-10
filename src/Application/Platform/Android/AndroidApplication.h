#pragma once

#include <Application/Application.h>

struct android_app;
namespace GuGu{
    class AndroidApplication : public Application
    {
    public:
        AndroidApplication();

        virtual ~AndroidApplication();

        virtual void pumpMessage() override;

        void setAndroidApp(android_app* app);

        void setSurfaceReady(bool value);

        bool getSurfaceReady();
    private:
        android_app* m_android_app;

        bool m_surface_initialize_ready;
    };
}