#pragma once

#include "nvrhi.h"

#include <vector>
#include <string>
#include <Core/GuGuUtf8Str.h>
#include <list>

namespace GuGu{
    struct DefaultMessageCallback : public nvrhi::IMessageCallback
    {
        static DefaultMessageCallback& GetInstance();

        void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    struct InstanceParameters{
        bool enableDebugRuntime = false;
        bool headlessDevice = false; //无界面
#if USE_VK
        std::vector<GuGuUtf8Str> requiredVulkanInstanceExtensions;
        std::vector<GuGuUtf8Str> requiredVulkanLayers;
        std::vector<GuGuUtf8Str> optionalVulkanInstanceExtensions;
        std::vector<GuGuUtf8Str> optionalVulkanLayers;
#endif
    };
    struct DeviceCreationParameters : public InstanceParameters
    {
        uint32_t backBufferWidth = 1280;
        uint32_t backBufferHeight = 720;
        uint32_t swapChainBufferCount = 3;
        bool enableNvrhiValidationLayer = false;
        bool vsyncEnabled = false;
        bool enableComputeQueue = false;
        bool enableCopyQueue = false;
        bool enableRayTracingExtensions = false;//for vulkan
        nvrhi::Format swapChainFormat = nvrhi::Format::RGBA8_UNORM;
        uint32_t swapChainSampleCount = 1;
        //index of the adapter(dx11, dx12) or physical device(vk) on which to initialize the device
        //negative values mean automatic detection
        //the order of indices matches that returned by DeviceManager::EnumerateAdapters
        int32_t adapterIndex = -1;
#if USE_VK
        std::vector<GuGuUtf8Str> requiredVulkanDeviceExtensions;
        std::vector<GuGuUtf8Str> optionalVulkanDeviceExtensions;
        std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;
#endif
    };

    class GuGuUtf8Str;
    class IRenderPass;
    class DeviceManager{
    public:

        static DeviceManager* Create(nvrhi::GraphicsAPI api);

        bool CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, GuGuUtf8Str& windowTitle);

        bool CreateInstance(const InstanceParameters& params);

        void AddRenderPassToFront(IRenderPass* pController);
        void AddRenderPassToBack(IRenderPass* pController);
        void RemoveRenderPass(IRenderPass* pController);

        void RunMessageLoop();
    private:
        static DeviceManager* CreateVK();

    protected:
        DeviceCreationParameters m_deviceParams;
        std::list<IRenderPass*> m_vRenderPasses;
        bool m_requestedVsync = false;
        bool m_instanceCreated = false;

        uint32_t m_FrameIndex = 0;

        void Render();
        void AnimateRenderPresent();

        //device-specific methods
        virtual bool CreateInstanceInternal() = 0;
        virtual void DestroyDeviceAndSwapChain() = 0;
        virtual bool CreateDevice() = 0;
        virtual bool CreateSwapChain() = 0;
    public:
        [[nodiscard]] virtual nvrhi::IDevice *GetDevice() const = 0;
        [[nodiscard]] uint32_t GetFrameIndex() const { return m_FrameIndex; }

        virtual void ShutDown();
        virtual ~DeviceManager() = default;
    };

    class IRenderPass
    {
    private:
        DeviceManager* m_DeviceManager;

    public:
        explicit IRenderPass(DeviceManager* deviceManager) : m_DeviceManager(deviceManager){}
        virtual ~IRenderPass() = default;

        //virtual void Render(nvrhi::IFramebuffer* framebuffer) { }
        virtual void Animate(float fElapsedTimeSeconds) { }
        virtual void BackBufferResizing() { }
        virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount) { }

        [[nodiscard]] DeviceManager* GetDeviceManager() const { return m_DeviceManager; }
        [[nodiscard]] nvrhi::IDevice* GetDevice() const { return m_DeviceManager->GetDevice(); }
        [[nodiscard]] uint32_t GetFrameIndex() const { return m_DeviceManager->GetFrameIndex(); }
    };
}