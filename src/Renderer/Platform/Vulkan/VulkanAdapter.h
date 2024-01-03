#pragma once

#include "Adapter.h"

namespace GuGu{
    class VulkanAdapter : public Adapter{
    public:
        VulkanAdapter();

        virtual ~VulkanAdapter();

        void init();

        virtual void onDestroy() override;

        VkInstance getInstance();
        VkDebugUtilsMessengerEXT getDebugMessenger();
    private:
        void setupDebugMessenger();
        VkInstance m_instance; //vulkan api context
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger
    };
}