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
            if(desc.graphicsQueue)
            {
                m_Queues[uint32_t(CommandQueue::Graphics)] = std::make_unique<Queue>(m_Context,
                                                                                     CommandQueue::Graphics, desc.graphicsQueue,
                                                                                     desc.graphicsQueueIndex);
            }

            if (desc.computeQueue)
            {
                m_Queues[uint32_t(CommandQueue::Compute)] = std::make_unique<Queue>(m_Context,
                                                                                    CommandQueue::Compute, desc.computeQueue, desc.computeQueueIndex);
            }

            if (desc.transferQueue)
            {
                m_Queues[uint32_t(CommandQueue::Copy)] = std::make_unique<Queue>(m_Context,
                                                                                 CommandQueue::Copy, desc.transferQueue, desc.transferQueueIndex);
            }

            //todo:add feature checking
        }

        Device::~Device() {

        }


    }
}