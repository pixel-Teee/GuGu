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
#include <Renderer/versioning.h>//todo:fix this

#include <tuple>

namespace GuGu{
    class GuGuUtf8Str;
    namespace nvrhi::vulkan{
        struct ResourceStateMapping
        {
            ResourceStates nvrhiState;
            VkPipelineStageFlags stageFlags;
            VkAccessFlags accessMask;
            VkImageLayout imageLayout;
            ResourceStateMapping(ResourceStates nvrhiState, VkPipelineStageFlags stageFlags, VkAccessFlags accessMask, VkImageLayout imageLayout):
                    nvrhiState(nvrhiState), stageFlags(stageFlags), accessMask(accessMask), imageLayout(imageLayout) {}
        };

        struct ResourceStateMapping2 // for use with KHR_synchronization2
        {
            ResourceStates nvrhiState;
            VkPipelineStageFlags2 stageFlags;
            VkAccessFlags2 accessMask;
            VkImageLayout imageLayout;
            ResourceStateMapping2(ResourceStates nvrhiState, VkPipelineStageFlags2 stageFlags, VkAccessFlags2 accessMask, VkImageLayout imageLayout) :
                    nvrhiState(nvrhiState), stageFlags(stageFlags), accessMask(accessMask), imageLayout(imageLayout) {}
        };

        VkShaderStageFlagBits convertShaderTypeToShaderStageFlagBits(ShaderType shaderType);
        VkPolygonMode convertFillMode(RasterFillMode mode);
        VkCullModeFlagBits convertCullMode(RasterCullMode mode);
        VkPrimitiveTopology convertPrimitiveTopology(PrimitiveType topology);
        VkCompareOp convertCompareOp(ComparisonFunc op);
        ResourceStateMapping convertResourceState(ResourceStates state);
        VkSamplerAddressMode convertSamplerAddressMode(SamplerAddressMode mode);
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

            void nameVKObject(const void* handle, VkObjectType objtype, const char* name) const;
            void warning(const GuGuUtf8Str& message) const;
            void error(const GuGuUtf8Str& message) const;
        };

        class MemoryResource
        {
        public:
            bool managed = true;
            VkDeviceMemory memory;
        };

        class Texture;
        class Buffer;
        class VulkanAllocator{
        public:
            explicit VulkanAllocator(const VulkanContext& context)
            : m_Context(context)
            {}
            void freeBufferMemory(Buffer* buffer) const;
            void freeTextureMemory(Texture* texture) const;
            void freeMemory(MemoryResource* res) const;
            VkResult allocateTextureMemory(Texture* texture) const;
            VkResult allocateBufferMemory(Buffer* buffer, bool enableBufferAddress = false) const;

            VkResult allocateMemory(MemoryResource* res,
                          VkMemoryRequirements memRequirements,
                          VkMemoryPropertyFlags memPropertyFlags,
                          bool enableDeviceAddress = false,
                          bool enableExportMemory = false,
                          VkImage dedicatedImage = VK_NULL_HANDLE,
                          VkBuffer dedicateBuffer = VK_NULL_HANDLE) const;
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
            std::vector<RefCountPtr<Buffer>> referencedStagingBuffers;//to allow synchronous mapBuffer

            uint64_t recordingID = 0;
            uint64_t submissionID = 0;

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

            // creates a command buffer and its synchronization resources
            TrackedCommandBufferPtr createCommandBuffer();

            TrackedCommandBufferPtr getOrCreateCommandBuffer();

            void addWaitSemaphore(VkSemaphore semaphore, uint64_t value);
            void addSignalSemaphore(VkSemaphore semaphore, uint64_t value);
            // submits a command buffer to this queue, returns submissionID
            uint64_t submit(ICommandList* const* ppCmd, size_t numCmd);

            uint64_t updateLastFinishedID();
            CommandQueue getQueueID() const { return m_QueueID; }
            uint64_t getLastSubmittedID() const { return m_LastSubmittedID; }
            uint64_t getLastFinishedID() const { return m_LastFinishedID; }

            bool pollCommandList(uint64_t commandListID);
            bool waitCommandList(uint64_t commandListID, uint64_t timeout);
        private:
            const VulkanContext& m_Context;

            VkQueue m_Queue;
            CommandQueue m_QueueID;
            uint32_t m_QueueFamilyIndex = uint32_t(-1);

            std::mutex m_Mutex;
            std::vector<VkSemaphore> m_WaitSemaphores;
            std::vector<uint64_t> m_WaitSemaphoreValues;
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

        class Framebuffer : public RefCounter<IFramebuffer>
        {
        public:
            FramebufferDesc desc;
            FramebufferInfoEx framebufferInfo;

            VkRenderPass renderPass = {};
            VkFramebuffer framebuffer = {};

            std::vector<ResourceHandle> resources;

            bool managed = true;

            explicit Framebuffer(const VulkanContext& context)
                    : m_Context(context)
            { }

            ~Framebuffer() override;
            const FramebufferDesc& getDesc() const override { return desc; }
            const FramebufferInfoEx& getFramebufferInfo() const override { return framebufferInfo; }
            Object getNativeObject(ObjectType objectType) override;

        private:
            const VulkanContext& m_Context;
        };

        class BindingLayout : public RefCounter<IBindingLayout>
        {
        public:
            BindingLayoutDesc desc;
            BindlessLayoutDesc bindlessDesc;
            bool isBindless;

            std::vector<VkDescriptorSetLayoutBinding> vulkanLayoutBindings;

            VkDescriptorSetLayout descriptorSetLayout;

            // descriptor pool size information per binding set
            std::vector<VkDescriptorPoolSize> descriptorPoolSizeInfo;

            BindingLayout(const VulkanContext& context, const BindingLayoutDesc& desc);
            BindingLayout(const VulkanContext& context, const BindlessLayoutDesc& desc);
            ~BindingLayout() override;
            const BindingLayoutDesc* getDesc() const override { return isBindless ? nullptr : &desc; }
            const BindlessLayoutDesc* getBindlessDesc() const override { return isBindless ? &bindlessDesc : nullptr; }
            Object getNativeObject(ObjectType objectType) override;

            // generate the descriptor set layout
            VkResult bake();

        private:
            const VulkanContext& m_Context;
        };

        // contains a vk::DescriptorSet
        class BindingSet : public RefCounter<IBindingSet>
        {
        public:
            BindingSetDesc desc;
            BindingLayoutHandle layout;

            // TODO: move pool to the context instead
            VkDescriptorPool descriptorPool;
            VkDescriptorSet descriptorSet;

            std::vector<ResourceHandle> resources;
            static_vector<Buffer*, c_MaxVolatileConstantBuffersPerLayout> volatileConstantBuffers;

            std::vector<uint16_t> bindingsThatNeedTransitions;

            explicit BindingSet(const VulkanContext& context)
                    : m_Context(context)
            { }

            ~BindingSet() override;
            const BindingSetDesc* getDesc() const override { return &desc; }
            IBindingLayout* getLayout() const override { return layout; }
            Object getNativeObject(ObjectType objectType) override;

        private:
            const VulkanContext& m_Context;
        };

        struct VolatileBufferState
        {
            int latestVersion = 0;
            int minVersion = 0;
            int maxVersion = 0;
            bool initialized = false;
        };

        // A copyable version of std::atomic to be used in an std::vector
        class BufferVersionItem : public std::atomic<uint64_t>  // NOLINT(cppcoreguidelines-special-member-functions)
        {
        public:
            BufferVersionItem()
                    : std::atomic<uint64_t>()
            { }

            BufferVersionItem(const BufferVersionItem& other)
            {
                store(other);
            }

            BufferVersionItem& operator=(const uint64_t a)
            {
                store(a);
                return *this;
            }
        };

        class Buffer : public MemoryResource, public RefCounter<IBuffer>, public BufferStateExtension
        {
        public:
            BufferDesc desc;

            VkBuffer buffer;
            VkDeviceAddress deviceAddress = 0;

            HeapHandle heap;

            std::unordered_map<uint64_t, VkBufferView> viewCache;

            std::vector<BufferVersionItem> versionTracking;
            void* mappedMemory = nullptr;
            void* sharedHandle = nullptr;
            uint32_t versionSearchStart = 0;

            // For staging buffers only
            CommandQueue lastUseQueue = CommandQueue::Graphics;
            uint64_t lastUseCommandListID = 0;

            Buffer(const VulkanContext& context, VulkanAllocator& allocator)
                    : BufferStateExtension(desc)
                    , m_Context(context)
                    , m_Allocator(allocator)
            { }

            ~Buffer() override;
            const BufferDesc& getDesc() const override { return desc; }
            Object getNativeObject(ObjectType type) override;

        private:
            const VulkanContext& m_Context;
            VulkanAllocator& m_Allocator;
        };

        class Sampler : public RefCounter<ISampler>
        {
        public:
            SamplerDesc desc;

            VkSamplerCreateInfo samplerInfo;
            VkSampler sampler;

            explicit Sampler(const VulkanContext& context)
                    : m_Context(context)
            { }

            ~Sampler() override;
            const SamplerDesc& getDesc() const override { return desc; }
            Object getNativeObject(ObjectType objectType) override;

        private:
            const VulkanContext& m_Context;
        };

        class Shader : public RefCounter<IShader>
        {
        public:
            ShaderDesc desc;

            VkShaderModule shaderModule;
            VkShaderStageFlagBits stageFlagBits{};

            // Shader specializations are just references to the original shader module
            // plus the specialization constant array.
            ResourceHandle baseShader; // Could be a Shader or ShaderLibrary
            std::vector<ShaderSpecialization> specializationConstants;

            explicit Shader(const VulkanContext& context)
                    : desc(ShaderType::None)
                    , m_Context(context)
            { }

            ~Shader() override;
            const ShaderDesc& getDesc() const override { return desc; }
            void getBytecode(const void** ppBytecode, size_t* pSize) const override;
            Object getNativeObject(ObjectType objectType) override;

        private:
            const VulkanContext& m_Context;
        };

        void countSpecializationConstants(
                Shader* shader,
                size_t& numShaders,
                size_t& numShadersWithSpecializations,
                size_t& numSpecializationConstants);

        VkPipelineShaderStageCreateInfo makeShaderStageCreateInfo(
                Shader* shader,
                std::vector<VkSpecializationInfo>& specInfos,
                std::vector<VkSpecializationMapEntry>& specMapEntries,
                std::vector<uint32_t>& specData);


        class InputLayout : public RefCounter<IInputLayout>
        {
        public:
            std::vector<VertexAttributeDesc> inputDesc;

            std::vector<VkVertexInputBindingDescription> bindingDesc;
            std::vector<VkVertexInputAttributeDescription> attributeDesc;

            uint32_t getNumAttributes() const override;
            const VertexAttributeDesc* getAttributeDesc(uint32_t index) const override;
        };

        template <typename T>
        using BindingVector = static_vector<T, c_MaxBindingLayouts>;

        class EventQuery : public RefCounter<IEventQuery>
        {
        public:
            CommandQueue queue = CommandQueue::Graphics;
            uint64_t commandListID = 0;
        };

        class GraphicsPipeline : public RefCounter<IGraphicsPipeline>
        {
        public:
            GraphicsPipelineDesc desc;
            FramebufferInfo framebufferInfo;
            ShaderType shaderMask = ShaderType::None;
            BindingVector<RefCountPtr<BindingLayout>> pipelineBindingLayouts;
            VkPipelineLayout pipelineLayout;
            VkPipeline pipeline;
            VkShaderStageFlags pushConstantVisibility;
            bool usesBlendConstants = false;

            explicit GraphicsPipeline(const VulkanContext& context)
                    : m_Context(context)
            { }

            ~GraphicsPipeline() override;
            const GraphicsPipelineDesc& getDesc() const override { return desc; }
            const FramebufferInfo& getFramebufferInfo() const override { return framebufferInfo; }
            Object getNativeObject(ObjectType objectType) override;

        private:
            const VulkanContext& m_Context;
        };

        struct BufferChunk
        {
            BufferHandle buffer;
            uint64_t version = 0;
            uint64_t bufferSize = 0;
            uint64_t writePointer = 0;
            void* mappedMemory = nullptr;

            static constexpr uint64_t c_sizeAlignment = 4096; // GPU page size
        };

        class Device;
        class UploadManager
        {
        public:
            UploadManager(Device* pParent, uint64_t defaultChunkSize, uint64_t memoryLimit, bool isScratchBuffer)
                    : m_Device(pParent)
                    , m_DefaultChunkSize(defaultChunkSize)
                    , m_MemoryLimit(memoryLimit)
                    , m_IsScratchBuffer(isScratchBuffer)
            { }

            std::shared_ptr<BufferChunk> CreateChunk(uint64_t size);

            bool suballocateBuffer(uint64_t size, Buffer** pBuffer, uint64_t* pOffset, void** pCpuVA, uint64_t currentVersion, uint32_t alignment = 256);
            void submitChunks(uint64_t currentVersion, uint64_t submittedVersion);

        private:
            Device* m_Device;
            uint64_t m_DefaultChunkSize = 0;
            uint64_t m_MemoryLimit = 0;
            uint64_t m_AllocatedMemory = 0;
            bool m_IsScratchBuffer = false;

            std::list<std::shared_ptr<BufferChunk>> m_ChunkPool;
            std::shared_ptr<BufferChunk> m_CurrentChunk;
        };


        class Device : public nvrhi::RefCounter<nvrhi::vulkan::IDevice>
        {
        public:
            // Internal backend methods

            Device(const DeviceDesc& desc);
            ~Device() override;
//
            Queue* getQueue(CommandQueue queue) const { return m_Queues[int(queue)].get(); }
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
            TextureHandle createTexture(const TextureDesc& d) override;
            //MemoryRequirements getTextureMemoryRequirements(ITexture* texture) override;
            //bool bindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) override;
//
            TextureHandle createHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) override;
//
            //StagingTextureHandle createStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess) override;
            //void *mapStagingTexture(IStagingTexture* tex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t *outRowPitch) override;
            //void unmapStagingTexture(IStagingTexture* tex) override;
//
            BufferHandle createBuffer(const BufferDesc& d) override;
            void *mapBuffer(IBuffer* b, CpuAccessMode mapFlags) override;
            void unmapBuffer(IBuffer* b) override;
            //MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) override;
            //bool bindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) override;
//
            //BufferHandle createHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& desc) override;
//
            ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) override;
            //ShaderHandle createShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) override;
            //ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) override;
//
            SamplerHandle createSampler(const SamplerDesc& d) override;
//
            InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader) override;
//
            //// event queries
            EventQueryHandle createEventQuery() override;
            void setEventQuery(IEventQuery* query, CommandQueue queue) override;
            //bool pollEventQuery(IEventQuery* query) override;
            void waitEventQuery(IEventQuery* query) override;
            void resetEventQuery(IEventQuery* query) override;
//
            //// timer queries
            //TimerQueryHandle createTimerQuery() override;
            //bool pollTimerQuery(ITimerQuery* query) override;
            //float getTimerQueryTime(ITimerQuery* query) override;
            //void resetTimerQuery(ITimerQuery* query) override;
//
            //GraphicsAPI getGraphicsAPI() override;
//
            FramebufferHandle createFramebuffer(const FramebufferDesc& desc) override;
//
            GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;
//
            //ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) override;
//
            //MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) override;
//
            //rt::PipelineHandle createRayTracingPipeline(const rt::PipelineDesc& desc) override;
//
            BindingLayoutHandle createBindingLayout(const BindingLayoutDesc& desc) override;
            //BindingLayoutHandle createBindlessLayout(const BindlessLayoutDesc& desc) override;
//
            BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) override;
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
            CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) override;
            uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) override;
            //void queueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) override;
            //void waitForIdle() override;
            //void runGarbageCollection() override;
            bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;
            //FormatSupport queryFormatSupport(Format format) override;
            //Object getNativeQueue(ObjectType objectType, CommandQueue queue) override;
            //IMessageCallback* getMessageCallback() override { return m_Context.messageCallback; }
//
            // vulkan::IDevice implementation
            VkSemaphore getQueueSemaphore(CommandQueue queue) override;
            void queueWaitForSemaphore(CommandQueue waitQueue, VkSemaphore semaphore, uint64_t value) override;
            void queueSignalSemaphore(CommandQueue executionQueue, VkSemaphore semaphore, uint64_t value) override;
            uint64_t queueGetCompletedInstance(CommandQueue queue) override;
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

            void *mapBuffer(IBuffer* b, CpuAccessMode flags, uint64_t offset, size_t size) const;
        };

        class CommandList : public RefCounter<ICommandList>
        {
        public:
            // Internal backend methods

            CommandList(Device* device, const VulkanContext& context, const CommandListParameters& parameters);

            void executed(Queue& queue, uint64_t submissionID);

            // IResource implementation

            //Object getNativeObject(ObjectType objectType) override;

            // ICommandList implementation

            void open() override;
            void close() override;
            void clearState() override;
//
            void clearTextureFloat(ITexture* texture, TextureSubresourceSet subresources, const Color& clearColor) override;
            //void clearDepthStencilTexture(ITexture* texture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil) override;
            //void clearTextureUInt(ITexture* texture, TextureSubresourceSet subresources, uint32_t clearColor) override;
//
            //void copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) override;
            //void copyTexture(IStagingTexture* dest, const TextureSlice& dstSlice, ITexture* src, const TextureSlice& srcSlice) override;
            //void copyTexture(ITexture* dest, const TextureSlice& dstSlice, IStagingTexture* src, const TextureSlice& srcSlice) override;
            void writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch) override;
            //void resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources) override;
//
            void writeBuffer(IBuffer* b, const void* data, size_t dataSize, uint64_t destOffsetBytes = 0) override;
            //void clearBufferUInt(IBuffer* b, uint32_t clearValue) override;
            void copyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes) override;
//
            //void setPushConstants(const void* data, size_t byteSize) override;
//
            void setGraphicsState(const GraphicsState& state) override;
            //void draw(const DrawArguments& args) override;
            void drawIndexed(const DrawArguments& args) override;
            //void drawIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
            //void drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
//
            //void setComputeState(const ComputeState& state) override;
            //void dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;
            //void dispatchIndirect(uint32_t offsetBytes)  override;
//
            //void setMeshletState(const MeshletState& state) override;
            //void dispatchMesh(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;
//
            //void setRayTracingState(const rt::State& state) override;
            //void dispatchRays(const rt::DispatchRaysArguments& args) override;
//
            //void buildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc) override;
            //void buildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlags buildFlags) override;
            //void compactBottomLevelAccelStructs() override;
            //void buildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlags buildFlags) override;
            //void buildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, nvrhi::IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances,
            //                                        rt::AccelStructBuildFlags buildFlags = rt::AccelStructBuildFlags::None) override;
//
            //void beginTimerQuery(ITimerQuery* query) override;
            //void endTimerQuery(ITimerQuery* query) override;
//
            //void beginMarker(const char* name) override;
            //void endMarker() override;
//
            //void setEnableAutomaticBarriers(bool enable) override;
            void setResourceStatesForBindingSet(IBindingSet* bindingSet) override;
//
            //void setEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers) override;
            //void setEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers) override;
//
            void beginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
            void beginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits) override;
//
            void setTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
            //void setBufferState(IBuffer* buffer, ResourceStates stateBits) override;
            //void setAccelStructState(rt::IAccelStruct* _as, ResourceStates stateBits) override;
//
            void setPermanentTextureState(ITexture* texture, ResourceStates stateBits) override;
            void setPermanentBufferState(IBuffer* buffer, ResourceStates stateBits) override;
//
            void commitBarriers() override;
//
            //ResourceStates getTextureSubresourceState(ITexture* texture, ArraySlice arraySlice, MipLevel mipLevel) override;
            //ResourceStates getBufferState(IBuffer* buffer) override;
//
            //IDevice* getDevice() override { return m_Device; }
            //const CommandListParameters& getDesc() override { return m_CommandListParameters; }
//
            TrackedCommandBufferPtr getCurrentCmdBuf() const { return m_CurrentCmdBuf; }
//
            private:
            Device* m_Device;
            const VulkanContext& m_Context;
//
            CommandListParameters m_CommandListParameters;
//
            CommandListResourceStateTracker m_StateTracker;
            bool m_EnableAutomaticBarriers = true;
//
            //// current internal command buffer
            TrackedCommandBufferPtr m_CurrentCmdBuf = nullptr;
//
            VkPipelineLayout m_CurrentPipelineLayout;
            VkShaderStageFlags m_CurrentPushConstantsVisibility;
            GraphicsState m_CurrentGraphicsState{};
            //ComputeState m_CurrentComputeState{};
            //MeshletState m_CurrentMeshletState{};
            //rt::State m_CurrentRayTracingState;
            bool m_AnyVolatileBufferWrites = false;
//
            //struct ShaderTableState
            //{
            //    vk::StridedDeviceAddressRegionKHR rayGen;
            //    vk::StridedDeviceAddressRegionKHR miss;
            //    vk::StridedDeviceAddressRegionKHR hitGroups;
            //    vk::StridedDeviceAddressRegionKHR callable;
            //    uint32_t version = 0;
            //} m_CurrentShaderTablePointers;
//
            std::unordered_map<Buffer*, VolatileBufferState> m_VolatileBufferStates;
//
            std::unique_ptr<UploadManager> m_UploadManager;
            std::unique_ptr<UploadManager> m_ScratchManager;
//
            void clearTexture(ITexture* texture, TextureSubresourceSet subresources, const VkClearColorValue& clearValue);
//
            void bindBindingSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, const BindingSetVector& bindings);
//
            void endRenderPass();
//
            void trackResourcesAndBarriers(const GraphicsState& state);
            //void trackResourcesAndBarriers(const MeshletState& state);
//
            void writeVolatileBuffer(Buffer* buffer, const void* data, size_t dataSize);
            void flushVolatileBufferWrites();
            void submitVolatileBuffers(uint64_t recordingID, uint64_t submittedID);
//
            void updateGraphicsVolatileBuffers();
            //void updateComputeVolatileBuffers();
            //void updateMeshletVolatileBuffers();
            //void updateRayTracingVolatileBuffers();
//
            void requireTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates state);
            void requireBufferState(IBuffer* buffer, ResourceStates state);
            //bool anyBarriers() const;
//
            //void buildTopLevelAccelStructInternal(AccelStruct* as, VkDeviceAddress instanceData, size_t numInstances, rt::AccelStructBuildFlags buildFlags, uint64_t currentVersion);
//
            void commitBarriersInternal();
            void commitBarriersInternal_synchronization2();
        };
    }


}