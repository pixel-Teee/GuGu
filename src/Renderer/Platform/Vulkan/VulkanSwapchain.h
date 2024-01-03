#pragma once

#include <Renderer/SwapChain.h>
#include <vector>

namespace GuGu{
    class VulkanSwapChain : public SwapChain{
    public:
        VulkanSwapChain(VkPhysicalDevice* device, VkDevice* logicDevice, VkSurfaceKHR* surface);

        virtual ~VulkanSwapChain();

        void init();

        VkSwapchainKHR getSwapChain();

        std::vector<VkImage>& getImages();

        VkFormat getFormat();

        std::vector<VkImageView> getImageViews();

        virtual void onDestroy() override;
    private:
        VkPhysicalDevice* m_device;//weak
        VkSurfaceKHR* m_surface;//weak
        VkDevice* m_logicDevice;//weak
        VkSwapchainKHR m_swapChain;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
    };
}