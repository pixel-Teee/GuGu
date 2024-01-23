#pragma once

#include "resource.h"

namespace GuGu{
    namespace nvrhi{
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

        //IDevice
        class IDevice : public IResource
        {
        public:
           //virtual HeapHandle createHeap(const HeapDesc& d) = 0;

           //virtual TextureHandle createTexture(const TextureDesc& d) = 0;
           //virtual MemoryRequirements getTextureMemoryRequirements(ITexture* texture) = 0;
           //virtual bool bindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) = 0;

           //virtual TextureHandle createHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) = 0;

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