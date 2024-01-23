#pragma once

#include "nvrhi.h"

#include <vector>
#include <string>
#include <Core/GuGuUtf8Str.h>

namespace GuGu{

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
    class DeviceManager{
    public:
        static DeviceManager* Create(nvrhi::GraphicsAPI api);

        bool CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, GuGuUtf8Str& windowTitle);

        bool CreateInstance(const InstanceParameters& params);
    private:
        static DeviceManager* CreateVK();

    protected:
        DeviceCreationParameters m_deviceParams;
        bool m_requestedVsync = false;
        bool m_instanceCreated = false;

        //device-specific methods
        virtual bool CreateInstanceInternal() = 0;
        virtual void DestroyDeviceAndSwapChain() = 0;
        virtual bool CreateDevice() = 0;
    public:
        virtual void ShutDown();
    };
}