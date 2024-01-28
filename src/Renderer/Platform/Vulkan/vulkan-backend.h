#pragma once

#include <Renderer/nvrhi.h>
#include <Renderer/resource.h>
#include "vulkan-constants.h"

#include <vulkan/vulkan.h>

#include "utils.h"
#include "vk_types.h"

#include <list>
#include <array>
#include <Renderer/state-tracking.h>

#include <tuple>

namespace GuGu{
    class GuGuUtf8Str;
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

            struct {
                bool KHR_synchronization2 = false;
                bool KHR_maintenance1 = false;
                bool EXT_debug_report = false;
                bool EXT_debug_marker = false;
                bool KHR_acceleration_structure = false;
                bool buffer_device_address = false; // either KHR_ or Vulkan 1.2 versions
                bool KHR_ray_query = false;
                bool KHR_ray_tracing_pipeline = false;
                bool NV_mesh_shader = false;
                bool KHR_fragment_shading_rate = false;
                bool EXT_conservative_rasterization = false;
                bool EXT_opacity_micromap = false;
                bool NV_ray_tracing_invocation_reorder = false;
            } extensions;

            VkPhysicalDeviceProperties physicalDeviceProperties;
            //VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties;
            //VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties;
            VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties;
            VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties;
            //VkPhysicalDeviceOpacityMicromapPropertiesEXT opacityMicromapProperties;
            //VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV nvRayTracingInvocationReorderProperties;
            VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures;
            IMessageCallback* messageCallback = nullptr;

            void warning(const GuGuUtf8Str& message) const;
            void error(const GuGuUtf8Str& message) const;
        };

        class MemoryResource
        {
        public:
            bool managed = true;
            VkDeviceMemory memory;
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

        class Texture;
        struct TextureSubresourceView
        {
            Texture& texture;
            TextureSubresourceSet subresource;

            VkImageView view = VK_NULL_HANDLE;
            VkImageSubresourceRange subresourceRange;

            TextureSubresourceView(Texture& texture)
                    : texture(texture)
            { }

            TextureSubresourceView(const TextureSubresourceView&) = delete;

            bool operator==(const TextureSubresourceView& other) const
            {
                return &texture == &other.texture &&
                       subresource == other.subresource &&
                       view == other.view &&
                       subresourceRange.aspectMask == other.subresourceRange.aspectMask &&
                       subresourceRange.baseMipLevel == other.subresourceRange.baseMipLevel &&
                       subresourceRange.levelCount == other.subresourceRange.levelCount &&
                       subresourceRange.baseArrayLayer == other.subresourceRange.baseArrayLayer &&
                       subresourceRange.layerCount == other.subresourceRange.layerCount;
            }
        };

        class Texture : public MemoryResource, public RefCounter<ITexture>, public TextureStateExtension
        {
        public:

            enum class TextureSubresourceViewType // see getSubresourceView()
            {
                AllAspects,
                DepthOnly,
                StencilOnly
            };

            struct Hash
            {
                std::size_t operator()(std::tuple<TextureSubresourceSet, TextureSubresourceViewType, TextureDimension, Format> const& s) const noexcept
                {
                    const auto& [subresources, viewType, dimension, format] = s;

                    size_t hash = 0;

                    hash_combine(hash, subresources.baseMipLevel);
                    hash_combine(hash, subresources.numMipLevels);
                    hash_combine(hash, subresources.baseArraySlice);
                    hash_combine(hash, subresources.numArraySlices);
                    hash_combine(hash, viewType);
                    hash_combine(hash, dimension);
                    hash_combine(hash, format);

                    return hash;
                }
            };


            TextureDesc desc;
//
            VkImageCreateInfo imageInfo;
            VkExternalMemoryImageCreateInfo externalMemoryImageInfo;
            VkImage image;
//
            //HeapHandle heap;
//
            void* sharedHandle = nullptr;
//
            //// contains subresource views for this texture
            //// note that we only create the views that the app uses, and that multiple views may map to the same subresources
            std::unordered_map<std::tuple<TextureSubresourceSet, TextureSubresourceViewType, TextureDimension, Format>, TextureSubresourceView, Texture::Hash> subresourceViews;
//
            Texture(const VulkanContext& context, VulkanAllocator& allocator)
                    : TextureStateExtension(desc)
                    , m_Context(context)
                    , m_Allocator(allocator)
            { }
            //todo:implement function
             //returns a subresource view for an arbitrary range of mip levels and array layers.
             //'viewtype' only matters when asking for a depthstencil view; in situations where only depth or stencil can be bound
             //(such as an SRV with ImageLayout::eShaderReadOnlyOptimal), but not both, then this specifies which of the two aspect bits is to be set.
            TextureSubresourceView& getSubresourceView(const TextureSubresourceSet& subresources, TextureDimension dimension,
                                                       Format format, TextureSubresourceViewType viewtype = TextureSubresourceViewType::AllAspects);

            uint32_t getNumSubresources() const;
            uint32_t getSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const;

            ~Texture() override;
            const TextureDesc& getDesc() const override { return desc; }
            Object getNativeObject(ObjectType objectType) override;
            Object getNativeView(ObjectType objectType, Format format, TextureSubresourceSet subresources, TextureDimension dimension, bool isReadOnlyDSV = false) override;

        private:
            const VulkanContext& m_Context;
            VulkanAllocator& m_Allocator;
            std::mutex m_Mutex;
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
            TextureHandle createHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) override;
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