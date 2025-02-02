#include "pch.h"

#include "AndroidApplication.h"
#include "AndroidMisc.h"

#include <Window/Platform/Android/AndroidWindow.h>
#include <Renderer/Platform/Vulkan/VulkanRenderer.h>

#include <Core/UI/WindowWidget.h>

namespace GuGu{
    std::shared_ptr<AndroidApplication> globalApplication;
    AndroidApplication::AndroidApplication()
    {
        m_surfaceInitializeReady = false;
        m_needToRecreateSwapChain = false;
    }

    AndroidApplication::~AndroidApplication() {

    }

    void AndroidApplication::init(std::shared_ptr<WindowWidget> inWindowWidget) {
        Application::init();
        //create renderer and init
        m_renderer = std::make_shared<VulkanRenderer>();
        m_renderer->init(inWindowWidget);
    }

    void AndroidApplication::pumpMessage() {
        processAndroidEvents(m_android_app);
    }

    std::shared_ptr<Window> AndroidApplication::getWindow(uint32_t index)
    {
        return m_window;
    }

    std::shared_ptr<AndroidApplication> AndroidApplication::getApplication() {
        return globalApplication;
    }

    std::shared_ptr <AndroidWindow> AndroidApplication::getPlatformWindow() {
        return m_window;
    }

    void AndroidApplication::setAndroidApp(android_app* app) {
        m_android_app = app;
    }

    void AndroidApplication::setSurfaceReady(bool value) {
        m_surfaceInitializeReady = value;
    }

    bool AndroidApplication::getSurfaceReady() const{
        return m_surfaceInitializeReady;
    }

    void AndroidApplication::setNeedToRecreateSwapChain(bool value) {
        m_needToRecreateSwapChain = value;
    }

    bool AndroidApplication::getNeedToRecreateSwapChainFlag() const {
        return m_needToRecreateSwapChain;
    }

    std::shared_ptr<Application> Application::getApplication()
    {
        return globalApplication;
    }

    void AndroidApplication::setAssetManager(AAssetManager* assetManager) {
        m_assetManager = assetManager;
    }

    AAssetManager *AndroidApplication::getAssetManager() const {
        return m_assetManager;
    }

    void AndroidApplication::setAndroidNativeWindow(android_app *pApp) {
        m_window = std::make_shared<AndroidWindow>();//note:just one window
        m_window->setNativeWindow(pApp->window);
        if(!m_windowWidgets.empty())
        {
            m_windowWidgets[0]->assocateWithNativeWindow(m_window);
        }
    }

    void AndroidApplication::makeWindow(std::shared_ptr<WindowWidget> inWindowWidget) {
        if(m_windowWidgets.empty())//android只能创建一个窗口
        {
            inWindowWidget->assocateWithNativeWindow(m_window);
            m_windowWidgets.push_back(inWindowWidget);
        }
    }

    GuGuUtf8Str Application::GetDirectoryWithExecutable() {
        return GuGuUtf8Str();//nothing to do
    }

	GuGuUtf8Str Application::GetExecutableFilePath() {
        return GuGuUtf8Str();//nothing to do
	}

    GuGuUtf8Str Application::GetContentDirectoryWithExecutable() {
        return "content";
    }

    //void AndroidApplication::resize(int32_t width, int32_t height) {
    //    m_renderer->onResize(width, height);
    //}

    std::shared_ptr<Application> CreateApplicationFactory()
    {
        globalApplication = std::make_shared<AndroidApplication>();
        return globalApplication;
    }
}