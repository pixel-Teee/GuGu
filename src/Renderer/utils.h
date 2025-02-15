#pragma once
#include <vector>
#include <mutex>

#include <Renderer/nvrhi.h>

namespace GuGu{
    class GuGuUtf8Str;
    namespace nvrhi::utils{
        BufferDesc CreateStaticConstantBufferDesc(
                uint32_t byteSize,
        const char* debugName);

        class BitSetAllocator{
        public:
            explicit BitSetAllocator(size_t capacity, bool multithreaded);

            int32_t allocate();
            void release(int32_t index);
            size_t getCapacity() const { return m_Allocated.size(); }
        private:
            int32_t m_NextAvailable = 0;
            std::vector<bool> m_Allocated;
            bool m_MultiThreaded;
            std::mutex m_Mutex;
        };

        void ClearColorAttachment(
                ICommandList* commandList,
                IFramebuffer* framebuffer,
                uint32_t attachmentIndex,
                Color color
        );

        bool CreateBindingSetAndLayout(
                IDevice* device,
                nvrhi::ShaderType visibility,
                uint32_t registerSpace,
                const BindingSetDesc& bindingSetDesc,
                BindingLayoutHandle& bindingLayout,
                BindingSetHandle& bindingSet);

		Format ChooseFormat(
			IDevice* device,
			nvrhi::FormatSupport requiredFeatures,
			const nvrhi::Format* requestedFormats,
			size_t requestedFormatCount);

        void InvalidEnum();
        void NotSupported();
        const char* DebugNameToString(const GuGuUtf8Str& debugName);
    }
}