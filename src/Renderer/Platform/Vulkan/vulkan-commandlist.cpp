#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        CommandList::CommandList(Device *device, const VulkanContext &context,
                                 const CommandListParameters &parameters)
        : m_Device(device)
        , m_Context(context)
        , m_CommandListParameters(parameters)
        , m_StateTracker(context.messageCallback)
        , m_UploadManager(std::make_unique<UploadManager>(device, parameters.uploadChunkSize, 0, false))
        , m_ScratchManager(std::make_unique<UploadManager>(device, parameters.scratchChunkSize, parameters.scratchMaxMemory, true)){
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
            endRenderPass();

            m_StateTracker.keepBufferInitialStates();
            m_StateTracker.keepTextureInitialStates();
            commitBarriers();

            vkEndCommandBuffer(m_CurrentCmdBuf->cmdBuf);

            clearState();

            //todo:add flush volatile buffer writes
            flushVolatileBufferWrites();
        }

        void CommandList::clearState() {
            //todo:add logic
            endRenderPass();

            m_CurrentPipelineLayout = {};
            m_CurrentPushConstantsVisibility = {};

            m_CurrentGraphicsState = GraphicsState();

            //todo:clear shader table state

            m_AnyVolatileBufferWrites = false;
        }

        void CommandList::executed(Queue &queue, uint64_t submissionID) {
            assert(m_CurrentCmdBuf);

            m_CurrentCmdBuf->submissionID = submissionID;

            const CommandQueue queueID = queue.getQueueID();
            const uint64_t recordingID = m_CurrentCmdBuf->recordingID;

            m_CurrentCmdBuf = nullptr;

            //todo:implement this
            submitVolatileBuffers(recordingID, submissionID);

            m_StateTracker.commandListSubmitted();

            m_UploadManager->submitChunks(
                    MakeVersion(recordingID, queueID, false),
                    MakeVersion(submissionID, queueID, true));
//
            m_ScratchManager->submitChunks(
                    MakeVersion(recordingID, queueID, false),
                    MakeVersion(submissionID, queueID, true));

            m_VolatileBufferStates.clear();
        }
    }
}