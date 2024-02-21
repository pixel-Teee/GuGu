#pragma once

#include <Renderer/DeviceManager.h>

#include "vulkan-constants.h"

#include <queue>
#include <vector>

namespace GuGu{
    class DeviceManager_VK : public DeviceManager{
    public:
        [[nodiscard]] nvrhi::IDevice* GetDevice() const override
        {
            //if (m_ValidationLayer)
            //    return m_ValidationLayer;

            return m_NvrhiDevice;
        }

        [[nodiscard]] nvrhi::GraphicsAPI GetGraphicsAPI() const override
        {
            return nvrhi::GraphicsAPI::VULKAN;
        }

    protected:
        bool CreateInstanceInternal() override;
        bool CreateDevice() override;
        bool CreateSwapChain() override;

        void DestroyDeviceAndSwapChain() override;

        void ResizeSwapChain() override
        {
            if (m_VulkanDevice)
            {
                destroySwapChain();
                createWindowSurface();
                createSwapChain();
            }
        }

		void BeginFrame() override;
		void Present();

        virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) override;
        virtual uint32_t GetCurrentBackBufferIndex() override;
        virtual uint32_t GetBackBufferCount() override;

        
    private:
        bool createInstance();
        bool createWindowSurface();
        void installDebugCallback();
        bool pickPhysicalDevice();
        bool findQueueFamilies(VkPhysicalDevice physicalDevice);
        bool createDevice();
        bool createSwapChain();
        void destroySwapChain();

        GuGuUtf8Str m_rendererString;//device name

        VkInstance m_VulkanInstance;
        //VkDebugReportCallbackEXT m_debugReportCallback;
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger

        VkSurfaceKHR m_windowSurface;
        VkSurfaceFormatKHR m_SwapChainFormat;
        VkSwapchainKHR m_SwapChain;
        bool m_SwapChainMutableFormatSupported = false;

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

        nvrhi::CommandListHandle m_BarrierCommandList;

        std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
        std::vector<nvrhi::EventQueryHandle> m_QueryPool;

        std::vector<VkSemaphore> m_PresentSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        uint64_t m_framesIndexInFlight = 0;
    };

}
