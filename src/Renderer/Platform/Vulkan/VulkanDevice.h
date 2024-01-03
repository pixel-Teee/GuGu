#pragma once

#include <Renderer/Device.h>

#include <memory>

#include <Renderer/Device.h>

namespace GuGu{
    class VulkanAdapter;
    class VulkanDevice : public Device{
    public:
        VulkanDevice(std::shared_ptr<VulkanAdapter> adapter);

        virtual ~VulkanDevice();

        virtual void onDestroy() override;

        void init();

        VkQueue getGraphicsQueue();

        VkDevice getDeviceHandle();


    private:
        std::weak_ptr<VulkanAdapter> m_vulkanAdapter;

        VkDevice m_device;

        VkQueue m_graphicsQueue;
    };
}