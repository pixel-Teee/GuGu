#include <pch.h>

#include "VulkanDevice.h"

#include "VulkanCommon.h"

#include "VulkanAdapter.h"

namespace GuGu{

    //VulkanDevice::VulkanDevice() {
//
    //}



    VulkanDevice::~VulkanDevice() {

    }

    void VulkanDevice::onDestroy() {
        vkDestroyDevice(m_device, nullptr);
    }

    VulkanDevice::VulkanDevice(std::shared_ptr<VulkanAdapter> adapter) {
        m_vulkanAdapter = adapter;
    }

    void VulkanDevice::init() {
        auto adapter = m_vulkanAdapter.lock();
        QueueFamilyIndices indices = findQueueFamilies(adapter->getAdapterHandle());

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();//VK_KHR_SWAPCHAIN_EXTENSION_NAME

        if (vkCreateDevice(adapter->getAdapterHandle(), &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            GuGu_LOGD("failed to create logical device!");
        }

        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    }

    VkQueue VulkanDevice::getGraphicsQueue() {
        return m_graphicsQueue;
    }

    VkDevice VulkanDevice::getDeviceHandle() {
        return m_device;
    }
}