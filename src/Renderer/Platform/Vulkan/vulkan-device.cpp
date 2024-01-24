#include <pch.h>

#include "vulkan-constants.h"

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan {
        DeviceHandle createDevice(const DeviceDesc &desc) {
            Device* device = new Device(desc);
            return DeviceHandle::Create(device);
        }
    }
}