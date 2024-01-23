#include "pch.h"

#include "AndroidApplication.h"

#include "AndroidMisc.h"
#include <Window/Platform/Android/AndroidWindow.h>
#include <Renderer/Platform/Vulkan/VulkanRenderer.h>

#include <Renderer/nvrhi.h>//todo:remove this
#include <Renderer/DeviceManager.h>//todo:remove this

namespace GuGu{
    std::shared_ptr<AndroidApplication> globalApplication;
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

    std::shared_ptr<AndroidApplication> AndroidApplication::getApplication() {
        return globalApplication;
    }

    void AndroidApplication::init() {
        //create renderer and init
        m_renderer = std::make_shared<VulkanRenderer>();
        m_renderer->init();

    }

    std::shared_ptr <AndroidWindow> AndroidApplication::getPlatformWindow() {
        return m_window;
    }

    void AndroidApplication::setAssetManager(AAssetManager* assetManager) {
        m_assetManager = assetManager;
    }

    AAssetManager *AndroidApplication::getAssetManager() {
        return m_assetManager;
    }


    std::shared_ptr<Application> CreateApplicationFactory()
    {
        globalApplication = std::make_shared<AndroidApplication>();
        return globalApplication;
    }
}