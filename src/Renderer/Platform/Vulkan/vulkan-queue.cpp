#include <pch.h>

#include "vulkan-backend.h"

#include <Core/GuGuUtf8Str.h>

#include <Renderer/misc.h>

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

            m_WaitSemaphores.push_back(semaphore);
            m_WaitSemaphoreValues.push_back(value);
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

            cmdBuf->recordingID = recordingID;
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

        uint64_t Queue::submit(ICommandList *const *ppCmd, size_t numCmd) {
            std::vector<VkPipelineStageFlags> waitStageArray(m_WaitSemaphores.size());
            std::vector<VkCommandBuffer> commandBuffers(numCmd);

            for (size_t i = 0; i < m_WaitSemaphores.size(); i++)
            {
                waitStageArray[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            }

            m_LastSubmittedID++;

            for (size_t i = 0; i < numCmd; i++)
            {
                CommandList* commandList = checked_cast<CommandList*>(ppCmd[i]);
                TrackedCommandBufferPtr commandBuffer = commandList->getCurrentCmdBuf();

                commandBuffers[i] = commandBuffer->cmdBuf;
                m_CommandBuffersInFlight.push_back(commandBuffer);

                for (const auto& buffer : commandBuffer->referencedStagingBuffers)
                {
                    buffer->lastUseQueue = m_QueueID;
                    buffer->lastUseCommandListID = m_LastSubmittedID;
                }
            }

            m_SignalSemaphores.push_back(trackingSemaphore);//timeline semaphore
            m_SignalSemaphoreValues.push_back(m_LastSubmittedID);

            VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {};
            timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
            timelineSemaphoreInfo.signalSemaphoreValueCount = uint32_t(m_SignalSemaphoreValues.size());
            timelineSemaphoreInfo.pSignalSemaphoreValues = m_SignalSemaphoreValues.data();

            //auto timelineSemaphoreInfo = vk::TimelineSemaphoreSubmitInfo()
            //        .setSignalSemaphoreValueCount(uint32_t(m_SignalSemaphoreValues.size()))
            //        .setPSignalSemaphoreValues(m_SignalSemaphoreValues.data());

            if (!m_WaitSemaphoreValues.empty())
            {
                timelineSemaphoreInfo.waitSemaphoreValueCount = uint32_t(m_WaitSemaphoreValues.size());
                timelineSemaphoreInfo.pWaitSemaphoreValues = m_WaitSemaphoreValues.data();
            }

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = &timelineSemaphoreInfo;
            submitInfo.commandBufferCount = uint32_t(numCmd);
            submitInfo.pCommandBuffers = commandBuffers.data();
            submitInfo.waitSemaphoreCount = uint32_t(m_WaitSemaphores.size());
            submitInfo.pWaitSemaphores = m_WaitSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStageArray.data();
            submitInfo.signalSemaphoreCount = uint32_t(m_SignalSemaphores.size());
            submitInfo.pSignalSemaphores = m_SignalSemaphores.data();

            //auto submitInfo = vk::SubmitInfo()
            //        .setPNext(&timelineSemaphoreInfo)
            //        .setCommandBufferCount(uint32_t(numCmd))
            //        .setPCommandBuffers(commandBuffers.data())
            //        .setWaitSemaphoreCount(uint32_t(m_WaitSemaphores.size()))
            //        .setPWaitSemaphores(m_WaitSemaphores.data())
            //        .setPWaitDstStageMask(waitStageArray.data())
            //        .setSignalSemaphoreCount(uint32_t(m_SignalSemaphores.size()))
            //        .setPSignalSemaphores(m_SignalSemaphores.data());

            vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE);

            //m_Queue.submit(submitInfo);

            m_WaitSemaphores.clear();
            m_WaitSemaphoreValues.clear();
            m_SignalSemaphores.clear();
            m_SignalSemaphoreValues.clear();

            return m_LastSubmittedID;
        }

        VkSemaphore Device::getQueueSemaphore(CommandQueue queueID)
        {
            Queue& queue = *m_Queues[uint32_t(queueID)];

            return queue.trackingSemaphore;
        }

        uint64_t Device::queueGetCompletedInstance(CommandQueue queue)
        {
            uint64_t value = 0;
            PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR = (PFN_vkGetSemaphoreCounterValueKHR) vkGetInstanceProcAddr(m_Context.instance, "vkGetSemaphoreCounterValueKHR");
            vkGetSemaphoreCounterValueKHR(m_Context.device, getQueueSemaphore(queue), &value);
            return value;
            //return m_Context.device.getSemaphoreCounterValue(getQueueSemaphore(queue));
        }

        bool Queue::pollCommandList(uint64_t commandListID)
        {
            if (commandListID > m_LastSubmittedID || commandListID == 0)
            return false;

            bool completed = getLastFinishedID() >= commandListID;
            if (completed)
            return true;

            completed = updateLastFinishedID() >= commandListID;
            return completed;
        }

        bool Queue::waitCommandList(uint64_t commandListID, uint64_t timeout)
        {
            if (commandListID > m_LastSubmittedID || commandListID == 0)
                return false;

            if (pollCommandList(commandListID))
                return true;

            std::array<const VkSemaphore, 1> semaphores = { trackingSemaphore };
            std::array<uint64_t, 1> waitValues = { commandListID };

            //auto waitInfo = vk::SemaphoreWaitInfo()
            //        .setSemaphores(semaphores)
            //        .setValues(waitValues);
//
            //vk::Result result = m_Context.device.waitSemaphores(waitInfo, timeout);

            //return (result == vk::Result::eSuccess);

            VkSemaphoreWaitInfo semaphoreWaitInfo = {};
            semaphoreWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            semaphoreWaitInfo.semaphoreCount = semaphores.size();
            semaphoreWaitInfo.pSemaphores = semaphores.data();
            semaphoreWaitInfo.pValues = waitValues.data();

            PFN_vkWaitSemaphores vkWaitSemaphores = (PFN_vkWaitSemaphores) vkGetInstanceProcAddr(m_Context.instance, "vkWaitSemaphores");

            VkResult result = vkWaitSemaphores(m_Context.device, &semaphoreWaitInfo, timeout);

            return result == VK_SUCCESS;
        }

        uint64_t Queue::updateLastFinishedID()
        {
            PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR = (PFN_vkGetSemaphoreCounterValueKHR) vkGetInstanceProcAddr(m_Context.instance, "vkGetSemaphoreCounterValueKHR");
            vkGetSemaphoreCounterValueKHR(m_Context.device, trackingSemaphore, &m_LastFinishedID);
            //m_LastFinishedID = m_Context.device.getSemaphoreCounterValue(trackingSemaphore);

            return m_LastFinishedID;
        }

        void Queue::addSignalSemaphore(VkSemaphore semaphore, uint64_t value) {
            if (!semaphore)
                return;

            m_SignalSemaphores.push_back(semaphore);
            m_SignalSemaphoreValues.push_back(value);
        }

        void Queue::retireCommandBuffers() {
            std::list<TrackedCommandBufferPtr> submissions = std::move(m_CommandBuffersInFlight);

            uint64_t lastFinishedID = updateLastFinishedID();

            for (const TrackedCommandBufferPtr& cmd : submissions)
            {
                if (cmd->submissionID <= lastFinishedID)
                {
                    cmd->referencedResources.clear();
                    cmd->referencedStagingBuffers.clear();
                    cmd->submissionID = 0;
                    m_CommandBuffersPool.push_back(cmd);
                }
                else
                {
                    m_CommandBuffersInFlight.push_back(cmd);
                }
            }
        }

        void Device::queueSignalSemaphore(CommandQueue executionQueueID, VkSemaphore semaphore,
                                          uint64_t value) {
            Queue& executionQueue = *m_Queues[uint32_t(executionQueueID)];

            executionQueue.addSignalSemaphore(semaphore, value);
        }
    }

}