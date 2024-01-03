#include "pch.h"

#include "VulkanSwapchain.h"

#include "VulkanCommon.h"

#include <Application/Platform/Android/AndroidApplication.h>
#include <Window/Platform/Android/AndroidWindow.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>//native window

namespace GuGu{

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.m_formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.m_formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.m_presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.m_presentModes.data());
        }
        //bool swapChainAdequate = false;
        //if (extensionsSupported) {
        //todo:add extension support query
         //   SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
         //   swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.m_presentModes.empty();
        //}
        return details;
    }


    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto &availableFormat: availableFormats) {
            if (
                availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            //glfwGetFramebufferSize(window, &width, &height);

            std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
            std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
            //vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
            height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
            width = ANativeWindow_getWidth(androidWindow->getNativeHandle());

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VulkanSwapChain::~VulkanSwapChain() {

    }

    VulkanSwapChain::VulkanSwapChain(VkPhysicalDevice *device, VkDevice* logicDevice, VkSurfaceKHR *surface) {
        m_device = device;
        m_logicDevice = logicDevice;
        m_surface = surface;
    }

    void VulkanSwapChain::init() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(*m_device, *m_surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.m_formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.m_presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        //uint32_t imageCount = swapChainSupport.capabilities.minImageCount;
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = *m_surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
       // QueueFamilyIndices indices = findQueueFamilies(m_device);
        //QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        //uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

       //if (indices.graphicsFamily != indices.presentFamily) {
       //    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
       //    createInfo.queueFamilyIndexCount = 2;
       //    createInfo.pQueueFamilyIndices = queueFamilyIndices;
       //} else {
       createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
       createInfo.queueFamilyIndexCount = 0; // Optional
       createInfo.pQueueFamilyIndices = nullptr; // Optional
       createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
       createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
       createInfo.presentMode = presentMode;
       createInfo.clipped = VK_TRUE;
       createInfo.oldSwapchain = VK_NULL_HANDLE;
        //}
        if (vkCreateSwapchainKHR(*m_logicDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
            GuGu_LOGD("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(*m_logicDevice, m_swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(*m_logicDevice, m_swapChain, &imageCount, swapChainImages.data());

        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;//todo:fix this
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            if (vkCreateImageView(*m_logicDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                GuGu_LOGE("failed to create image views!");
            }
        }
    }

    VkSwapchainKHR VulkanSwapChain::getSwapChain() {
        return m_swapChain;
    }

    void VulkanSwapChain::onDestroy() {
        vkDestroySwapchainKHR(*m_logicDevice, m_swapChain, nullptr);
    }

    std::vector<VkImage> &VulkanSwapChain::getImages() {
        return swapChainImages;
    }

    VkFormat VulkanSwapChain::getFormat() {
        return VK_FORMAT_R8G8B8A8_SRGB;//todo:fix this
    }

    std::vector<VkImageView> VulkanSwapChain::getImageViews() {
        return swapChainImageViews;
    }
}