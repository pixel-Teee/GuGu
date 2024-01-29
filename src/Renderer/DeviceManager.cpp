#include <pch.h>

#include "DeviceManager.h"

#if ANDROID
#include <Application/Platform/Android/AndroidApplication.h>
#include <Window/Platform/Android/AndroidWindow.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>//native window
#endif

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

        AnimateRenderPresent();
    }

    void DeviceManager::AnimateRenderPresent() {


        ++m_FrameIndex;
    }

    void DeviceManager::Render() {
        //todo:add begin frame
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