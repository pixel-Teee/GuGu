#pragma once

#include "resource.h"

#define NVRHI_ENUM_CLASS_FLAG_OPERATORS(T) \
    inline T operator | (T a, T b) { return T(uint32_t(a) | uint32_t(b)); } \
    inline T operator & (T a, T b) { return T(uint32_t(a) & uint32_t(b)); } /* NOLINT(bugprone-macro-parentheses) */ \
    inline T operator ~ (T a) { return T(~uint32_t(a)); } /* NOLINT(bugprone-macro-parentheses) */ \
    inline bool operator !(T a) { return uint32_t(a) == 0; } \
    inline bool operator ==(T a, uint32_t b) { return uint32_t(a) == b; } \
    inline bool operator !=(T a, uint32_t b) { return uint32_t(a) != b; }

namespace GuGu{
    namespace nvrhi{
        struct Color
        {
            float r, g, b, a;

            Color() : r(0.f), g(0.f), b(0.f), a(0.f) { }
            Color(float c) : r(c), g(c), b(c), a(c) { }
            Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) { }

            bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
            bool operator !=(const Color& _b) const { return !(*this == _b); }
        };

        enum class GraphicsAPI : uint8_t
        {
            D3D11,
            D3D12,
            VULKAN
        };

        enum class Format : uint8_t
        {
            UNKNOWN,

            R8_UINT,
            R8_SINT,
            R8_UNORM,
            R8_SNORM,
            RG8_UINT,
            RG8_SINT,
            RG8_UNORM,
            RG8_SNORM,
            R16_UINT,
            R16_SINT,
            R16_UNORM,
            R16_SNORM,
            R16_FLOAT,
            BGRA4_UNORM,
            B5G6R5_UNORM,
            B5G5R5A1_UNORM,
            RGBA8_UINT,
            RGBA8_SINT,
            RGBA8_UNORM,
            RGBA8_SNORM,
            BGRA8_UNORM,
            SRGBA8_UNORM,
            SBGRA8_UNORM,
            R10G10B10A2_UNORM,
            R11G11B10_FLOAT,
            RG16_UINT,
            RG16_SINT,
            RG16_UNORM,
            RG16_SNORM,
            RG16_FLOAT,
            R32_UINT,
            R32_SINT,
            R32_FLOAT,
            RGBA16_UINT,
            RGBA16_SINT,
            RGBA16_FLOAT,
            RGBA16_UNORM,
            RGBA16_SNORM,
            RG32_UINT,
            RG32_SINT,
            RG32_FLOAT,
            RGB32_UINT,
            RGB32_SINT,
            RGB32_FLOAT,
            RGBA32_UINT,
            RGBA32_SINT,
            RGBA32_FLOAT,

            D16,
            D24S8,
            X24G8_UINT,
            D32,
            D32S8,
            X32G8_UINT,

            BC1_UNORM,
            BC1_UNORM_SRGB,
            BC2_UNORM,
            BC2_UNORM_SRGB,
            BC3_UNORM,
            BC3_UNORM_SRGB,
            BC4_UNORM,
            BC4_SNORM,
            BC5_UNORM,
            BC5_SNORM,
            BC6H_UFLOAT,
            BC6H_SFLOAT,
            BC7_UNORM,
            BC7_UNORM_SRGB,

            COUNT,
        };

        enum class MessageSeverity : uint8_t
        {
            Info,
            Warning,
            Error,
            Fatal
        };

        enum class CommandQueue : uint8_t
        {
            Graphics = 0,
            Compute,
            Copy,

            Count
        };

        // Flags for resources that need to be shared with other graphics APIs or other GPU devices.
        enum class SharedResourceFlags : uint32_t
        {
            None                = 0,

            // D3D11: adds D3D11_RESOURCE_MISC_SHARED
            // D3D12: adds D3D12_HEAP_FLAG_SHARED
            // Vulkan: adds vk::ExternalMemoryImageCreateInfo and vk::ExportMemoryAllocateInfo/vk::ExternalMemoryBufferCreateInfo
            Shared              = 0x01,

            // D3D11: adds (D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_SHARED_NTHANDLE)
            // D3D12, Vulkan: ignored
            Shared_NTHandle     = 0x02,

            // D3D12: adds D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER and D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER
            // D3D11, Vulkan: ignored
            Shared_CrossAdapter = 0x04,
        };

        NVRHI_ENUM_CLASS_FLAG_OPERATORS(SharedResourceFlags)

        enum class ResourceStates : uint32_t
        {
            Unknown                     = 0,
            Common                      = 0x00000001,
            ConstantBuffer              = 0x00000002,
            VertexBuffer                = 0x00000004,
            IndexBuffer                 = 0x00000008,
            IndirectArgument            = 0x00000010,
            ShaderResource              = 0x00000020,
            UnorderedAccess             = 0x00000040,
            RenderTarget                = 0x00000080,
            DepthWrite                  = 0x00000100,
            DepthRead                   = 0x00000200,
            StreamOut                   = 0x00000400,
            CopyDest                    = 0x00000800,
            CopySource                  = 0x00001000,
            ResolveDest                 = 0x00002000,
            ResolveSource               = 0x00004000,
            Present                     = 0x00008000,
            AccelStructRead             = 0x00010000,
            AccelStructWrite            = 0x00020000,
            AccelStructBuildInput       = 0x00040000,
            AccelStructBuildBlas        = 0x00080000,
            ShadingRateSurface          = 0x00100000,
            OpacityMicromapWrite        = 0x00200000,
            OpacityMicromapBuildInput   = 0x00400000,
        };

        NVRHI_ENUM_CLASS_FLAG_OPERATORS(ResourceStates)

        typedef uint32_t MipLevel;
        typedef uint32_t ArraySlice;

        enum class TextureDimension : uint8_t
        {
            Unknown,
            Texture1D,
            Texture1DArray,
            Texture2D,
            Texture2DArray,
            TextureCube,
            TextureCubeArray,
            Texture2DMS,
            Texture2DMSArray,
            Texture3D
        };

        struct TextureDesc
        {
            uint32_t width = 1;
            uint32_t height = 1;
            uint32_t depth = 1;
            uint32_t arraySize = 1;
            uint32_t mipLevels = 1;
            uint32_t sampleCount = 1;
            uint32_t sampleQuality = 0;
            Format format = Format::UNKNOWN;
            TextureDimension dimension = TextureDimension::Texture2D;
            std::string debugName;

            bool isShaderResource = true; // Note: isShaderResource is initialized to 'true' for backward compatibility
            bool isRenderTarget = false;
            bool isUAV = false;
            bool isTypeless = false;
            bool isShadingRateSurface = false;

            SharedResourceFlags sharedResourceFlags = SharedResourceFlags::None;

            // Indicates that the texture is created with no backing memory,
            // and memory is bound to the texture later using bindTextureMemory.
            // On DX12, the texture resource is created at the time of memory binding.
            bool isVirtual = false;

            Color clearValue;
            bool useClearValue = false;

            ResourceStates initialState = ResourceStates::Unknown;

            // If keepInitialState is true, command lists that use the texture will automatically
            // begin tracking the texture from the initial state and transition it to the initial state
            // on command list close.
            bool keepInitialState = false;

            constexpr TextureDesc& setWidth(uint32_t value) { width = value; return *this; }
            constexpr TextureDesc& setHeight(uint32_t value) { height = value; return *this; }
            constexpr TextureDesc& setDepth(uint32_t value) { depth = value; return *this; }
            constexpr TextureDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
            constexpr TextureDesc& setMipLevels(uint32_t value) { mipLevels = value; return *this; }
            constexpr TextureDesc& setSampleCount(uint32_t value) { sampleCount = value; return *this; }
            constexpr TextureDesc& setSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
            constexpr TextureDesc& setFormat(Format value) { format = value; return *this; }
            constexpr TextureDesc& setDimension(TextureDimension value) { dimension = value; return *this; }
            TextureDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
            constexpr TextureDesc& setIsRenderTarget(bool value) { isRenderTarget = value; return *this; }
            constexpr TextureDesc& setIsUAV(bool value) { isUAV = value; return *this; }
            constexpr TextureDesc& setIsTypeless(bool value) { isTypeless = value; return *this; }
            constexpr TextureDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
            constexpr TextureDesc& setClearValue(const Color& value) { clearValue = value; useClearValue = true; return *this; }
            constexpr TextureDesc& setUseClearValue(bool value) { useClearValue = value; return *this; }
            constexpr TextureDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
            constexpr TextureDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
        };

        // describes a 2D section of a single mip level + single slice of a texture
        struct TextureSlice
        {
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t z = 0;
            // -1 means the entire dimension is part of the region
            // resolve() below will translate these values into actual dimensions
            uint32_t width = uint32_t(-1);
            uint32_t height = uint32_t(-1);
            uint32_t depth = uint32_t(-1);

            MipLevel mipLevel = 0;
            ArraySlice arraySlice = 0;

            [[nodiscard]] TextureSlice resolve(const TextureDesc& desc) const;

            constexpr TextureSlice& setOrigin(uint32_t vx = 0, uint32_t vy = 0, uint32_t vz = 0) { x = vx; y = vy; z = vz; return *this; }
            constexpr TextureSlice& setWidth(uint32_t value) { width = value; return *this; }
            constexpr TextureSlice& setHeight(uint32_t value) { height = value; return *this; }
            constexpr TextureSlice& setDepth(uint32_t value) { depth = value; return *this; }
            constexpr TextureSlice& setSize(uint32_t vx = uint32_t(-1), uint32_t vy = uint32_t(-1), uint32_t vz = uint32_t(-1)) { width = vx; height = vy; depth = vz; return *this; }
            constexpr TextureSlice& setMipLevel(MipLevel level) { mipLevel = level; return *this; }
            constexpr TextureSlice& setArraySlice(ArraySlice slice) { arraySlice = slice; return *this; }
        };

        struct TextureSubresourceSet
        {
            static constexpr MipLevel AllMipLevels = MipLevel(-1);
            static constexpr ArraySlice AllArraySlices = ArraySlice(-1);

            MipLevel baseMipLevel = 0;
            MipLevel numMipLevels = 1;
            ArraySlice baseArraySlice = 0;
            ArraySlice numArraySlices = 1;

            TextureSubresourceSet() = default;

            TextureSubresourceSet(MipLevel _baseMipLevel, MipLevel _numMipLevels, ArraySlice _baseArraySlice, ArraySlice _numArraySlices)
                    : baseMipLevel(_baseMipLevel)
                    , numMipLevels(_numMipLevels)
                    , baseArraySlice(_baseArraySlice)
                    , numArraySlices(_numArraySlices)
            {
            }

            [[nodiscard]] TextureSubresourceSet resolve(const TextureDesc& desc, bool singleMipLevel) const;
            [[nodiscard]] bool isEntireTexture(const TextureDesc& desc) const;

            bool operator ==(const TextureSubresourceSet& other) const
            {
                return baseMipLevel == other.baseMipLevel &&
                       numMipLevels == other.numMipLevels &&
                       baseArraySlice == other.baseArraySlice &&
                       numArraySlices == other.numArraySlices;
            }
            bool operator !=(const TextureSubresourceSet& other) const { return !(*this == other); }

            constexpr TextureSubresourceSet& setBaseMipLevel(MipLevel value) { baseMipLevel = value; return *this; }
            constexpr TextureSubresourceSet& setNumMipLevels(MipLevel value) { numMipLevels = value; return *this; }
            constexpr TextureSubresourceSet& setMipLevels(MipLevel base, MipLevel num) { baseMipLevel = base; numMipLevels = num; return *this; }
            constexpr TextureSubresourceSet& setBaseArraySlice(ArraySlice value) { baseArraySlice = value; return *this; }
            constexpr TextureSubresourceSet& setNumArraySlices(ArraySlice value) { numArraySlices = value; return *this; }
            constexpr TextureSubresourceSet& setArraySlices(ArraySlice base, ArraySlice num) { baseArraySlice = base; numArraySlices = num; return *this; }

            // see the bottom of this file for a specialization of std::hash<TextureSubresourceSet>
        };

        static const TextureSubresourceSet AllSubresources = TextureSubresourceSet(0, TextureSubresourceSet::AllMipLevels, 0, TextureSubresourceSet::AllArraySlices);

        class ITexture : public IResource
        {
        public:
            [[nodiscard]] virtual const TextureDesc& getDesc() const = 0;

            // Similar to getNativeObject, returns a native view for a specified set of subresources. Returns nullptr if unavailable.
            // TODO: on D3D12, the views might become invalid later if the view heap is grown/reallocated, we should do something about that.
            virtual Object getNativeView(ObjectType objectType, Format format = Format::UNKNOWN, TextureSubresourceSet subresources = AllSubresources, TextureDimension dimension = TextureDimension::Unknown, bool isReadOnlyDSV = false) = 0;
        };
        typedef RefCountPtr<ITexture> TextureHandle;

        // IMessageCallback should be implemented by the application.
        class IMessageCallback
        {
        protected:
            IMessageCallback() = default;
            virtual ~IMessageCallback() = default;

        public:
            // NVRHI will call message(...) whenever it needs to signal something.
            // The application is free to ignore the messages, show message boxes, or terminate.
            virtual void message(MessageSeverity severity, const char* messageText) = 0;

            IMessageCallback(const IMessageCallback&) = delete;
            IMessageCallback(const IMessageCallback&&) = delete;
            IMessageCallback& operator=(const IMessageCallback&) = delete;
            IMessageCallback& operator=(const IMessageCallback&&) = delete;
        };

        //IDevice
        class IDevice : public IResource
        {
        public:
           //virtual HeapHandle createHeap(const HeapDesc& d) = 0;

           //virtual TextureHandle createTexture(const TextureDesc& d) = 0;
           //virtual MemoryRequirements getTextureMemoryRequirements(ITexture* texture) = 0;
           //virtual bool bindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) = 0;

           virtual TextureHandle createHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) = 0;

           //virtual StagingTextureHandle createStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess) = 0;
           //virtual void *mapStagingTexture(IStagingTexture* tex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t *outRowPitch) = 0;
           //virtual void unmapStagingTexture(IStagingTexture* tex) = 0;

           //virtual BufferHandle createBuffer(const BufferDesc& d) = 0;
           //virtual void *mapBuffer(IBuffer* buffer, CpuAccessMode cpuAccess) = 0;
           //virtual void unmapBuffer(IBuffer* buffer) = 0;
           //virtual MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) = 0;
           //virtual bool bindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) = 0;

           //virtual BufferHandle createHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& desc) = 0;

           //virtual ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) = 0;
           //virtual ShaderHandle createShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) = 0;
           //virtual ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) = 0;

           //virtual SamplerHandle createSampler(const SamplerDesc& d) = 0;

           //// Note: vertexShader is only necessary on D3D11, otherwise it may be null
           //virtual InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader) = 0;

           //// Event queries
           //virtual EventQueryHandle createEventQuery() = 0;
           //virtual void setEventQuery(IEventQuery* query, CommandQueue queue) = 0;
           //virtual bool pollEventQuery(IEventQuery* query) = 0;
           //virtual void waitEventQuery(IEventQuery* query) = 0;
           //virtual void resetEventQuery(IEventQuery* query) = 0;

           //// Timer queries - see also begin/endTimerQuery in ICommandList
           //virtual TimerQueryHandle createTimerQuery() = 0;
           //virtual bool pollTimerQuery(ITimerQuery* query) = 0;
           //// returns time in seconds
           //virtual float getTimerQueryTime(ITimerQuery* query) = 0;
           //virtual void resetTimerQuery(ITimerQuery* query) = 0;

           //// Returns the API kind that the RHI backend is running on top of.
           //virtual GraphicsAPI getGraphicsAPI() = 0;

           //virtual FramebufferHandle createFramebuffer(const FramebufferDesc& desc) = 0;

           //virtual GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) = 0;

           //virtual ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) = 0;

           //virtual MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) = 0;

           //virtual rt::PipelineHandle createRayTracingPipeline(const rt::PipelineDesc& desc) = 0;

           //virtual BindingLayoutHandle createBindingLayout(const BindingLayoutDesc& desc) = 0;
           //virtual BindingLayoutHandle createBindlessLayout(const BindlessLayoutDesc& desc) = 0;

           //virtual BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) = 0;
           //virtual DescriptorTableHandle createDescriptorTable(IBindingLayout* layout) = 0;

           //virtual void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) = 0;
           //virtual bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) = 0;

           //virtual rt::OpacityMicromapHandle createOpacityMicromap(const rt::OpacityMicromapDesc& desc) = 0;
           //virtual rt::AccelStructHandle createAccelStruct(const rt::AccelStructDesc& desc) = 0;
           //virtual MemoryRequirements getAccelStructMemoryRequirements(rt::IAccelStruct* as) = 0;
           //virtual bool bindAccelStructMemory(rt::IAccelStruct* as, IHeap* heap, uint64_t offset) = 0;

           //virtual CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) = 0;
           //virtual uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) = 0;
           //virtual void queueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) = 0;
           //virtual void waitForIdle() = 0;

           //// Releases the resources that were referenced in the command lists that have finished executing.
           //// IMPORTANT: Call this method at least once per frame.
           //virtual void runGarbageCollection() = 0;

           //virtual bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) = 0;

           //virtual FormatSupport queryFormatSupport(Format format) = 0;

           //virtual Object getNativeQueue(ObjectType objectType, CommandQueue queue) = 0;

           //virtual IMessageCallback* getMessageCallback() = 0;

           //// Front-end for executeCommandLists(..., 1) for compatibility and convenience
           //uint64_t executeCommandList(ICommandList* commandList, CommandQueue executionQueue = CommandQueue::Graphics)
           //{
           //    return executeCommandLists(&commandList, 1, executionQueue);
           //}
        };
    }


}