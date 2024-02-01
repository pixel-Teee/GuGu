#include <pch.h>

#include "vulkan-backend.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
    namespace nvrhi::vulkan {
        Queue::Queue(const VulkanContext &context, CommandQueue queueID, VkQueue queue,
        uint32_t queueFamilyIndex)
        : m_Context(context)
        , m_Queue(queue)
        , m_QueueID(queueID)
        , m_QueueFamilyIndex(queueFamilyIndex){
            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{};
            semaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
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

        void Queue::addWaitSemaphore(VkSemaphore semaphore, uint64_t value) {
            if(!semaphore)
                return;

            m_WaitSemahpores.push_back(semaphore);
            m_WaitSemahporeValues.push_back(value);
        }

        TrackedCommandBufferPtr Queue::getOrCreateCommandBuffer() {
            std::lock_guard lockGuard(m_Mutex); // this is called from CommandList::open, so free-threaded

            uint64_t recordingID = ++m_LastRecordingID;

            TrackedCommandBufferPtr cmdBuf;
            if (m_CommandBuffersPool.empty())
            {
                cmdBuf = createCommandBuffer();
            }
            else
            {
                cmdBuf = m_CommandBuffersPool.front();
                m_CommandBuffersPool.pop_front();
            }

            cmdBuf->recordingId = recordingID;
            return cmdBuf;
        }

        TrackedCommandBufferPtr Queue::createCommandBuffer() {
            VkResult res;

            TrackedCommandBufferPtr ret = std::make_shared<TrackedCommandBuffer>(m_Context);

            VkCommandPoolCreateInfo commandPoolCreateInfo = {};
            commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

            res = vkCreateCommandPool(m_Context.device, &commandPoolCreateInfo, m_Context.allocationCallbacks, &ret->cmdPool);

            VK_CHECK(res);

            //allocate command buffer

            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandPool = ret->cmdPool;
            commandBufferAllocateInfo.commandBufferCount = 1;

            res = vkAllocateCommandBuffers(m_Context.device, &commandBufferAllocateInfo, &ret->cmdBuf);

            VK_CHECK(res);

            return ret;
        }


    }

}