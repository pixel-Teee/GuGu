#include <pch.h>

#include "DeviceManager.h"

#if ANDROID
#include <Application/Platform/Android/AndroidApplication.h>
#include <Window/Platform/Android/AndroidWindow.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>//native window
#include <Renderer/Platform/Vulkan/DeviceManager_VK.h>

#else
#ifdef WIN32
#include <Application/Platform/Windows/WindowsApplication.h>
#include <Window/Platform/Windows/WindowsWindow.h>
#include <Core/Platform/Windows/WindowsGuGuFile.h>
#endif
#endif

#include <Core/Timer.h>

namespace GuGu{

    static const struct
    {
        nvrhi::Format format;
        uint32_t redBits;
        uint32_t greenBits;
        uint32_t blueBits;
        uint32_t alphaBits;
        uint32_t depthBits;
        uint32_t stencilBits;
    }formatInfo[] = {
       { nvrhi::Format::UNKNOWN,            0,  0,  0,  0,  0,  0, },
       { nvrhi::Format::R8_UINT,            8,  0,  0,  0,  0,  0, },
       { nvrhi::Format::RG8_UINT,           8,  8,  0,  0,  0,  0, },
       { nvrhi::Format::RG8_UNORM,          8,  8,  0,  0,  0,  0, },
       { nvrhi::Format::R16_UINT,          16,  0,  0,  0,  0,  0, },
       { nvrhi::Format::R16_UNORM,         16,  0,  0,  0,  0,  0, },
       { nvrhi::Format::R16_FLOAT,         16,  0,  0,  0,  0,  0, },
       { nvrhi::Format::RGBA8_UNORM,        8,  8,  8,  8,  0,  0, },
       { nvrhi::Format::RGBA8_SNORM,        8,  8,  8,  8,  0,  0, },
       { nvrhi::Format::BGRA8_UNORM,        8,  8,  8,  8,  0,  0, },
       { nvrhi::Format::SRGBA8_UNORM,       8,  8,  8,  8,  0,  0, },
       { nvrhi::Format::SBGRA8_UNORM,       8,  8,  8,  8,  0,  0, },
       { nvrhi::Format::R10G10B10A2_UNORM, 10, 10, 10,  2,  0,  0, },
       { nvrhi::Format::R11G11B10_FLOAT,   11, 11, 10,  0,  0,  0, },
       { nvrhi::Format::RG16_UINT,         16, 16,  0,  0,  0,  0, },
       { nvrhi::Format::RG16_FLOAT,        16, 16,  0,  0,  0,  0, },
       { nvrhi::Format::R32_UINT,          32,  0,  0,  0,  0,  0, },
       { nvrhi::Format::R32_FLOAT,         32,  0,  0,  0,  0,  0, },
       { nvrhi::Format::RGBA16_FLOAT,      16, 16, 16, 16,  0,  0, },
       { nvrhi::Format::RGBA16_UNORM,      16, 16, 16, 16,  0,  0, },
       { nvrhi::Format::RGBA16_SNORM,      16, 16, 16, 16,  0,  0, },
       { nvrhi::Format::RG32_UINT,         32, 32,  0,  0,  0,  0, },
       { nvrhi::Format::RG32_FLOAT,        32, 32,  0,  0,  0,  0, },
       { nvrhi::Format::RGB32_UINT,        32, 32, 32,  0,  0,  0, },
       { nvrhi::Format::RGB32_FLOAT,       32, 32, 32,  0,  0,  0, },
       { nvrhi::Format::RGBA32_UINT,       32, 32, 32, 32,  0,  0, },
       { nvrhi::Format::RGBA32_FLOAT,      32, 32, 32, 32,  0,  0, },
    };

    DeviceManager *DeviceManager::Create(nvrhi::GraphicsAPI api) {
        switch (api) {
#if USE_VK
            case nvrhi::GraphicsAPI::VULKAN:
                return CreateVK();
#endif
        }

        return nullptr;
    }

    bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters &params,
                                                       GuGuUtf8Str &windowTitle) {
        //todo:add dpi aware

        m_deviceParams = params;
        m_deviceParams.headlessDevice = false;
        m_requestedVsync = params.vsyncEnabled;

        if(!CreateInstance(m_deviceParams))
        {
            return false;
        }

#if ANDROID
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
#endif
        //vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
        //m_height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        //m_width = ANativeWindow_getWidth(androidWindow->getNativeHandle());

        bool foundFormat = false;
        for(const auto& info : formatInfo)
        {
            if(info.format == params.swapChainFormat) { //todo:fix this
#if ANDROID
                ANativeWindow_setBuffersGeometry(androidWindow->getNativeHandle(), 0, 0, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);
                foundFormat = true;
                break;
#else
#if WIN32
                foundFormat = true;
                break;
#endif
#endif
            }
        }

        assert(foundFormat);
        //todo:add swap chain sample count and refresh rate handle

        //todo:add fullscreen handle
#if ANDROID
        int32_t height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        int32_t width = ANativeWindow_getWidth(androidWindow->getNativeHandle());
        m_deviceParams.backBufferHeight = height;
        m_deviceParams.backBufferWidth = width;
#endif

        //todo:add window title check

        //todo:add message call back handle

        if(!CreateDevice())
            return false;

        if(!CreateSwapChain())
            return false;

        //todo:reset the back buffer size state to enforce a resize event
        m_deviceParams.backBufferWidth = 0;
        m_deviceParams.backBufferHeight = 0;

        UpdateWindowSize();

        return true;
    }

    bool DeviceManager::CreateInstance(const InstanceParameters &params) {
        if(m_instanceCreated)
            return true;

        static_cast<InstanceParameters&>(m_deviceParams) = params;

        m_instanceCreated = CreateInstanceInternal();
        return m_instanceCreated;
    }

    void DeviceManager::ShutDown() {
        DestroyDeviceAndSwapChain();

        m_instanceCreated = false;
    }

    void DeviceManager::AddRenderPassToFront(IRenderPass *pRenderPass) {
        m_vRenderPasses.remove(pRenderPass);
        m_vRenderPasses.push_front(pRenderPass);

        pRenderPass->BackBufferResizing();
        pRenderPass->BackBufferResized(
                m_deviceParams.backBufferWidth,
                m_deviceParams.backBufferHeight,
                m_deviceParams.swapChainSampleCount);
    }

    void DeviceManager::AddRenderPassToBack(IRenderPass *pRenderPass) {
        m_vRenderPasses.remove(pRenderPass);
        m_vRenderPasses.push_back(pRenderPass);
        pRenderPass->BackBufferResizing();
        pRenderPass->BackBufferResized(
                m_deviceParams.backBufferWidth,
                m_deviceParams.backBufferHeight,
                m_deviceParams.swapChainSampleCount
        );
    }

    void DeviceManager::RemoveRenderPass(IRenderPass *pRenderPass) {
        m_vRenderPasses.remove(pRenderPass);
    }

    void DeviceManager::RunMessageLoop() {
        //todo:record previous frame time stamp
#ifndef ANDROID
        //note:android have bug
        UpdateWindowSize();
#endif
        AnimateRenderPresent();
    }

    const DeviceCreationParameters& DeviceManager::getDeviceCreationParameters() const
    {
        return m_deviceParams;
    }

    void DeviceManager::AnimateRenderPresent() {

         std::shared_ptr<Application> application = Application::getApplication();

         Animate(application->getTimer()->GetDeltaTime());

         Render();//todo:fix this

         Present();

         GetDevice()->runGarbageCollection();
         ++m_FrameIndex;
    }

    void DeviceManager::Render() {
        //todo:add begin frame
        BeginFrame();

        nvrhi::IFramebuffer* framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];

        for(auto it : m_vRenderPasses)
        {
            it->Render(framebuffer);
        }
    }

    void DeviceManager::BackBufferResizing() {
        m_SwapChainFramebuffers.clear();

        for(auto it : m_vRenderPasses)
        {
            it->BackBufferResizing();
        }
    }

    void DeviceManager::BackBufferResized() {
        for(auto it : m_vRenderPasses)
        {
            it->BackBufferResized(m_deviceParams.backBufferWidth,
                                  m_deviceParams.backBufferHeight,
                                  m_deviceParams.swapChainSampleCount);
        }

        uint32_t backBufferCount = GetBackBufferCount();
        m_SwapChainFramebuffers.resize(backBufferCount);
        for (uint32_t index = 0; index < backBufferCount; index++)
        {
            m_SwapChainFramebuffers[index] = GetDevice()->createFramebuffer(
                    nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
        }
    }

    void DeviceManager::UpdateWindowSize() {
        int width = 0;
        int height = 0;
        //glfwGetWindowSize(m_Window, &width, &height);
        bool needToRecreateSwapChain = false;
#ifdef ANDROID
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();

        height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        width = ANativeWindow_getWidth(androidWindow->getNativeHandle());

        needToRecreateSwapChain = androidApplication->getNeedToRecreateSwapChainFlag();
#else
        std::shared_ptr<WindowsApplication> windowsApplication = WindowsApplication::getApplication();
        std::shared_ptr<WindowsWindow> windowsWindow = windowsApplication->getPlatformWindows()[0]; //todo:fix this

        RECT rect;
        if (GetClientRect(windowsWindow->getNativeWindowHandle(), &rect))
        {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }

#endif

        if (width <= 0 || height <= 0)
        {
            // window is minimized
            //m_windowVisible = false;
#ifdef ANDROID
            androidApplication->setFocused(false);
#else
#if WIN32
            windowsApplication->setFocused(false);
#endif
#endif
            return;
        }

        //m_windowVisible = true;
        //androidApplication->setFocused(true);

        if (int(m_deviceParams.backBufferWidth) != width ||
            int(m_deviceParams.backBufferHeight) != height || needToRecreateSwapChain) //todo:fix this
        {
#ifdef ANDROID
            androidApplication->setNeedToRecreateSwapChain(false);
#endif
            // window is not minimized, and the size has changed

            BackBufferResizing();


            m_deviceParams.backBufferWidth = width;
            m_deviceParams.backBufferHeight = height;

            //m_deviceParams.vsyncEnabled = m_RequestedVSync;

            ResizeSwapChain();
            BackBufferResized();
        }

       // m_deviceParams.vsyncEnabled = m_RequestedVSync;
    }

    void DeviceManager::Animate(double elapsedTime) {
        for(auto it : m_vRenderPasses)
        {
            it->Animate(float(elapsedTime));
        }
    }

    DefaultMessageCallback &DefaultMessageCallback::GetInstance() {
        static DefaultMessageCallback Instance;
        return Instance;
    }

    void DefaultMessageCallback::message(nvrhi::MessageSeverity severity, const char *messageText) {
        switch(severity)
        {
            case nvrhi::MessageSeverity::Info:
                GuGu_LOGI("%s", messageText);
                break;
            case nvrhi::MessageSeverity::Warning:
                GuGu_LOGW("%s", messageText);
                break;
            case nvrhi::MessageSeverity::Error:
                GuGu_LOGE("%s", messageText);
                break;
            //case nvrhi::MessageSeverity::Fatal:
            //    break;
        }
    }
}