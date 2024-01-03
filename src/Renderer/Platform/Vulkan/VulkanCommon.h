#pragma once

#include <optional>
#include <vector>

namespace GuGu{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };

    constexpr bool enableValidationLayers = true;

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}