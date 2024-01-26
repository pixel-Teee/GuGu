#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu {
    namespace nvrhi::vulkan {
        Queue::Queue(const VulkanContext &context, CommandQueue queueID, VkQueue queue,
        uint32_t queueFamilyIndex)
        : m_Context(context)
        , m_Queue(queue)
        , m_QueueID(queueID)
        , m_QueueFamilyIndex(queueFamilyIndex){
            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{};
            semaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreTypeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreCreateInfo.pNext = &semaphoreTypeCreateInfo;

            vkCreateSemaphore(context.device, &semaphoreCreateInfo, context.allocationCallbacks, &trackingSemaphore);
        }

        Queue::~Queue() {
            vkDestroySemaphore(m_Context.device, trackingSemaphore, m_Context.allocationCallbacks);
            trackingSemaphore = VK_NULL_HANDLE;
        }

    }

}