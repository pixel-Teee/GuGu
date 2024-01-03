#pragma once

#include "Adapter.h"

#include "VulkanCommon.h"

namespace GuGu{
    class VulkanAdapter : public Adapter{
    public:
        VulkanAdapter();

        virtual ~VulkanAdapter();

        void init();

        virtual void onDestroy() override;

        //QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        VkInstance getInstance();
        VkDebugUtilsMessengerEXT getDebugMessenger();
        VkPhysicalDevice getAdapterHandle();
    private:
        void setupDebugMessenger();
        void pickPhysicalDevice();
        VkInstance m_instance; //vulkan api context
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;//adapter
    };
}