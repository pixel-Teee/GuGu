#include <pch.h>

#include "vulkan-constants.h"

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan {
        DeviceHandle createDevice(const DeviceDesc &desc) {
            Device* device = new Device(desc);
            return DeviceHandle::Create(device);
        }

        Device::Device(const nvrhi::vulkan::DeviceDesc &desc)
        : m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))
        , m_Allocator(m_Context)
        , m_TimerQueryAllocator(desc.maxTimerQueries, true) //256
        {

        }

        Device::~Device() {

        }

    }
}