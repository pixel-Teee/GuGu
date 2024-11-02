#pragma once

#include <Renderer/DeviceManager.h>

#include "vulkan-constants.h"

#include <queue>
#include <vector>

namespace GuGu{
	struct SwapChainImage
	{
		VkImage image;
		nvrhi::TextureHandle rhiHandle;
	};
    struct WindowWidgetViewportInfo {
        //swap chain image 
        std::vector<SwapChainImage> m_swapChainImages;

        std::vector<SwapChainImage> m_oldSwapChainImages;

        VkSwapchainKHR m_SwapChain;
        VkSwapchainKHR m_oldSwapChain;
        VkSurfaceKHR m_windowSurface;
        //projection
        math::float4x4 m_orthogonalProjection;

        std::vector<VkSemaphore> m_presentSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

        uint64_t m_framesIndexInFlight = 0;
		uint32_t m_SwapChainIndex = uint32_t(-1);

        std::vector<nvrhi::FramebufferHandle> m_SwapChainFramebuffers;
    };
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

        virtual bool CreateSwapChain(std::shared_ptr<WindowWidget> windowWidget) override;

        virtual void UpdateWindowSize(std::shared_ptr<WindowWidget> windowWidget) override;

        virtual bool createWindowSurface(std::shared_ptr<WindowWidget> windowWidget) override;

        virtual nvrhi::FramebufferHandle getCurrentBackBuffer(std::shared_ptr<WindowWidget> windowWidget) const override;
    protected:
        bool CreateInstanceInternal() override;
        bool CreateDevice(std::shared_ptr<WindowWidget> inWindowWidget) override;

        void DestroyDeviceAndSwapChain() override;

        void ResizeSwapChain() override
        {
            if (m_VulkanDevice)
            {
                //m_oldSwapChain = m_SwapChain;
                //std::swap(m_OldSwapChainImages, m_SwapChainImages);
                destroyOldSwapChain();
                createWindowSurface();
                createSwapChain();
            }
        }

        virtual void ResizeSwapChain(std::shared_ptr<WindowWidget> windowWidget) override
        {
            if (m_VulkanDevice)
            {
				auto it = m_windowViewports.find(windowWidget.get());
				assert(it != m_windowViewports.end());
                it->second.m_oldSwapChain = it->second.m_SwapChain;
                std::swap(it->second.m_oldSwapChainImages, it->second.m_swapChainImages);

                destroyOldSwapChain(windowWidget);
                createWindowSurface(windowWidget);
                CreateSwapChain(windowWidget);
            }
        }

		void BeginFrame(std::shared_ptr<WindowWidget> inWindowWidget) override;
		void Present(std::shared_ptr<WindowWidget> inWindowWidget) override;

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
        void destroyOldSwapChain();
        void destroyOldSwapChain(std::shared_ptr<WindowWidget> inWindowWidget);

        GuGuUtf8Str m_rendererString;//device name

        VkInstance m_VulkanInstance;
        //VkDebugReportCallbackEXT m_debugReportCallback;
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger

        //VkSurfaceKHR m_windowSurface;
        VkSurfaceFormatKHR m_SwapChainFormat;
        //VkSwapchainKHR m_SwapChain;
        bool m_SwapChainMutableFormatSupported = false;

		//struct SwapChainImage
		//{
		//    VkImage image;
		//    nvrhi::TextureHandle rhiHandle;
		//};
		//std::vector<SwapChainImage> m_SwapChainImages;
        //窗口以及对应的交换链以及投影矩阵，surface
        std::map<WindowWidget*, WindowWidgetViewportInfo> m_windowViewports;

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

        //std::vector<VkSemaphore> m_PresentSemaphores;
        //std::vector<VkSemaphore> m_RenderFinishedSemaphores;

        VkExtent2D m_displaySizeIdentity;
        VkSurfaceTransformFlagBitsKHR m_preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        VkSwapchainKHR m_oldSwapChain = VK_NULL_HANDLE;
        //std::vector<SwapChainImage> m_OldSwapChainImages;
    };

}
