#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        CommandList::CommandList(Device *device, const VulkanContext &context,
                                 const CommandListParameters &parameters)
        : m_Device(device)
        , m_Context(context)
        , m_CommandListParameters(parameters){
            //todo:add state tracker and upload manager

        }

        void CommandList::open() {
            m_CurrentCmdBuf = m_Device->getQueue(m_CommandListParameters.queueType)->getOrCreateCommandBuffer();

            VkCommandBufferBeginInfo commandBufferBeginInfo = {};
            commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(m_CurrentCmdBuf->cmdBuf, &commandBufferBeginInfo);
            m_CurrentCmdBuf->referencedResources.push_back(this);//prevent deletion of e.g. UploadManager

            clearState();
        }

        void CommandList::close() {
            //todo:add state tracker and commit barriers

            vkEndCommandBuffer(m_CurrentCmdBuf->cmdBuf);

            clearState();

            //todo:add flush volatile buffer writes
        }

        void CommandList::clearState() {
            //todo:add logic
        }
    }
}