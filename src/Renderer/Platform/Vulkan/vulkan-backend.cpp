#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        Device::Device(const nvrhi::vulkan::DeviceDesc &desc)
        : m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))

        {

        }

        Device::~Device() {

        }

    }

}