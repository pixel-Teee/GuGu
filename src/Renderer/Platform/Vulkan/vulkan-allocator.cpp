#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{

    namespace nvrhi::vulkan{
        void VulkanAllocator::freeTextureMemory(Texture *texture) const {
            freeMemory(texture);
        }

        void VulkanAllocator::freeMemory(MemoryResource *res) const {
            assert(res->managed);

            //m_Context.device.freeMemory(res->memory, m_Context.allocationCallbacks);
            //res->memory = vk::DeviceMemory(nullptr);
            vkFreeMemory(m_Context.device, res->memory, m_Context.allocationCallbacks);
            res->memory = VK_NULL_HANDLE;
        }
    }
}