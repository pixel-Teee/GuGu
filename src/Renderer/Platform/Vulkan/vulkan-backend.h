#pragma once

#include <Renderer/nvrhi.h>
#include <Renderer/resource.h>
#include "vulkan-constants.h"

#include <vulkan/vulkan.h>

#include "utils.h"

#include <list>
#include <array>

namespace GuGu{
    namespace nvrhi::vulkan{

        struct VulkanContext{
            VulkanContext(
                    VkInstance instance,
                    VkPhysicalDevice physicalDevice,
                    VkDevice device,
                    VkAllocationCallbacks* allocationCallbacks = nullptr
            ):instance(instance)
            , physicalDevice(physicalDevice)
            , device(device)
            , allocationCallbacks(allocationCallbacks)
            , pipelineCache(VK_NULL_HANDLE)
            {}
            VkInstance instance;
            VkPhysicalDevice physicalDevice;
            VkDevice device;
            VkAllocationCallbacks* allocationCallbacks;
            VkPipelineCache pipelineCache;
        };
        class VulkanAllocator{
        public:
            explicit VulkanAllocator(const VulkanContext& context)
            : m_Context(context)
            {}

        private:
            const VulkanContext& m_Context;
        };

        //command buffer with resource tracking
        class TrackedCommandBuffer
        {
        public:
            //the command buffer itself
            VkCommandBuffer cmdBuf = {};
            VkCommandPool cmdPool = {};

            std::vector<RefCountPtr<IResource>> referencedResources;//to keep them alive
            //todo:fix this
            //std::vector<RefCountPtr<Buffer>> referencedStagingBuffers;//to allow synchronous mapBuffer

            uint64_t recordingId = 0;
            uint64_t submissionId = 0;

            explicit TrackedCommandBuffer(const VulkanContext& context)
            : m_Context(context)
            {}

            ~TrackedCommandBuffer(){}
        private:
            const VulkanContext& m_Context;
        };

        typedef std::shared_ptr<TrackedCommandBuffer> TrackedCommandBufferPtr;

        //represents a hardware queue
        class Queue{
        public:
            VkSemaphore trackingSemaphore;

            //todo:add definition
            Queue(const VulkanContext& context, CommandQueue queueID, VkQueue queue, uint32_t queueFamilyIndex);
            ~Queue();

        private:
            const VulkanContext& m_Context;

            VkQueue m_Queue;
            CommandQueue m_QueueID;
            uint32_t m_QueueFamilyIndex = uint32_t(-1);

            std::mutex m_Mutex;
            std::vector<VkSemaphore> m_WaitSemahpores;
            std::vector<uint64_t> m_WaitSemahporeValues;
            std::vector<VkSemaphore> m_SignalSemaphores;
            std::vector<uint64_t> m_SignalSemaphoreValues;

            uint64_t m_LastRecordingID = 0;
            uint64_t m_LastSubmittedID = 0;
            uint64_t m_LastFinishedID = 0;

            //tracks the list of command buffers in flight on this queue
            std::list<TrackedCommandBufferPtr> m_CommandBuffersInFlight;
            std::list<TrackedCommandBufferPtr> m_CommandBuffersPool;
        };

        class Device : public nvrhi::RefCounter<nvrhi::vulkan::IDevice>
        {
        public:
            // Internal backend methods

            Device(const DeviceDesc& desc);
            ~Device() override;
//
            //Queue* getQueue(CommandQueue queue) const { return m_Queues[int(queue)].get(); }
            //vk::QueryPool getTimerQueryPool() const { return m_TimerQueryPool; }
//
            //// IResource implementation
//
            //Object getNativeObject(ObjectType objectType) override;
//
//
            //// IDevice implementation
//
            //HeapHandle createHeap(const HeapDesc& d) override;
//
            //TextureHandle createTexture(const TextureDesc& d) override;
            //MemoryRequirements getTextureMemoryRequirements(ITexture* texture) override;
            //bool bindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) override;
//
            //TextureHandle createHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) override;
//
            //StagingTextureHandle createStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess) override;
            //void *mapStagingTexture(IStagingTexture* tex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t *outRowPitch) override;
            //void unmapStagingTexture(IStagingTexture* tex) override;
//
            //BufferHandle createBuffer(const BufferDesc& d) override;
            //void *mapBuffer(IBuffer* b, CpuAccessMode mapFlags) override;
            //void unmapBuffer(IBuffer* b) override;
            //MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) override;
            //bool bindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) override;
//
            //BufferHandle createHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& desc) override;
//
            //ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) override;
            //ShaderHandle createShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) override;
            //ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) override;
//
            //SamplerHandle createSampler(const SamplerDesc& d) override;
//
            //InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader) override;
//
            //// event queries
            //EventQueryHandle createEventQuery() override;
            //void setEventQuery(IEventQuery* query, CommandQueue queue) override;
            //bool pollEventQuery(IEventQuery* query) override;
            //void waitEventQuery(IEventQuery* query) override;
            //void resetEventQuery(IEventQuery* query) override;
//
            //// timer queries
            //TimerQueryHandle createTimerQuery() override;
            //bool pollTimerQuery(ITimerQuery* query) override;
            //float getTimerQueryTime(ITimerQuery* query) override;
            //void resetTimerQuery(ITimerQuery* query) override;
//
            //GraphicsAPI getGraphicsAPI() override;
//
            //FramebufferHandle createFramebuffer(const FramebufferDesc& desc) override;
//
            //GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;
//
            //ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) override;
//
            //MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) override;
//
            //rt::PipelineHandle createRayTracingPipeline(const rt::PipelineDesc& desc) override;
//
            //BindingLayoutHandle createBindingLayout(const BindingLayoutDesc& desc) override;
            //BindingLayoutHandle createBindlessLayout(const BindlessLayoutDesc& desc) override;
//
            //BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) override;
            //DescriptorTableHandle createDescriptorTable(IBindingLayout* layout) override;
//
            //void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) override;
            //bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) override;
//
            //rt::OpacityMicromapHandle createOpacityMicromap(const rt::OpacityMicromapDesc& desc) override;
            //rt::AccelStructHandle createAccelStruct(const rt::AccelStructDesc& desc) override;
            //MemoryRequirements getAccelStructMemoryRequirements(rt::IAccelStruct* as) override;
            //bool bindAccelStructMemory(rt::IAccelStruct* as, IHeap* heap, uint64_t offset) override;
//
            //CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) override;
            //uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) override;
            //void queueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) override;
            //void waitForIdle() override;
            //void runGarbageCollection() override;
            //bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;
            //FormatSupport queryFormatSupport(Format format) override;
            //Object getNativeQueue(ObjectType objectType, CommandQueue queue) override;
            //IMessageCallback* getMessageCallback() override { return m_Context.messageCallback; }
//
            //// vulkan::IDevice implementation
            //VkSemaphore getQueueSemaphore(CommandQueue queue) override;
            //void queueWaitForSemaphore(CommandQueue waitQueue, VkSemaphore semaphore, uint64_t value) override;
            //void queueSignalSemaphore(CommandQueue executionQueue, VkSemaphore semaphore, uint64_t value) override;
            //uint64_t queueGetCompletedInstance(CommandQueue queue) override;
            //FramebufferHandle createHandleForNativeFramebuffer(VkRenderPass renderPass, VkFramebuffer framebuffer,
            //                                                  const FramebufferDesc& desc, bool transferOwnership) override;

        private:
            VulkanContext m_Context;
            VulkanAllocator m_Allocator;

            //vk::QueryPool m_TimerQueryPool = nullptr;
            utils::BitSetAllocator m_TimerQueryAllocator;

            //std::mutex m_Mutex;

            //// array of submission queues
            std::array<std::unique_ptr<Queue>, uint32_t(CommandQueue::Count)> m_Queues;

            //void *mapBuffer(IBuffer* b, CpuAccessMode flags, uint64_t offset, size_t size) const;
        };
    }

}