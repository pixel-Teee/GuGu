#pragma once

#include <Renderer/DeviceManager.h>

#include "vulkan-constants.h"

namespace GuGu{
    class DeviceManager_VK : public DeviceManager{
    public:

    protected:
        bool CreateInstanceInternal() override;
        void DestroyDeviceAndSwapChain() override;
        bool CreateDevice() override;
        bool CreateSwapChain() override;
#if 1
    public:
        VkInstance getInstance();
        VkDebugUtilsMessengerEXT getDebugMessenger();
        VkDevice getDevice();
        VkPhysicalDevice getPhysicalDevice();
        VkQueue getQueue();
        int32_t getQueueFamilyIndex();
        VkSurfaceKHR getSurface();
#endif
    private:
        bool createInstance();
        bool createWindowSurface();
        void installDebugCallback();
        bool pickPhysicalDevice();
        bool findQueueFamilies(VkPhysicalDevice physicalDevice);
        bool createDevice();
        bool createSwapChain();
        void destroySwapChain();

        GuGuUtf8Str m_rendererString;

        VkInstance m_VulkanInstance;
        //VkDebugReportCallbackEXT m_debugReportCallback;
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger
        VkSurfaceKHR m_windowSurface;

        VkSurfaceFormatKHR m_SwapChainFormat;
        VkSwapchainKHR m_SwapChain;

        struct SwapChainImage
        {
            VkImage image;
            nvrhi::TextureHandle rhiHandle;
        };
        std::vector<SwapChainImage> m_SwapChainImages;
        uint32_t m_SwapChainIndex = uint32_t(-1);

        VkPhysicalDevice m_VulkanPhysicalDevice;

        int32_t m_GraphicsQueueFamily = -1;
        int32_t m_ComputeQueueFamily = -1;
        int32_t m_TransferQueueFamily = -1;
        int32_t m_PresentQueueFamily = -1;

        VkDevice m_VulkanDevice;
        VkQueue m_GraphicsQueue;
        VkQueue m_ComputeQueue;
        VkQueue m_TransferQueue;
        VkQueue m_PresentQueue;

        bool m_BufferDeviceAddressSupported = false;

        nvrhi::vulkan::DeviceHandle m_NvrhiDevice;

        bool m_SwapChainMutableFormatSupported = false;
    };

}