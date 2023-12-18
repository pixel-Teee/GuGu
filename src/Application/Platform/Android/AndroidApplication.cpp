#include "pch.h"

#include "AndroidApplication.h"

#include "AndroidMisc.h"
#include <Window/Platform/Android/AndroidWindow.h>

namespace GuGu{
    AndroidApplication::AndroidApplication()
    {
        m_surface_initialize_ready = false;
    }

    AndroidApplication::~AndroidApplication() {

    }

    void AndroidApplication::setAndroidApp(android_app* app) {
        m_android_app = app;
    }

    void AndroidApplication::setSurfaceReady(bool value) {
        m_surface_initialize_ready = value;
    }

    bool AndroidApplication::getSurfaceReady() {
        return m_surface_initialize_ready;
    }

    void AndroidApplication::pumpMessage() {
        processAndroidEvents(m_android_app);
    }

    void AndroidApplication::setAndroidNativeWindow(android_app *pApp) {
        m_window = std::make_shared<AndroidWindow>();//note:just one window
        m_window->setNativeWindow(pApp->window);
    }


    std::shared_ptr<Application> CreateApplicationFactory()
    {
        return std::make_shared<AndroidApplication>();
    }
}